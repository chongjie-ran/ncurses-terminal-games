// LC785 Is Graph Bipartite? - BFS二染色
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    bool isBipartite(vector<vector<int>>& graph) {
        int n = graph.size();
        vector<int> color(n, -1); // -1=未染色, 0/1=两种颜色
        for (int i = 0; i < n; i++) {
            if (color[i] != -1) continue;
            queue<int> q;
            q.push(i);
            color[i] = 0;
            while (!q.empty()) {
                int u = q.front(); q.pop();
                for (int v : graph[u]) {
                    if (color[v] == -1) {
                        color[v] = 1 - color[u];
                        q.push(v);
                    } else if (color[v] == color[u]) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
};

int main() {
    Solution s;
    // [[1,2,3],[0,2],[0,1,3],[0,2]] -> false (存在奇环)
    vector<vector<int>> g1 = {{1,2,3},{0,2},{0,1,3},{0,2}};
    cout << (s.isBipartite(g1) ? "T" : "F") << endl;
    // [[1,3],[0,2],[1,3],[0,2]] -> true
    vector<vector<int>> g2 = {{1,3},{0,2},{1,3},{0,2}};
    cout << (s.isBipartite(g2) ? "T" : "F") << endl;
    return 0;
}
