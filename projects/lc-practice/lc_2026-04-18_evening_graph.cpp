#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
using namespace std;

// LC133 克隆图 - DFS递归版
// 核心：unordered_map记录新旧节点映射
class Node {
public:
    int val;
    vector<Node*> neighbors;
    Node(int v=0): val(v) {}
};

Node* cloneGraph(Node* node) {
    if (!node) return nullptr;
    unordered_map<Node*, Node*> mp;
    function<void(Node*)> dfs = [&](Node* cur) {
        mp[cur] = new Node(cur->val);
        for (Node* nxt : cur->neighbors) {
            if (!mp.count(nxt)) dfs(nxt);
        }
    };
    dfs(node);
    function<void(Node*)> build = [&](Node* cur) {
        for (Node* nxt : cur->neighbors) {
            mp[cur]->neighbors.push_back(mp[nxt]);
        }
    };
    build(node);
    return mp[node];
}

// LC207 课程表 - BFS拓扑排序
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
    int visited = 0;
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        visited++;
        for (int nxt : g[cur]) {
            if (--indeg[nxt] == 0) q.push(nxt);
        }
    }
    return visited == numCourses;
}

// LC210 课程表 II - 返回学习顺序
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
    vector<int> order;
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        order.push_back(cur);
        for (int nxt : g[cur])
            if (--indeg[nxt] == 0) q.push(nxt);
    }
    return order.size() == numCourses ? order : vector<int>();
}

// LC310 最小高度树 - 拓扑剪枝（从叶子节点向中心）
vector<int> findMinHeightTrees(int n, vector<vector<int>>& edges) {
    if (n == 1) return {0};
    vector<vector<int>> g(n);
    vector<int> indeg(n, 0);
    for (auto& e : edges) {
        g[e[0]].push_back(e[1]);
        g[e[1]].push_back(e[0]);
        indeg[e[0]]++; indeg[e[1]]++;
    }
    queue<int> q;
    for (int i = 0; i < n; i++)
        if (indeg[i] == 1) q.push(i);
    int remaining = n;
    while (remaining > 2) {
        int sz = q.size();
        remaining -= sz;
        for (int i = 0; i < sz; i++) {
            int cur = q.front(); q.pop();
            for (int nxt : g[cur]) {
                if (--indeg[nxt] == 1) q.push(nxt);
            }
        }
    }
    vector<int> res;
    while (!q.empty()) { res.push_back(q.front()); q.pop(); }
    return res;
}

// LC785 判断二分图 - BFS染色
bool isBipartite(vector<vector<int>>& graph) {
    vector<int> color(graph.size(), -1);
    queue<int> q;
    for (int i = 0; i < graph.size(); i++) {
        if (color[i] != -1) continue;
        color[i] = 0;
        q.push(i);
        while (!q.empty()) {
            int cur = q.front(); q.pop();
            for (int nxt : graph[cur]) {
                if (color[nxt] == -1) {
                    color[nxt] = 1 - color[cur];
                    q.push(nxt);
                } else if (color[nxt] == color[cur]) {
                    return false;
                }
            }
        }
    }
    return true;
}

int main() {
    // LC207 test
    vector<vector<int>> p1 = {{1,0}};
    cout << canFinish(2, p1) << endl; // 1
    
    // LC207 cycle test
    vector<vector<int>> p2 = {{1,0},{0,1}};
    cout << canFinish(2, p2) << endl; // 0
    
    // LC785 test
    vector<vector<int>> g1 = {{1,2,3},{0,2},{0,1,3},{0,2}};
    cout << isBipartite(g1) << endl; // 0
    
    // LC785 valid
    vector<vector<int>> g2 = {{1,3},{0,2},{1,3},{0,2}};
    cout << isBipartite(g2) << endl; // 1
    
    return 0;
}
