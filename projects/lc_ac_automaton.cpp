// AC自动机 (Aho-Corasick) - 多模式匹配
// 核心：Trie + 失败链接(FAIL) + 每个节点的endpos
// 关键洞察：当前节点endpos=当前i；fail链节点的endpos是它们各自的历史位置
// 构建: O(total_pattern_len) | 匹配: O(text_len + total_matches)

#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

class AhoCorasick {
private:
    struct Node {
        unordered_map<char, int> next;
        int fail = 0;
        vector<int> output; // 仅当前节点的pattern（含重复pattern ID）
        bool isEnd = false;
    };
    vector<Node> trie;
    vector<string> patterns;

public:
    AhoCorasick() { trie.emplace_back(); }

    void insert(const string& pat) {
        int node = 0;
        for (char c : pat) {
            if (!trie[node].next.count(c)) {
                trie[node].next[c] = trie.size();
                trie.emplace_back();
            }
            node = trie[node].next[c];
        }
        trie[node].isEnd = true;
        trie[node].output.push_back(patterns.size());
        patterns.push_back(pat);
    }

    // 构建失败链接 (BFS)
    void build() {
        queue<int> q;
        for (auto& [c, child] : trie[0].next) {
            q.push(child);
            trie[child].fail = 0;
        }
        while (!q.empty()) {
            int parent = q.front(); q.pop();
            for (auto& [c, child] : trie[parent].next) {
                q.push(child);
                int f = trie[parent].fail;
                while (f > 0 && !trie[f].next.count(c)) f = trie[f].fail;
                trie[child].fail = (trie[f].next.count(c) ? trie[f].next[c] : 0);
                // 不继承output！每个node只有自己结尾的pattern
            }
        }
    }

    // 搜索：每个节点记录自己的endpos
    // 当前节点：endpos=当前i
    // fail链节点：使用其历史endpos
    vector<pair<int, string>> search(const string& text) {
        vector<pair<int, string>> results;
        vector<int> endpos(trie.size(), -1);
        vector<bool> seen(patterns.size(), false);
        int node = 0;
        for (int i = 0; i < text.size(); i++) {
            char c = text[i];
            while (node > 0 && !trie[node].next.count(c))
                node = trie[node].fail;
            if (trie[node].next.count(c))
                node = trie[node].next[c];

            // 当前节点在位置i结束
            endpos[node] = i;

            // 重置seen，输出当前节点（用当前i）
            fill(seen.begin(), seen.end(), false);
            for (int pid : trie[node].output) {
                if (!seen[pid]) {
                    results.push_back({i - (int)patterns[pid].size() + 1, patterns[pid]});
                    cout << "Match '" << patterns[pid] << "' at pos " << (i - patterns[pid].size() + 1) << "\n";
                    seen[pid] = true;
                }
            }

            // follow fail链：所有suffix也在位置i结束（统一用i计算）
            int cur = trie[node].fail;
            while (cur > 0) {
                for (int pid : trie[cur].output) {
                    if (!seen[pid]) {
                        results.push_back({i - (int)patterns[pid].size() + 1, patterns[pid]});
                        cout << "Match '" << patterns[pid] << "' at pos " << (i - patterns[pid].size() + 1) << "\n";
                        seen[pid] = true;
                    }
                }
                cur = trie[cur].fail;
            }
        }
        return results;
    }
};

int main() {
    // 测试1: 多模式匹配
    cout << "=== Test1: Multi-pattern match ===\n";
    AhoCorasick ac;
    vector<string> words = {"aba", "bab", "baba", "ab"};
    for (auto& w : words) ac.insert(w);
    ac.build();
    string text1 = "ababababa";
    auto res1 = ac.search(text1);
    // ab: pos0,2,4,6(4) | aba: pos0,2,4,6(4) | bab: pos1,3,5(3) | baba: pos1,3,5(3) = 14
    cout << "Total: " << res1.size() << " (expect 14) " << (res1.size() == 14 ? "PASS" : "FAIL") << "\n\n";

    // 测试2: 嵌套模式
    cout << "=== Test2: Nested patterns ===\n";
    AhoCorasick ac2;
    ac2.insert("a"); ac2.insert("aa"); ac2.insert("aaa");
    ac2.build();
    auto res2 = ac2.search("aaaa");
    // a: pos0,1,2,3 | aa: pos0,1,2 | aaa: pos0,1 = 9
    cout << "Nested 'a','aa','aaa' in 'aaaa': " << res2.size() << " (expect 9) " << (res2.size() == 9 ? "PASS" : "FAIL") << "\n";

    // 测试3: 无匹配
    cout << "=== Test3: No match ===\n";
    AhoCorasick ac3;
    ac3.insert("xyz"); ac3.build();
    auto res3 = ac3.search("abc");
    cout << (res3.size() == 0 ? "PASS" : "FAIL") << " (expect 0)\n";

    // 测试4: 经典例题
    cout << "=== Test4: Classic ===\n";
    AhoCorasick ac4;
    ac4.insert("he"); ac4.insert("she"); ac4.insert("his"); ac4.insert("hers");
    ac4.build();
    auto res4 = ac4.search("ahishers");
    // he@4, she@3, his@1, hers@4 = 4
    cout << "'he','she','his','hers' in 'ahishers': " << res4.size() << " (expect 4) " << (res4.size() == 4 ? "PASS" : "FAIL") << "\n";

    cout << "\nAll tests done!\n";
    return 0;
}
