"""
Child execution loop that uses nanobot AgentRunner when a real LLMProvider is available.
Falls back to single provider.run() call when using stubs or simple sync providers.
"""
from __future__ import annotations

import asyncio
import json
import logging
import os
from time import monotonic

# Model used for rescue calls — matches sub_agent_runner default
_RESCUE_MODEL = os.environ.get("AIOPS_SUB_AGENT_MODEL", "claude-haiku-4-5-20251001")

# Substring that identifies the nanobot "max iterations exhausted" message
_MAX_ITER_MARKER = "reached the maximum number of tool call iterations"

# Prompt appended to conversation history in a rescue call
_RESCUE_PROMPT = (
    "你已达到最大工具调用次数。请立即基于以上工具调用的结果，"
    "输出符合要求的 JSON 格式最终结论，不要再调用任何工具。"
    "若数据不完整，请在 summary 中注明并设置 needs_followup=true，confidence 设为较低值（≤0.5）。"
)
_FINALIZATION_RETRY_PROMPT = "Please provide your response to the user based on the conversation above."
_EMPTY_FINAL_RESPONSE_MARKER = "I completed the tool steps but couldn't produce a final answer."

logger = logging.getLogger(__name__)


def _log(agent_id: str, msg: str) -> None:
    logger.debug("[child-loop:%s] %s", agent_id, msg)


def _first_turn_tool_choice_for_agent(agent_id: str) -> str | None:
    """resource-resolver 首轮默认强制至少一次 tool call，避免模型直接输出 JSON 幻觉。

    设置 ``AIOPS_RESOURCE_RESOLVER_FIRST_TOOL_CHOICE=auto``（或 off/false/0）可恢复旧行为。
    """
    if agent_id != "resource-resolver":
        return None
    raw = os.environ.get("AIOPS_RESOURCE_RESOLVER_FIRST_TOOL_CHOICE", "required").strip()
    if raw.lower() in ("auto", "off", "false", "0", ""):
        return None
    return raw


def _registry_tool_names(tool_registry) -> list[str]:
    if tool_registry is None:
        return []
    if hasattr(tool_registry, "list_tools"):
        try:
            return [t.name for t in tool_registry.list_tools()]
        except Exception:
            pass
    names = getattr(tool_registry, "tool_names", None)
    if names is not None:
        return list(names)
    return []


def _is_fence_only_response(raw: str) -> bool:
    normalized = (raw or "").strip().lower()
    return normalized in {"```", "```json"}


def provider_supports_agent_runner(provider) -> bool:
    """True if provider has an async chat() method (real nanobot LLMProvider)."""
    return callable(getattr(provider, "chat", None))


def _get_nanobot_provider(provider):
    """Extract underlying nanobot LLMProvider from a v4 adapter wrapper, if available."""
    # Try common attribute names for the wrapped provider
    for attr in ("_provider", "_llm_provider", "provider", "llm_provider"):
        inner = getattr(provider, attr, None)
        if inner is not None and provider_supports_agent_runner(inner):
            return inner
    # Provider itself might be a real LLMProvider
    if provider_supports_agent_runner(provider):
        return provider
    return None


