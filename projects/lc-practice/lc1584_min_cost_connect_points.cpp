// LC1584 - Min Cost to Connect All Points
// Medium: Kruskal's Algorithm (MST) + Union-Find
// 思路: 将所有点转换为边权为曼哈顿距离的完全图，用Kruskal求MST
// 时间: O(n² log n) 空间: O(n²)

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdlib>
using namespace std;

struct DSU {
    vector<int> p, r;
    DSU(int n): p(n), r(n, 0) { iota(p.begin(), p.end(), 0); }
    int find(int x) { return p[x]==x ? x : p[x]=find(p[x]); }
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (r[a] < r[b]) swap(a, b);
        p[b] = a;
        if (r[a] == r[b]) ++r[a];
        return true;
    }
};

struct Edge {
    int u, v, w;
    bool operator<(Edge const& o) const { return w < o.w; }
};

int minCostConnectPoints(vector<vector<int>>& pts) {
    int n = pts.size();
    vector<Edge> edges;
    for (int i = 0; i < n; ++i)
        for (int j = i+1; j < n; ++j) {
            int d = abs(pts[i][0]-pts[j][0]) + abs(pts[i][1]-pts[j][1]);
            edges.push_back({i, j, d});
        }
    sort(edges.begin(), edges.end());
    
    DSU dsu(n);
    int cost = 0, added = 0;
    for (auto& e : edges) {
        if (dsu.unite(e.u, e.v)) {
            cost += e.w;
            if (++added == n-1) break;
        }
    }
    return cost;
}

int main() {
    vector<vector<int>> g1 = {{0,0},{2,2},{3,10},{5,2},{7,0}};
    cout << "Test1: " << minCostConnectPoints(g1) << " (expected: 20)" << endl;
    
    vector<vector<int>> g2 = {{3,12},{-2,5},{-4,1}};
    cout << "Test2: " << minCostConnectPoints(g2) << " (expected: 18)" << endl;

    vector<vector<int>> g3 = {{0,0},{2,2},{3,10},{5,2},{7,0}};
    cout << "Test3: " << minCostConnectPoints(g3) << " (expected: 20)" << endl;
    
    return 0;
}
