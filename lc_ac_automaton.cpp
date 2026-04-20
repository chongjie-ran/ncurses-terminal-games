#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
using namespace std;

// AC自动机: 修复自环bug
// 关键：根的next要全部初始化，fail=-1时按0处理
struct ACAutomaton {
    struct Node {
        int next[26];
        int fail;
        int pat_id;
        Node() {
            memset(next, -1, sizeof(next));
            fail = 0;
            pat_id = -1;
        }
    };
    vector<Node> nodes;
    vector<string> patterns;

    ACAutomaton() { nodes.emplace_back(); }

    void insert(const string& s, int id) {
        int v = 0;
        for (char ch : s) {
            int c = ch - 'a';
            if (nodes[v].next[c] == -1) {
                nodes[v].next[c] = nodes.size();
                nodes.emplace_back();
            }
            v = nodes[v].next[c];
        }
        nodes[v].pat_id = id;
    }

    void build() {
        queue<int> q;
        // 初始化根的子节点fail=0，缺失边指向0
        for (int c = 0; c < 26; c++) {
            int v = nodes[0].next[c];
            if (v != -1) {
                nodes[v].fail = 0;
                q.push(v);
            } else {
                nodes[0].next[c] = 0;  // 缺失边才填0
            }
        }
        // BFS计算fail
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (int c = 0; c < 26; c++) {
                int u = nodes[v].next[c];
                if (u != -1) {
                    nodes[u].fail = nodes[nodes[v].fail].next[c];
                    q.push(u);
                }
            }
        }
    }

    vector<int> search(const string& text) {
        vector<int> cnt(patterns.size(), 0);
        int v = 0;
        for (char ch : text) {
            int c = ch - 'a';
            v = nodes[v].next[c];
            // 遍历fail链（排除根节点0）
            for (int u = v; u > 0; u = nodes[u].fail) {
                if (nodes[u].pat_id >= 0)
                    cnt[nodes[u].pat_id]++;
            }
        }
        return cnt;
    }
};

int main() {
    // Test 1: 基础多模式匹配
    {
        ACAutomaton ac;
        ac.patterns = {"he", "she", "her"};
        ac.insert("he", 0);
        ac.insert("she", 1);
        ac.insert("her", 2);
        ac.build();

        string text = "hershesay";
        auto cnt = ac.search(text);
        cout << "Test1 (hershesay, he/she/her): ";
        for (int i = 0; i < 3; i++) cout << cnt[i] << " ";
        cout << endl;
        cout << "  Expected: 3 1 2" << endl;
    }

    // Test 2: 重复字符模式
    {
        ACAutomaton ac;
        ac.patterns = {"a", "aa", "aaa"};
        for (int i = 0; i < (int)ac.patterns.size(); i++)
            ac.insert(ac.patterns[i], i);
        ac.build();

        string text = "aaaa";
        auto cnt = ac.search(text);
        cout << "Test2 (aaaa, a/aa/aaa): ";
        for (int x : cnt) cout << x << " ";
        cout << endl;
        cout << "  Expected: 4 3 2" << endl;
    }

    // Test 3: 纯不存在
    {
        ACAutomaton ac;
        ac.patterns = {"xyz", "abc"};
        ac.insert("xyz", 0);
        ac.insert("abc", 1);
        ac.build();

        string text = "hello";
        auto cnt = ac.search(text);
        cout << "Test3 (hello, xyz/abc): " << cnt[0] << " " << cnt[1] << endl;
        cout << "  Expected: 0 0" << endl;
    }

    // Test 4: 单模式
    {
        ACAutomaton ac;
        ac.patterns = {"ab"};
        ac.insert("ab", 0);
        ac.build();

        string text = "ababab";
        auto cnt = ac.search(text);
        cout << "Test4 (ababab, ab): " << cnt[0] << endl;
        cout << "  Expected: 3" << endl;
    }

    // Test 5: 多模式含有包含关系
    {
        ACAutomaton ac;
        ac.patterns = {"a", "ab", "bab", "bc", "bca", "c"};
        for (int i = 0; i < (int)ac.patterns.size(); i++)
            ac.insert(ac.patterns[i], i);
        ac.build();

        string text = "abccab";
        auto cnt = ac.search(text);
        cout << "Test5 (abccab, a/ab/bab/bc/bca/c): ";
        for (int x : cnt) cout << x << " ";
        cout << endl;
        cout << "  Expected: 2 2 1 2 0 3" << endl;
    }

    // Test 6: 重叠模式
    {
        ACAutomaton ac;
        ac.patterns = {"abc", "bcd", "c"};
        ac.insert("abc", 0);
        ac.insert("bcd", 1);
        ac.insert("c", 2);
        ac.build();

        string text = "abcd";
        auto cnt = ac.search(text);
        cout << "Test6 (abcd, abc/bcd/c): " << cnt[0] << " " << cnt[1] << " " << cnt[2] << endl;
        cout << "  Expected: 1 1 2" << endl;
    }

    // Test 7: LC208类似
    {
        ACAutomaton ac;
        ac.patterns = {"app", "apple", "beer", "jam", "java"};
        for (int i = 0; i < (int)ac.patterns.size(); i++)
            ac.insert(ac.patterns[i], i);
        ac.build();

        string text = "applecoffee";
        auto cnt = ac.search(text);
        cout << "Test7 (applecoffee, app/apple/beer/jam/java): ";
        for (int x : cnt) cout << x << " ";
        cout << endl;
        cout << "  Expected: 1 1 0 0 0" << endl;
    }

    cout << "\nAC Automaton Tests Complete!" << endl;
    return 0;
}
