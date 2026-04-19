// LC323 Number of Connected Components in an Undirected Graph
// Union-Find: 边列表求连通分量
#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

class UnionFind {
public:
    vector<int> parent, rank;
    int count;
    UnionFind(int n) {
        parent.resize(n);
        rank.assign(n, 0);
        count = n;
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    void unite(int a, int b) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return;
        if (rank[ra] < rank[rb]) swap(ra, rb);
        parent[rb] = ra;
        if (rank[ra] == rank[rb]) rank[ra]++;
        count--;
    }
};

int countComponents(int n, vector<vector<int>>& edges) {
    UnionFind uf(n);
    for (auto& e : edges) uf.unite(e[0], e[1]);
    return uf.count;
}

int main() {
    int n1 = 5;
    vector<vector<int>> e1 = {{0,1},{1,2},{3,4}};
    cout << countComponents(n1, e1) << endl; // 2
    
    int n2 = 6;
    vector<vector<int>> e2 = {{0,1},{1,2},{2,3},{4,5}};
    cout << countComponents(n2, e2) << endl; // 2
    
    return 0;
}
