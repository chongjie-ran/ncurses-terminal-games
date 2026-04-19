// LC261 Graph Valid Tree - Union-Find判断是否为有效树
// 条件: n个节点, n-1条边, 无环(连通)
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
        if (ra == rb) return false; // 已有环
        if (rank[ra] < rank[rb]) swap(ra, rb);
        parent[rb] = ra;
        if (rank[ra] == rank[rb]) rank[ra]++;
        return true;
    }
};

bool validTree(int n, vector<vector<int>>& edges) {
    if (edges.size() != n - 1) return false;
    UnionFind uf(n);
    for (auto& e : edges) {
        if (!uf.unite(e[0], e[1])) return false; // 形成环
    }
    return true;
}

int main() {
    int n1 = 5;
    vector<vector<int>> e1 = {{0,1},{0,2},{0,3},{1,4}};
    cout << validTree(n1, e1) << " (expect 1)" << endl;
    
    int n2 = 5;
    vector<vector<int>> e2 = {{0,1},{1,2},{2,3},{1,3},{3,4}};
    cout << validTree(n2, e2) << " (expect 0)" << endl;
    
    return 0;
}
