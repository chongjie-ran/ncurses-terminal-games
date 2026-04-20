#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
using namespace std;

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
        cout << "  Insert '" << s << "' at terminal node " << v << endl;
    }

    void build() {
        queue<int> q;
        // 初始化根的直接子节点
        for (int c = 0; c < 26; c++) {
            int v = nodes[0].next[c];
            if (v != -1) {
                nodes[v].fail = 0;
                q.push(v);
            } else {
                nodes[0].next[c] = 0;
            }
        }

        cout << "After init: nodes[1].fail=" << nodes[1].fail 
             << " nodes[3].fail=" << nodes[3].fail << endl;

        while (!q.empty()) {
            int v = q.front(); q.pop();
            cout << "Processing node " << v << " (char?";

            for (int c = 0; c < 26; c++) {
                int u = nodes[v].next[c];
                if (u != -1) {
                    nodes[u].fail = nodes[nodes[v].fail].next[c];
                    cout << " child[ch=" << (char)('a'+c) << "]=" << u << "(fail=" << nodes[u].fail << ")";
                    q.push(u);
                } else {
                    nodes[v].next[c] = nodes[nodes[v].fail].next[c];
                }
            }
            cout << endl;
        }
    }

    vector<int> search(const string& text) {
        vector<int> cnt(patterns.size(), 0);
        int v = 0;
        for (int i = 0; i < (int)text.size(); i++) {
            int c = text[i] - 'a';
            v = nodes[v].next[c];
            cout << "Pos " << i << "='" << text[i] << "' -> v=" << v 
                 << " pat_id=" << nodes[v].pat_id;
            // 打印fail链
            cout << " fail_chain: ";
            for (int u = v; u != 0; u = nodes[u].fail)
                cout << u << "(p" << nodes[u].pat_id << ") ";
            cout << endl;
            // 计数
            for (int u = v; u != 0; u = nodes[u].fail) {
                if (nodes[u].pat_id >= 0)
                    cnt[nodes[u].pat_id]++;
            }
        }
        return cnt;
    }
};

int main() {
    ACAutomaton ac;
    ac.patterns = {"he", "she", "her"};
    cout << "=== Building AC for he/she/her ===" << endl;
    ac.insert("he", 0);
    ac.insert("she", 1);
    ac.insert("her", 2);
    cout << endl;
    ac.build();
    cout << endl;
    
    cout << "=== Searching 'hershesay' ===" << endl;
    auto cnt = ac.search("hershesay");
    cout << endl;
    cout << "Result: ";
    for (int i = 0; i < 3; i++) cout << cnt[i] << " ";
    cout << endl;
    cout << "Expected: 3 1 2" << endl;
    return 0;
}
