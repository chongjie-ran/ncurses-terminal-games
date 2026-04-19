// LC207 Course Schedule - 拓扑排序 (BFS/DFS)
// 判断是否能完成所有课程（无环依赖）
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// ============ 方法1: BFS Kahn算法 (拓扑排序) ============
bool canFinish_BFS(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> indegree(numCourses, 0);
    
    for (auto& p : prerequisites) {
        adj[p[1]].push_back(p[0]); // p[1] -> p[0]
        indegree[p[0]]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++) {
        if (indegree[i] == 0) q.push(i);
    }
    
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

// ============ 测试 ============
int main() {
    // Test 1: [[1,0]] - 可以完成
    vector<vector<int>> p1 = {{1,0}};
    bool r1 = canFinish_BFS(2, p1);
    cout << "Test1 (expect 1): " << r1 << endl;
    
    // Test 2: [[1,0],[0,1]] - 环，不能完成
    vector<vector<int>> p2 = {{1,0},{0,1}};
    bool r2 = canFinish_BFS(2, p2);
    cout << "Test2 (expect 0): " << r2 << endl;
    
    // Test 3: 无前置条件
    vector<vector<int>> p3 = {};
    bool r3 = canFinish_BFS(3, p3);
    cout << "Test3 (expect 1): " << r3 << endl;
    
    // Test 4: [[1,0],[2,0],[3,1],[3,2]] - 0->1,2; 1,2->3
    vector<vector<int>> p4 = {{1,0},{2,0},{3,1},{3,2}};
    bool r4 = canFinish_BFS(4, p4);
    cout << "Test4 (expect 1): " << r4 << endl;
    
    // Test 5: [[0,1],[1,2],[2,0]] - 环
    vector<vector<int>> p5 = {{0,1},{1,2},{2,0}};
    bool r5 = canFinish_BFS(3, p5);
    cout << "Test5 (expect 0): " << r5 << endl;
    
    // Test 6: LC210 Course Schedule II - 拓扑排序输出顺序
    vector<vector<int>> p6 = {{1,0},{2,0},{3,1},{3,2}};
    int n6 = 4;
    vector<vector<int>> adj6(n6);
    vector<int> indegree6(n6, 0);
    for (auto& p : p6) {
        adj6[p[1]].push_back(p[0]);
        indegree6[p[0]]++;
    }
    queue<int> q6;
    for (int i = 0; i < n6; i++) if (indegree6[i] == 0) q6.push(i);
    vector<int> order;
    while (!q6.empty()) {
        int u = q6.front(); q6.pop();
        order.push_back(u);
        for (int v : adj6[u]) {
            indegree6[v]--;
            if (indegree6[v] == 0) q6.push(v);
        }
    }
    cout << "Test6 order (expect 0 1 2 3 or similar): ";
    for (int x : order) cout << x << " ";
    cout << endl;
    
    return 0;
}
