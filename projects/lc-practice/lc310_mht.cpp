// LC310 Minimum Height Trees - 叶子剥落法/贪心
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
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
        vector<int> leaves;
        while (!q.empty()) {
            leaves.clear();
            int sz = q.size();
            while (sz-- > 0) {
                int u = q.front(); q.pop();
                leaves.push_back(u);
                for (int v : g[u]) {
                    if (--indeg[v] == 1) q.push(v);
                }
            }
        }
        return leaves;
    }
};

int main() {
    Solution s;
    vector<vector<int>> e1 = {{1,0},{1,2},{1,3}};
    vector<int> r1 = s.findMinHeightTrees(4, e1);
    for (int x : r1) cout << x << " ";
    cout << endl;
    vector<vector<int>> e2 = {{0,1},{0,2},{0,3},{3,4},{4,5}};
    vector<int> r2 = s.findMinHeightTrees(6, e2);
    for (int x : r2) cout << x << " ";
    cout << endl;
    return 0;
}
