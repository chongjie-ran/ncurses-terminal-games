#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// LC261 以图判树 - BFS: 连通 + 无环 + n个点n-1条边
bool validTree(int n, vector<vector<int>>& edges) {
    if (edges.size() != n-1) return false;
    vector<vector<int>> g(n);
    for (auto& e : edges) g[e[0]].push_back(e[1]), g[e[1]].push_back(e[0]);
    queue<int> q; q.push(0);
    vector<int> visited(n, 0); visited[0] = 1;
    int cnt = 1;
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        for (int nxt : g[cur]) {
            if (!visited[nxt]) {
                visited[nxt] = 1;
                q.push(nxt);
                cnt++;
            }
        }
    }
    return cnt == n;
}

// LC323 无向图连通分量数 - BFS
int countComponents(int n, vector<vector<int>>& edges) {
    vector<vector<int>> g(n);
    for (auto& e : edges) g[e[0]].push_back(e[1]), g[e[1]].push_back(e[0]);
    vector<int> visited(n, 0);
    int count = 0;
    queue<int> q;
    for (int i = 0; i < n; i++) {
        if (visited[i]) continue;
        count++;
        visited[i] = 1; q.push(i);
        while (!q.empty()) {
            int cur = q.front(); q.pop();
            for (int nxt : g[cur])
                if (!visited[nxt]) { visited[nxt] = 1; q.push(nxt); }
        }
    }
    return count;
}

// LC847 访问所有节点的最短路径 - BFS + 状态压缩
int shortestPathLength(vector<vector<int>>& graph) {
    int n = graph.size();
    if (n == 1) return 0;
    int ALL = (1 << n) - 1;
    queue<tuple<int,int,int>> q; // node, visited_mask, steps
    for (int i = 0; i < n; i++) q.emplace(i, 1<<i, 0);
    vector<vector<int>> visited(n, vector<int>(1<<n, 0));
    for (int i = 0; i < n; i++) visited[i][1<<i] = 1;
    while (!q.empty()) {
        auto [node, mask, d] = q.front(); q.pop();
        for (int nxt : graph[node]) {
            int nmask = mask | (1 << nxt);
            if (nmask == ALL) return d + 1;
            if (!visited[nxt][nmask]) {
                visited[nxt][nmask] = 1;
                q.emplace(nxt, nmask, d+1);
            }
        }
    }
    return -1;
}

// LC851 喧闹和富有 - 拓扑排序变形（DFS + memo）
vector<int> loudAndRich(vector<vector<int>>& richer, vector<int>& quiet) {
    int n = quiet.size();
    vector<vector<int>> g(n);
    for (auto& r : richer) g[r[1]].push_back(r[0]); // r[1]比r[0]富有
    vector<int> ans(n, -1);
    function<int(int)> dfs = [&](int x) {
        if (ans[x] != -1) return ans[x];
        ans[x] = x;
        for (int nxt : g[x]) {
            int cand = dfs(nxt);
            if (quiet[cand] < quiet[ans[x]]) ans[x] = cand;
        }
        return ans[x];
    };
    for (int i = 0; i < n; i++) dfs(i);
    return ans;
}

int main() {
    // validTree: n=5, 4 edges (tree)
    vector<vector<int>> e1 = {{0,1},{0,2},{0,3},{1,4}};
    cout << validTree(5, e1) << endl; // 1
    
    // validTree: cycle
    vector<vector<int>> e2 = {{0,1},{1,2},{2,0}};
    cout << validTree(3, e2) << endl; // 0
    
    // countComponents
    vector<vector<int>> e3 = {{0,1},{2,3}};
    cout << countComponents(4, e3) << endl; // 2
    
    // shortestPathLength - triangle
    vector<vector<int>> g1 = {{1,2,3},{0},{0},{0}};
    cout << shortestPathLength(g1) << endl; // 1 (0->3->2 or 0->1)
    
    return 0;
}
