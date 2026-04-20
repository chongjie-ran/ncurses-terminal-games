// LC301 删除无效括号 (Hard - BFS最小删除)
// 核心模式: BFS层序遍历，删除一个括号生成下一层，第一个找到valid的层就是最小删除层
// 剪枝: visited去重，同层第一个valid后continue该层其他节点

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_set>
using namespace std;

class Solution {
public:
    vector<string> removeInvalidParentheses(string s) {
        unordered_set<string> result;
        queue<pair<string,int>> q;
        q.push({s, 0});
        unordered_set<string> visited;
        visited.insert(s);
        bool found = false;
        
        while (!q.empty() && !found) {
            int sz = q.size();
            for (int i = 0; i < sz; i++) {
                auto [cur, idx] = q.front(); q.pop();
                
                if (isValid(cur)) {
                    result.insert(cur);
                    found = true;  // 找到一层valid后，停止扩展但继续收集同层其他valid
                }
                
                if (found) continue;  // 找到后不再扩展下一层
                
                for (int j = 0; j < cur.size(); j++) {
                    if (cur[j] == '(' || cur[j] == ')') {
                        string next = cur.substr(0, j) + cur.substr(j+1);
                        if (visited.find(next) == visited.end()) {
                            visited.insert(next);
                            q.push({next, j});
                        }
                    }
                }
            }
        }
        
        return vector<string>(result.begin(), result.end());
    }
    
    bool isValid(const string& t) {
        int cnt = 0;
        for (char c : t) {
            if (c == '(') cnt++;
            else if (c == ')') { cnt--; if (cnt < 0) return false; }
        }
        return cnt == 0;
    }
};
