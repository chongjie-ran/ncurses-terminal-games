#!/bin/bash
# WASM Game Builder - Build & Test All WASM Games
# Usage: ./build-all.sh [--games "snake,wordle,2048"] [--test-only] [--compile-only]

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECTS_DIR="$(dirname "$SCRIPT_DIR")"
EMSCRIPTEN_SETUP="source ~/emsdk/emsdk_env.sh > /dev/null 2>&1"

# Parse arguments
GAMES_FILTER=""
TEST_ONLY=false
COMPILE_ONLY=false
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --games) GAMES_FILTER="$2"; shift 2 ;;
        --test-only) TEST_ONLY=true; shift ;;
        --compile-only) COMPILE_ONLY=true; shift ;;
        --verbose) VERBOSE=true; shift ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
done

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; BLUE='\033[0;34m'; NC='\033[0m'

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_ok()   { echo -e "${GREEN}[OK]${NC}   $1"; }
log_skip() { echo -e "${YELLOW}[SKIP]${NC} $1"; }
log_fail() { echo -e "${RED}[FAIL]${NC} $1"; }

# Capitalize first letter - portable (works on bash 3+ and zsh)
upper_first() {
    local str="$1"
    local first="${str:0:1}"
    local rest="${str:1}"
    echo "${first}${rest}" | sed 's/./\U&/'
}

find_wasm_games() {
    find "$PROJECTS_DIR" -maxdepth 1 -type d -name "*-wasm" 2>/dev/null | sort
}

basename_dir() { basename "$1"; }

compile_game() {
    local game_dir="$1"
    local name=$(basename_dir "$game_dir")
    local game_c="$game_dir/game.c"
    local wasm_main="$game_dir/wasm_main.c"
    
    if [[ ! -f "$game_c" || ! -f "$wasm_main" ]]; then
        log_skip "$name: missing game.c or wasm_main.c"
        return 1
    fi
    
    if [[ -f "$game_dir/${name}.wasm" && -f "$game_dir/${name}.js" ]]; then
        local src_newer
        src_newer=$(find "$game_c" "$wasm_main" -newer "$game_dir/${name}.wasm" 2>/dev/null | head -1)
        if [[ -z "$src_newer" ]]; then
            log_skip "$name: already compiled (up-to-date)"
            return 0
        fi
    fi
    
    log_info "Compiling $name..."
    
    # Extract exported function names from wasm_main.c (portable, works with mawk)
    # EMSCRIPTEN_KEEPALIVE is on its own line before each function
    local exports
    exports=$(grep -v "^EMSCRIPTEN_KEEPALIVE$" "$wasm_main" 2>/dev/null | \
        grep -Eo '\bwasm_[a-zA-Z0-9_]+\(' | sed 's/($//' | sort -u)
    
    # Build EXPORTED_FUNCTIONS
    # Build EXPORTED_FUNCTIONS as a proper bash array then serialize
    local func_names=()
    while IFS= read -r func; do
        [[ -n "$func" ]] && func_names+=("${func}")
    done <<< "$exports"
    
    # Serialize to JSON-like string: '["_func1", "_func2", ...]'
    local func_list_json="["
    for i in "${!func_names[@]}"; do
        [[ $i -gt 0 ]] && func_list_json+=", "
        func_list_json+="\"_${func_names[$i]}\""
    done
    func_list_json+="]"
    
    # Build module name (e.g., wordle-wasm -> Wordle)
    # Remove -wasm suffix, then capitalize first letter
    local mod_name
    # Remove -wasm suffix, capitalize each word (handle hyphens)
    mod_name=$(echo "$name" | sed 's/-wasm$//' | sed 's/-/ /g' | awk '{for(i=1;i<=NF;i++) $i=toupper(substr($i,1,1)) tolower(substr($i,2)); print}' | tr -d ' ')
    
    eval "$EMSCRIPTEN_SETUP"
    
    set +e
    emcc -O3 \
        -s MODULARIZE=1 \
        -s EXPORT_NAME="${mod_name}Module" \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s TOTAL_MEMORY=64MB \
        -s EXPORTED_FUNCTIONS="$func_list_json" \
        --no-entry -o "${game_dir}/${name}.js" \
        "$game_c" "$wasm_main" 2>&1
    local compile_status=$?
    set -e
    
    if [[ $compile_status -eq 0 && -f "${game_dir}/${name}.wasm" ]]; then
        log_ok "$name: compiled successfully"
        return 0
    else
        log_fail "$name: compilation failed (exit $compile_status)"
        return 1
    fi
}

