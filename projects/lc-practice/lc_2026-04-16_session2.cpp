// LC练习 2026-04-16 04:20 - 图论专题
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// LC207 课程表 - 拓扑排序 (BFS)
bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> g(numCourses);
    vector<int> indeg(numCourses, 0);
    for (auto& p : prerequisites) {
        g[p[1]].push_back(p[0]);
        indeg[p[0]]++;
    }
    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (indeg[i] == 0) q.push(i);
    int cnt = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        cnt++;
        for (int v : g[u])
            if (--indeg[v] == 0) q.push(v);
    }
    return cnt == numCourses;
}

// LC210 课程表II - 拓扑排序返回顺序
vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> g(numCourses);
    vector<int> indeg(numCourses, 0);
    for (auto& p : prerequisites) {
        g[p[1]].push_back(p[0]);
        indeg[p[0]]++;
    }
    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (indeg[i] == 0) q.push(i);
    vector<int> res;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        res.push_back(u);
        for (int v : g[u])
            if (--indeg[v] == 0) q.push(v);
    }
    if ((int)res.size() != numCourses) return {};
    return res;
}

// LC797 所有路径 - DFS
void dfs(int u, int t, vector<vector<int>>& g, vector<int>& path, vector<vector<int>>& res) {
    path.push_back(u);
    if (u == t) { res.push_back(path); }
    else for (int v : g[u]) dfs(v, t, g, path, res);
    path.pop_back();
}
vector<vector<int>> allPathsSourceTarget(vector<vector<int>>& graph) {
    vector<vector<int>> res;
    vector<int> path;
    dfs(0, (int)graph.size()-1, graph, path, res);
    return res;
}

int main() {
    // Test LC207
    {
        vector<vector<int>> p = {{1,0},{2,0},{3,1},{3,2}};
        cout << "LC207 canFinish: " << canFinish(4, p) << endl; // 1
    }
    // Test LC210
    {
        vector<vector<int>> p = {{1,0},{2,0},{3,1},{3,2}};
        auto r = findOrder(4, p);
        cout << "LC210 findOrder: ";
        for (int x : r) cout << x << " "; cout << endl;
    }
    // Test LC797
    {
        vector<vector<int>> g = {{1,2},{3},{3},{}};
        auto r = allPathsSourceTarget(g);
        cout << "LC797 paths: " << r.size() << endl; // 2
    }
    return 0;
}