def run_child_with_tools(
    *,
    system_prompt: str,
    user_prompt: str,
    provider,
    tool_registry,
    max_iterations: int,
    timeout: int,
    agent_id: str = "",
    clock=monotonic,
    slot=None,
    first_turn_tool_choice: str | None = None,
) -> dict:
    """Run a child agent using AgentRunner + real tools if available, else single-call fallback.

    Returns the same dict structure as execute_child_runtime:
    {raw_response, payload, runtime_metadata}
    """
    nanobot_provider = _get_nanobot_provider(provider)

    if nanobot_provider is None or tool_registry is None:
        # Fallback: single synchronous call
        _log(agent_id, f"path=single-call (nanobot_provider={'None' if nanobot_provider is None else 'ok'}, "
             f"tool_registry={'None' if tool_registry is None else 'ok'})")
        started_at = clock()
        raw_response = provider.run(system_prompt, user_prompt)
        elapsed = clock() - started_at
        try:
            payload = json.loads(raw_response)
        except json.JSONDecodeError:
            payload = None
        return {
            "raw_response": raw_response,
            "payload": payload,
            "runtime_metadata": {
                "iterations_used": 1,
                "max_iterations": max_iterations,
                "timeout_seconds": timeout,
                "timeout_hit": elapsed > timeout,
                "stop_condition_hit": None,
                "agent_runner_used": False,
                "visible_tools": _registry_tool_names(tool_registry),
                "visible_skills": [],
            },
        }

    # Real path: use nanobot AgentRunner
    _log(agent_id, "path=agent_runner")
    return _run_with_agent_runner(
        system_prompt=system_prompt,
        user_prompt=user_prompt,
        nanobot_provider=nanobot_provider,
        tool_registry=tool_registry,
        max_iterations=max_iterations,
        timeout=timeout,
        agent_id=agent_id,
        clock=clock,
        adapter=provider,  # pass through so the adapter's shared loop can be used
        slot=slot,
        first_turn_tool_choice=first_turn_tool_choice,
    )


async def _try_rescue_call(provider, messages: list[dict], agent_id: str) -> str:
    """One final no-tools LLM call using the full conversation history.

    When the agent runner exhausts its iterations, the full message list
    (including all tool results collected during those iterations) is still
    available in ``AgentRunResult.messages``.  We append a rescue prompt and
    call the LLM once more — without tools — so it can format whatever it has
    gathered into the required JSON output.

    Returns the rescue response content, or "" on failure.
    """
    has_tool_results = any(m.get("role") == "tool" for m in messages)
    if not has_tool_results:
        _log(agent_id, "rescue-call skipped: no tool results in history")
        return ""
    rescue_messages = list(messages) + [{"role": "user", "content": _RESCUE_PROMPT}]
    try:
        response = await provider.chat(messages=rescue_messages, model=_RESCUE_MODEL)
        content = response.content or ""
        _log(agent_id, f"rescue-call response={content!r:.120}")
        return content
    except Exception as exc:
        _log(agent_id, f"rescue-call failed: {exc}")
        return ""


async def _try_finalization_call(provider, messages: list[dict], agent_id: str) -> str:
    """Fallback call when runner returns empty-final-response placeholder."""
    finalization_messages = list(messages) + [{"role": "user", "content": _FINALIZATION_RETRY_PROMPT}]
    try:
        response = await provider.chat(messages=finalization_messages, model=_RESCUE_MODEL)
        content = response.content or ""
        _log(agent_id, f"finalization-call response={content!r:.120}")
        return content
    except Exception as exc:
        _log(agent_id, f"finalization-call failed: {exc}")
        return ""