generate_test() {
    local game_dir="$1"
    local name=$(basename_dir "$game_dir")
    local test_file="$game_dir/test.spec.js"
    
    # Build display name (strip -wasm suffix for title matching)
    local display_name
    display_name=$(echo "$name" | sed 's/-wasm$//')
    
    cat > "$test_file" << TESTEOF
const { test, expect, _electron_electron = require('child_process').exec } = require('@playwright/test');
const path = require('path');

test.describe('$name WASM game tests', () => {
    let server;
    
    test.beforeAll(async () => {
        // Start a simple HTTP server for the game files
        const http = require('http');
        const fs = require('fs');
        const games_dir = path.dirname(__dirname);
        
        server = http.createServer((req, res) => {
            let filePath = path.join(__dirname, req.url === '/' ? 'index.html' : req.url);
            const ext = path.extname(filePath);
            const mimeTypes = {
                '.html': 'text/html',
                '.js': 'application/javascript',
                '.wasm': 'application/wasm',
            };
            
            fs.readFile(filePath, (err, data) => {
                if (err) {
                    res.writeHead(404);
                    res.end('Not found');
                    return;
                }
                res.writeHead(200, { 'Content-Type': mimeTypes[ext] || 'application/octet-stream' });
                res.end(data);
            });
        });
        
        await new Promise(resolve => server.listen(8765, resolve));
    });
    
    test.afterAll(async () => {
        if (server) server.close();
    });
    
    test('should load without errors', async ({ page }) => {
        const errors = [];
        page.on('console', msg => {
            if (msg.type() === 'error') errors.push(msg.text());
        });
        page.on('pageerror', err => errors.push(err.message));
        
        await page.goto('http://localhost:8765/', { waitUntil: 'domcontentloaded' });
        await expect(page).toHaveTitle(/$display_name/i);
        await page.waitForTimeout(1500);
        
        const criticalErrors = errors.filter(e => 
            !e.includes('favicon') && !e.includes('DevTools') && !e.includes('net::ERR')
        );
        expect(criticalErrors, 'Console errors: ' + criticalErrors.join('; ')).toHaveLength(0);
    });
    
    test('should render game container', async ({ page }) => {
        await page.goto('http://localhost:8765/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(500);
        const body = page.locator('body');
        await expect(body).toBeVisible();
    });
});
TESTEOF
    echo "$test_file"
}

run_test() {
    local game_dir="$1"
    local name=$(basename_dir "$game_dir")
    local test_file="$game_dir/test.spec.js"
    
    if [[ ! -f "$test_file" ]]; then
        generate_test "$game_dir" > /dev/null
    fi
    
    if [[ ! -f "$game_dir/index.html" ]]; then
        log_skip "$name: no index.html"
        return 1
    fi
    
    log_info "Testing $name..."
    
    # Use local @playwright/test (in builder's node_modules)
    local playwright_bin="$SCRIPT_DIR/node_modules/.bin/playwright"
    [[ ! -x "$playwright_bin" ]] && playwright_bin="npx playwright"
    
    set +e
    local result
    result=$(cd "$game_dir" && NODE_PATH="$SCRIPT_DIR/node_modules" "$playwright_bin" test test.spec.js --reporter=line 2>&1)
    local test_status=$?
    set -e
    
    if echo "$result" | grep -qE "passed|passed|passed"; then
        log_ok "$name: tests passed"
        return 0
    elif echo "$result" | grep -q "skipped"; then
        log_skip "$name: tests skipped"
        return 2
    else
        log_fail "$name: tests failed"
        $VERBOSE && echo "$result" | tail -20
        return 1
    fi
}

main() {
    echo "=============================================="
    echo "  WASM Game Builder - Build & Test Suite"
    echo "=============================================="
    echo ""
    
    # Collect all WASM game dirs
    all_games=()
    while IFS= read -r g; do all_games+=("$g"); done < <(find_wasm_games)
    
    selected_games=()
    if [[ -n "$GAMES_FILTER" ]]; then
        IFS=',' read -ra filters <<< "$GAMES_FILTER"
        for filter in "${filters[@]}"; do
            filter=$(echo "$filter" | xargs)
            for g in "${all_games[@]}"; do
                local bn
                bn=$(basename_dir "$g" | sed 's/-wasm$//')
                if [[ "$bn" == "$filter" ]]; then
                    selected_games+=("$g")
                    break
                fi
            done
        done
    else
        selected_games=("${all_games[@]}")
    fi
    
    echo "Found ${#selected_games[@]} WASM game(s)"
    echo ""
    
    compile_ok=0; compile_fail=0; test_ok=0; test_fail=0; test_skip=0
    
    for game_dir in "${selected_games[@]}"; do
        [[ ! -d "$game_dir" ]] && continue
        local name
        name=$(basename_dir "$game_dir")
        
        if ! $TEST_ONLY; then
            if compile_game "$game_dir"; then
                ((compile_ok++)) || true
            else
                ((compile_fail++)) || true
            fi
        fi
        
        if ! $COMPILE_ONLY; then
            run_test "$game_dir"
            t_status=$?
            [[ $t_status -eq 0 ]] && ((test_ok++)) || true
            [[ $t_status -eq 2 ]] && ((test_skip++)) || true
            [[ $t_status -ne 0 ]] && ((test_fail++)) || true
        fi
        echo ""
    done
    
    echo "=============================================="
    echo "  Summary"
    echo "=============================================="
    total=${#selected_games[@]}
    echo -e "  Compile:  ${GREEN}${compile_ok} OK${NC}  ${RED}${compile_fail} failed${NC}  ${YELLOW}$(( total - compile_ok - compile_fail )) skipped${NC}"
    if ! $COMPILE_ONLY; then
        local actual_failures=$(( test_fail - test_skip ))
        [[ $actual_failures -lt 0 ]] && actual_failures=0
        echo -e "  Test:     ${GREEN}${test_ok} OK${NC}  ${RED}${actual_failures} failed${NC}  ${YELLOW}${test_skip} skipped${NC}"
    fi
    echo ""
    
    local ec=0
    [[ $compile_fail -gt 0 ]] && ec=1
    local fd=$(( test_fail - test_skip ))
    [[ $fd -gt 0 ]] && ec=1
    [[ $ec -eq 1 ]] && exit 1
    true
}

main
