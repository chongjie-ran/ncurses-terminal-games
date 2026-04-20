// LC207 Course Schedule - 拓扑排序/DFS检测环
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// 方法1: DFS + 三色标记 (白/灰/黑)
class Solution1 {
public:
    bool canFinish(int n, vector<vector<int>>& edges) {
        vector<vector<int>> g(n);
        for (auto& e : edges) g[e[1]].push_back(e[0]); // e[1]->e[0] 依赖
        vector<int> vis(n, 0); // 0=白,1=灰,2=黑
        for (int i = 0; i < n; i++)
            if (dfs(i, g, vis)) return false;
        return true;
    }
    bool dfs(int u, vector<vector<int>>& g, vector<int>& vis) {
        if (vis[u] == 1) return true;  // 灰: 环
        if (vis[u] == 2) return false; // 黑: 已完成
        vis[u] = 1;
        for (int v : g[u])
            if (dfs(v, g, vis)) return true;
        vis[u] = 2;
        return false;
    }
};

// 方法2: Kahn BFS (入度)
class Solution2 {
public:
    bool canFinish(int n, vector<vector<int>>& edges) {
        vector<vector<int>> g(n);
        vector<int> indeg(n, 0);
        for (auto& e : edges) {
            g[e[1]].push_back(e[0]);
            indeg[e[0]]++;
        }
        queue<int> q;
        for (int i = 0; i < n; i++)
            if (indeg[i] == 0) q.push(i);
        int cnt = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            cnt++;
            for (int v : g[u])
                if (--indeg[v] == 0) q.push(v);
        }
        return cnt == n;
    }
};

int main() {
    Solution1 s1; Solution2 s2;
    vector<vector<int>> e1 = {{1,0}};
    cout << (s1.canFinish(2, e1) ? "T" : "F") << " ";
    cout << (s2.canFinish(2, e1) ? "T" : "F") << endl;
    vector<vector<int>> e2 = {{1,0},{0,1}};
    cout << (s1.canFinish(2, e2) ? "T" : "F") << " ";
    cout << (s2.canFinish(2, e2) ? "T" : "F") << endl;
    return 0;
}
