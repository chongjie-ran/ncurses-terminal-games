// LC547 Number of Provinces - Union-Find
#include <vector>
using namespace std;

class UnionFind {
public:
    vector<int> parent, rank;
    UnionFind(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++) parent[i] = i;
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
    }
};

int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    UnionFind uf(n);
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (isConnected[i][j]) uf.unite(i, j);
        }
    }
    int provinces = 0;
    for (int i = 0; i < n; i++) {
        if (uf.find(i) == i) provinces++;
    }
    return provinces;
}

// Test: [[1,1,0],[1,1,0],[0,0,1]] -> 2 provinces
int main() {
    vector<vector<int>> isConnected = {{1,1,0},{1,1,0},{0,0,1}};
    // Result: 2
    return 0;
}
