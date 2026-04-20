// LC210 Course Schedule II - 拓扑排序返回学习顺序
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    vector<int> findOrder(int n, vector<vector<int>>& edges) {
        vector<vector<int>> g(n);
        vector<int> indeg(n, 0);
        for (auto& e : edges) {
            g[e[1]].push_back(e[0]);
            indeg[e[0]]++;
        }
        queue<int> q;
        for (int i = 0; i < n; i++)
            if (indeg[i] == 0) q.push(i);
        vector<int> order;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            order.push_back(u);
            for (int v : g[u])
                if (--indeg[v] == 0) q.push(v);
        }
        if ((int)order.size() != n) return {}; // 有环
        return order;
    }
};

int main() {
    Solution s;
    // case1: [[1,0]] -> [0,1]
    vector<vector<int>> e1 = {{1,0}};
    vector<int> r1 = s.findOrder(2, e1);
    for (int x : r1) cout << x << " ";
    cout << endl;
    // case2: 4门课, [[1,0],[2,0],[3,1],[3,2]] -> [0,1,2,3]或[0,2,1,3]
    vector<vector<int>> e2 = {{1,0},{2,0},{3,1},{3,2}};
    vector<int> r2 = s.findOrder(4, e2);
    for (int x : r2) cout << x << " ";
    cout << endl;
    return 0;
}
