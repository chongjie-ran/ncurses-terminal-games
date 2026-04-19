"""
P2 向量嵌入证据匹配器

用于 AIOps v4 Q5 质量门禁的语义匹配增强。
使用 bge-small-zh-v1.5 模型进行中文优化的语义相似度匹配。
"""

from __future__ import annotations

import logging
import time
from typing import Optional

import numpy as np

# 条件导入 sentence-transformers
try:
    from sentence_transformers import SentenceTransformer

    HAS_SENTENCE_TRANSFORMERS = True
except ImportError:
    HAS_SENTENCE_TRANSFORMERS = False
    SentenceTransformer = None  # type: ignore

logger = logging.getLogger(__name__)

# 置信度阈值
HIGH_CONFIDENCE = 0.85
LOW_CONFIDENCE = 0.75


class EvidenceMatcher:
    """
    向量嵌入证据匹配器

    使用 bge-small-zh-v1.5 模型进行语义匹配。
    作为 P0/P1 规则匹配的 Fallback 层。

    使用方式:
        matcher = EvidenceMatcher()
        is_match = matcher.is_semantic_match("mysql服务", ["mysql_service", "postgres_db"])
    """

    _instance: Optional[EvidenceMatcher] = None
    _model = None
    _evidence_embeddings: Optional[np.ndarray] = None
    _evidence_names: list[str] = []
    _last_build_time: float = 0
    _cache: dict[str, tuple[bool, float, float]] = {}
    _cache_ttl: int = 300  # 5分钟缓存

    def __init__(
        self,
        model_name: str = "BAAI/bge-small-zh-v1.5",
        cache_ttl: int = 300,
    ):
        """
        初始化匹配器

        Args:
            model_name: HuggingFace 模型名
            cache_ttl: 缓存过期时间（秒）
        """
        self._model_name = model_name
        self._cache_ttl = cache_ttl
        self._initialized = False

    def _ensure_model(self) -> bool:
        """懒加载模型"""
        if self._model is not None:
            return True

        if not HAS_SENTENCE_TRANSFORMERS:
            logger.info(
                "[EvidenceMatcher] sentence-transformers not installed, "
                "falling back to rule-based matching",
            )
            return False

        try:
            self._model = SentenceTransformer(self._model_name)
            logger.debug("[EvidenceMatcher] Loaded model: %s", self._model_name)
            return True
        except Exception as e:
            logger.info("[EvidenceMatcher] Failed to load model: %s", e)
            return False

    def build_index(self, evidence_names: list[str]) -> None:
        """
        构建 evidence 向量索引

        Args:
            evidence_names: evidence 名称列表
        """
        if not evidence_names:
            return

        self._evidence_names = list(evidence_names)

        if not self._ensure_model():
            return

        # 批量向量化
        self._evidence_embeddings = self._model.encode(
            evidence_names,
            convert_to_numpy=True,
            normalize_embeddings=True,
        )
        self._last_build_time = time.time()

        logger.debug(
            "[EvidenceMatcher] Built index with %s items",
            len(evidence_names),
        )

    def _cosine_similarity(self, a: np.ndarray, B: np.ndarray) -> np.ndarray:
        """计算余弦相似度"""
        norm_a = np.linalg.norm(a)
        if norm_a == 0:
            return np.zeros(len(B))
        norm_B = np.linalg.norm(B, axis=1)
        norm_B[norm_B == 0] = 1  # 避免除零
        return np.dot(B, a) / (norm_a * norm_B)

    def is_semantic_match(
        self,
        sample: str,
        evidence_names: list[str] | None = None,
        threshold: float = LOW_CONFIDENCE,
    ) -> tuple[bool, float]:
        """
        检查 sample 是否与 evidence 语义匹配

        Args:
            sample: 待匹配文本
            evidence_names: 可选，临时指定 evidence 列表
            threshold: 相似度阈值

        Returns:
            (is_matched, max_similarity)
        """
        # 检查缓存
        cache_key = f"{sample}:{','.join(evidence_names or [])}"
        if cache_key in self._cache:
            result, max_sim, timestamp = self._cache[cache_key]
            if time.time() - timestamp < self._cache_ttl:
                return result, max_sim

        # 如果提供了新的 evidence 列表，重新构建索引
        if evidence_names is not None:
            if (
                evidence_names != self._evidence_names
                or self._evidence_embeddings is None
            ):
                self.build_index(evidence_names)

        # 如果没有索引，无法匹配
        if self._evidence_embeddings is None or len(self._evidence_names) == 0:
            return False, 0.0

        # 确保模型已加载
        if not self._ensure_model():
            return False, 0.0

        try:
            # 向量化 sample
            sample_vec = self._model.encode(
                [sample],
                convert_to_numpy=True,
                normalize_embeddings=True,
            )[0]

            # 计算相似度
            similarities = self._cosine_similarity(
                sample_vec, self._evidence_embeddings
            )
            max_sim = float(np.max(similarities))

            result = max_sim >= threshold

            # 缓存结果
            self._cache[cache_key] = (result, max_sim, time.time())

            return result, max_sim

        except Exception as e:
            logger.debug("[EvidenceMatcher] Semantic match failed: %s", e)
            return False, 0.0

    def find_top_matches(
        self,
        sample: str,
        evidence_names: list[str] | None = None,
        top_k: int = 3,
    ) -> list[tuple[str, float]]:
        """
        查找 top-k 最相似的 evidence

        Args:
            sample: 待匹配文本
            evidence_names: 可选，临时指定 evidence 列表
            top_k: 返回前 k 个匹配

        Returns:
            [(evidence_name, similarity), ...]
        """
        if evidence_names is not None:
            if (
                evidence_names != self._evidence_names
                or self._evidence_embeddings is None
            ):
                self.build_index(evidence_names)

        if self._evidence_embeddings is None or len(self._evidence_names) == 0:
            return []

        if not self._ensure_model():
            return []

        try:
            sample_vec = self._model.encode(
                [sample],
                convert_to_numpy=True,
                normalize_embeddings=True,
            )[0]

            similarities = self._cosine_similarity(
                sample_vec, self._evidence_embeddings
            )

            # 获取 top-k
            top_indices = np.argsort(similarities)[-top_k:][::-1]

            return [
                (self._evidence_names[i], float(similarities[i]))
                for i in top_indices
            ]

        except Exception as e:
            logger.debug("[EvidenceMatcher] find_top_matches failed: %s", e)
            return []

    def clear_cache(self) -> None:
        """清空缓存"""
        self._cache.clear()

    def reset(self) -> None:
        """重置匹配器"""
        self._model = None
        self._evidence_embeddings = None
        self._evidence_names = []
        self._last_build_time = 0
        self._cache.clear()
        self._initialized = False


