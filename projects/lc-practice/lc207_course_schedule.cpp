// LC207 Course Schedule - 拓扑排序 (BFS/DFS)
// 2026-03-31 07:45
// 判断是否有环 + 返回学习顺序
#include <vector>
#include <queue>
using namespace std;

// BFS拓扑排序 (Kahn算法)
bool canFinishBFS(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> indegree(numCourses, 0);
    for (auto& p : prerequisites) {
        adj[p[1]].push_back(p[0]);  // p[1] -> p[0]
        indegree[p[0]]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (indegree[i] == 0) q.push(i);
    
    int visited = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        visited++;
        for (int v : adj[u]) {
            indegree[v]--;
            if (indegree[v] == 0) q.push(v);
        }
    }
    return visited == numCourses;
}

// DFS检测环
bool dfs(int u, vector<vector<int>>& adj, vector<int>& state, vector<int>& order) {
    state[u] = 1; // visiting
    for (int v : adj[u]) {
        if (state[v] == 1) return false; // 环
        if (state[v] == 0 && !dfs(v, adj, state, order)) return false;
    }
    state[u] = 2; // done
    order.push_back(u);
    return true;
}

bool canFinishDFS(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    for (auto& p : prerequisites)
        adj[p[1]].push_back(p[0]);
    
    vector<int> state(numCourses, 0); // 0=unvisited, 1=visiting, 2=done
    vector<int> order;
    
    for (int i = 0; i < numCourses; i++) {
        if (state[i] == 0) {
            if (!dfs(i, adj, state, order)) return false;
        }
    }
    return true;
}

// 返回学习顺序 (LC210)
vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    for (auto& p : prerequisites)
        adj[p[1]].push_back(p[0]);
    
    vector<int> state(numCourses, 0), order;
    for (int i = 0; i < numCourses; i++) {
        if (state[i] == 0) {
            if (!dfs(i, adj, state, order)) return {}; // 有环
        }
    }
    reverse(order.begin(), order.end()); // 逆序得到正确顺序
    return order;
}
