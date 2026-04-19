// LC547 Number of Provinces - Union-Find
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

int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    UnionFind uf(n);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (isConnected[i][j]) uf.unite(i, j);
    return uf.count;
}

int main() {
    vector<vector<int>> g1 = {{1,1,0},{1,1,0},{0,0,1}};
    cout << "g1: " << findCircleNum(g1) << " (expect 2)" << endl;
    vector<vector<int>> g2 = {{1,1,0},{1,1,1},{0,1,1}};
    cout << "g2: " << findCircleNum(g2) << " (expect 1)" << endl;
    return 0;
}
