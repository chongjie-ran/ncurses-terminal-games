// LC684 Redundant Connection
#include <iostream>
#include <vector>
using namespace std;

struct UnionFind {
    vector<int> parent;
    UnionFind(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        parent[px] = py;
        return true;
    }
};

vector<int> findRedundantConnection(vector<vector<int>>& edges) {
    int n = edges.size();
    UnionFind uf(n + 1);
    for (auto& e : edges) {
        if (!uf.unite(e[0], e[1])) {
            return e;
        }
    }
    return {};
}

int main() {
    vector<vector<int>> e1 = {{1,2},{1,3},{2,3}};
    auto r1 = findRedundantConnection(e1);
    cout << r1[0] << " " << r1[1] << endl; // 2 3
    
    vector<vector<int>> e2 = {{1,2},{2,3},{3,4},{1,4},{1,5}};
    auto r2 = findRedundantConnection(e2);
    cout << r2[0] << " " << r2[1] << endl; // 1 4
    return 0;
}
