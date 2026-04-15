// LC207 Course Schedule - 课程表 (检测有向图环 / 拓扑排序)
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// 方法1: BFS拓扑排序 (Kahn算法) - 统计能完成的课程数
bool canFinish1(int numCourses, vector<pair<int,int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> indegree(numCourses, 0);
    for (auto& p : prerequisites) {
        adj[p.second].push_back(p.first);
        indegree[p.first]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (indegree[i] == 0) q.push(i);
    
    int count = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        count++;
        for (int v : adj[u]) {
            if (--indegree[v] == 0) q.push(v);
        }
    }
    return count == numCourses;
}

// 方法2: DFS检测环
bool dfsCycle(int u, vector<vector<int>>& adj, vector<int>& state) {
    state[u] = 1; // visiting
    for (int v : adj[u]) {
        if (state[v] == 1) return true;  // 环
        if (state[v] == 0 && dfsCycle(v, adj, state)) return true;
    }
    state[u] = 2; // done
    return false;
}

bool canFinish2(int numCourses, vector<pair<int,int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    for (auto& p : prerequisites) adj[p.second].push_back(p.first);
    vector<int> state(numCourses, 0); // 0=unvisited, 1=visiting, 2=done
    for (int i = 0; i < numCourses; i++)
        if (state[i] == 0 && dfsCycle(i, adj, state)) return false;
    return true;
}

int main() {
    vector<pair<int,int>> pre1 = {{1,0},{0,1}}; // 环: 0->1->0
    cout << canFinish1(2, pre1) << endl; // 0
    cout << canFinish2(2, pre1) << endl; // 0
    
    vector<pair<int,int>> pre2 = {{1,0},{2,1},{3,2}}; // 链
    cout << canFinish1(4, pre2) << endl; // 1
    cout << canFinish2(4, pre2) << endl; // 1
    
    return 0;
}