# 全局单例
_matcher: Optional[EvidenceMatcher] = None


def get_matcher() -> EvidenceMatcher:
    """获取全局匹配器单例"""
    global _matcher
    if _matcher is None:
        _matcher = EvidenceMatcher()
    return _matcher


# 便捷函数
def is_semantic_match(
    sample: str,
    evidence_names: list[str],
    threshold: float = LOW_CONFIDENCE,
) -> tuple[bool, float]:
    """
    便捷函数：检查 sample 是否与 evidence 语义匹配

    Args:
        sample: 待匹配文本
        evidence_names: evidence 名称列表
        threshold: 相似度阈值

    Returns:
        (is_matched, max_similarity)
    """
    matcher = get_matcher()
    return matcher.is_semantic_match(sample, evidence_names, threshold)


if __name__ == "__main__":
    # 简单测试
    logger.info("Testing EvidenceMatcher...")

    if not HAS_SENTENCE_TRANSFORMERS:
        logger.info("ERROR: sentence-transformers not installed")
        logger.info("Install with: pip install sentence-transformers")
        exit(1)

    matcher = EvidenceMatcher()

    # 构建索引
    evidence = [
        "mysql_master_01",
        "mariadb_10324",
        "postgres_db_primary",
        "order_service",
        "用户数据库",
        "mariadb主从高可用",
    ]
    matcher.build_index(evidence)

    # 测试用例
    test_cases = [
        ("mysql主库", 0.75),
        ("mariadb主从高可用", 0.75),
        ("mysql_master", 0.75),
        ("订单服务", 0.75),
        ("postgresql数据库", 0.75),
        ("pg主库", 0.75),
        ("unknown_xyz", 0.75),
    ]

    logger.info("\nTest Results:")
    for sample, threshold in test_cases:
        matched, sim = matcher.is_semantic_match(sample, threshold=threshold)
        logger.info("  '%s' -> matched=%s, similarity=%.3f", sample, matched, sim)

    # 查找 top-k
    logger.info("\nTop matches for 'mysql服务':")
    for name, sim in matcher.find_top_matches("mysql服务"):
        logger.info("  %s: %.3f", name, sim)
