// AC自动机 - 字符串多模式匹配
// 三部分: Trie + fail指针 + BFS构建 + 匹配
// 复杂度: O(text_len + patterns_total_len + matches)
// LeetCode应用: LC1032 Stream of Characters
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
using namespace std;

struct ANode {
    unordered_map<char,int> nxt;
    int fail = 0;
    int out = 0; // 多少个模式串在此节点结束
    vector<int> pattern_ids;
};

struct AAutomaton {
    vector<ANode> t;
    AAutomaton() { t.emplace_back(); } // root=0
    
    void insert(const string& s, int id) {
        int node = 0;
        for(char c: s) {
            if(!t[node].nxt.count(c)) {
                t[node].nxt[c] = t.size();
                t.emplace_back();
            }
            node = t[node].nxt[c];
        }
        t[node].out = 1;
        t[node].pattern_ids.push_back(id);
    }
    
    void build() {
        queue<int> q;
        // BFS: 设置第一层fail指向root(0)
        for(auto& [c, n]: t[0].nxt) {
            t[n].fail = 0;
            q.push(n);
        }
        while(!q.empty()) {
            int v = q.front(); q.pop();
            for(auto& [c, n]: t[v].nxt) {
                int f = t[v].fail;
                while(f && !t[f].nxt.count(c)) f = t[f].fail;
                if(t[f].nxt.count(c)) f = t[f].nxt[c];
                t[n].fail = f;
                t[n].out += t[f].out; // 累加fail链上的输出
                q.push(n);
            }
        }
    }
    
    // 在text中搜索，返回所有匹配的位置
    void search(const string& text, const vector<string>& patterns) {
        int node = 0;
        for(int i=0;i<(int)text.size();i++) {
            char c = text[i];
            while(node && !t[node].nxt.count(c)) node = t[node].fail;
            if(t[node].nxt.count(c)) node = t[node].nxt[c];
            if(t[node].out > 0) {
                for(int pid: t[node].pattern_ids) {
                    cout << "Pattern '" << patterns[pid] << "' ends at pos " << i << endl;
                }
            }
        }
    }
};

// LC1032 Stream of Characters: AC自动机流式查询
struct StreamChecker {
    AAutomaton ac;
    int node = 0;
    vector<string> patterns;
    
    StreamChecker(const vector<string>& words) {
        patterns = words;
        for(int i=0;i<(int)words.size();i++) ac.insert(words[i], i);
        ac.build();
    }
    
    bool query(char c) {
        while(node && !ac.t[node].nxt.count(c)) node = ac.t[node].fail;
        if(ac.t[node].nxt.count(c)) node = ac.t[node].nxt[c];
        if(ac.t[node].out > 0) return true;
        return false;
    }
};

int main() {
    // AC自动机基础验证
    AAutomaton ac;
    vector<string> patterns = {"he", "she", "his", "hers"};
    for(int i=0;i<(int)patterns.size();i++) ac.insert(patterns[i], i);
    ac.build();
    
    string text = "ahishers";
    cout << "=== AC自动机基础测试 ===" << endl;
    ac.search(text, patterns);
    cout << endl;
    
    // LC1032 Stream of Characters
    cout << "=== LC1032 Stream of Characters ===" << endl;
    StreamChecker sc({"cd", "f", "kl"});
    cout << "a:" << sc.query('a') << " b:" << sc.query('b') 
         << " c:" << sc.query('c') << " d:" << sc.query('d') << endl;
    // cd -> True at d
    return 0;
}