def _run_with_agent_runner(
    *,
    system_prompt: str,
    user_prompt: str,
    nanobot_provider,
    tool_registry,
    max_iterations: int,
    timeout: int,
    agent_id: str,
    clock,
    adapter=None,
    slot=None,
    first_turn_tool_choice: str | None = None,
) -> dict:
    """Invoke run_sub_agent from aiops_v3 and return structured result dict.

    *adapter* — if provided and has a ``run_coroutine`` method, the sub-agent
    coroutine is submitted to the adapter's shared background loop.  This
    avoids cross-loop aiohttp session conflicts when the LLM provider caches
    its HTTP session in that loop.

    When the agent runner exhausts its iterations without producing JSON,
    a rescue call is made using the accumulated tool results so the agent can
    still output a (possibly partial) JSON response.
    """
    started_at = clock()
    try:
        from aiops_v3.agents.sub_agent_runner import run_sub_agent

        # Build a checkpoint callback that forwards tool events to the slot.
        # The callback is async (AgentRunner awaits it) and runs on the adapter
        # shared loop, so slot._put() (thread-safe) is safe to call from here.
        async def _checkpoint(payload: dict) -> None:
            if slot is None:
                return
            phase = payload.get("phase")
            if phase == "awaiting_tools":
                for tc in payload.get("pending_tool_calls", []):
                    tool_name = (tc.get("function") or {}).get("name") or tc.get("name") or "?"
                    fn = getattr(slot, "tool_call", None)
                    if fn is not None:
                        fn(agent_id, tool_name)
            elif phase == "tools_completed":
                for tr in payload.get("completed_tool_results", []):
                    tool_name = tr.get("name") or "?"
                    content = tr.get("content") or ""
                    brief = content[:80] if isinstance(content, str) else str(content)[:80]
                    fn = getattr(slot, "tool_result", None)
                    if fn is not None:
                        fn(agent_id, tool_name, brief)

        async def _run():
            result = await run_sub_agent(
                provider=nanobot_provider,
                system_prompt=system_prompt,
                user_message=user_prompt,
                tools=tool_registry,
                max_iterations=max_iterations,
                session_key=agent_id or None,
                checkpoint_callback=_checkpoint,
                first_turn_tool_choice=first_turn_tool_choice,
            )
            raw = result.final_content or ""

            # Rescue path: fallback when final output is incomplete for JSON parsing.
            rescue_reason = None
            if _MAX_ITER_MARKER in raw:
                rescue_reason = "max-iterations-marker"
            elif _is_fence_only_response(raw) or not raw.strip():
                rescue_reason = "empty-or-fence-only"
            elif _EMPTY_FINAL_RESPONSE_MARKER in raw:
                rescue_reason = "empty-final-response-marker"

            if rescue_reason:
                _log(agent_id, f"attempting rescue call ({rescue_reason})")
                if rescue_reason == "empty-final-response-marker":
                    rescued = await _try_finalization_call(nanobot_provider, result.messages, agent_id)
                else:
                    rescued = await _try_rescue_call(nanobot_provider, result.messages, agent_id)
                if rescued:
                    raw = rescued

            return result, raw

        # Prefer the adapter's shared loop so the provider's HTTP session
        # (created in that loop) is reused without cross-loop conflicts.
        if adapter is not None and callable(getattr(adapter, "run_coroutine", None)):
            _log(agent_id, "path=agent_runner using adapter shared loop")
            result, raw_response = adapter.run_coroutine(_run())
        else:
            try:
                result, raw_response = asyncio.run(_run())
            except RuntimeError:
                # Event loop already running (e.g. Jupyter / async context)
                loop = asyncio.get_event_loop()
                result, raw_response = loop.run_until_complete(_run())

        elapsed = clock() - started_at
        # AgentRunResult has no iterations_used field; derive from tool_events length.
        tool_calls_made = len(getattr(result, "tool_events", []) or [])
        stop_reason = getattr(result, "stop_reason", "completed")
        _log(agent_id, f"agent_runner done in {elapsed:.1f}s  "
             f"tool_calls={tool_calls_made}  stop_reason={stop_reason!r}  "
             f"raw={raw_response!r:.120}")

        try:
            payload = json.loads(raw_response)
        except json.JSONDecodeError:
            payload = None

        return {
            "raw_response": raw_response,
            "payload": payload,
            "runtime_metadata": {
                "iterations_used": tool_calls_made,
                "max_iterations": max_iterations,
                "timeout_seconds": timeout,
                "timeout_hit": elapsed > timeout,
                "stop_condition_hit": None,
                "stop_reason": stop_reason,
                "agent_runner_used": True,
                "rescue_call_used": (
                    _MAX_ITER_MARKER in (result.final_content or "")
                    or _is_fence_only_response(result.final_content or "")
                    or not (result.final_content or "").strip()
                ),
                "visible_tools": _registry_tool_names(tool_registry),
                "visible_skills": [],
            },
        }
    except Exception as exc:
        elapsed = clock() - started_at
        _log(agent_id, f"ERROR: agent_runner exception: {exc}")
        return {
            "raw_response": "",
            "payload": None,
            "runtime_metadata": {
                "iterations_used": 0,
                "max_iterations": max_iterations,
                "timeout_seconds": timeout,
                "timeout_hit": elapsed > timeout,
                "stop_condition_hit": None,
                "agent_runner_used": False,
                "error": str(exc),
                "visible_tools": [],
                "visible_skills": [],
            },
        }
