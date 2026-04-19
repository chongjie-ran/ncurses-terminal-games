// LC684 Redundant Connection - 找形成环的边
#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

class UnionFind {
public:
    vector<int> parent, rank;
    UnionFind(int n) {
        parent.resize(n);
        rank.assign(n, 0);
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int a, int b) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return false; // 已连通，此边为冗余
        if (rank[ra] < rank[rb]) swap(ra, rb);
        parent[rb] = ra;
        if (rank[ra] == rank[rb]) rank[ra]++;
        return true;
    }
};

vector<int> findRedundantConnection(vector<vector<int>>& edges) {
    int n = edges.size();
    UnionFind uf(n + 1);
    vector<int> result;
    for (auto& e : edges) {
        if (!uf.unite(e[0], e[1])) {
            result = e; // 这条边导致环
        }
    }
    return result;
}

int main() {
    vector<vector<int>> e1 = {{1,2},{1,3},{2,3}};
    auto r1 = findRedundantConnection(e1);
    cout << r1[0] << " " << r1[1] << " (expect 2 3)" << endl;
    
    vector<vector<int>> e2 = {{1,2},{2,3},{3,1}};
    auto r2 = findRedundantConnection(e2);
    cout << r2[0] << " " << r2[1] << " (expect 3 1)" << endl;
    
    return 0;
}
