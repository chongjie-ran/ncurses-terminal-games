// Union-Find 并查集 模板
// LC547 省份数量 / LC200 岛屿数量（并查集版）
#include <iostream>
#include <vector>
using namespace std;

class UnionFind {
    vector<int> parent, rank;
public:
    UnionFind(int n) {
        parent.resize(n);
        rank.assign(n, 0);
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
    bool same(int a, int b) { return find(a) == find(b); }
};

int main() {
    // Test LC547: 省份数量
    vector<vector<int>> isConnected = {
        {1,1,0,1},
        {1,1,0,0},
        {0,0,1,0},
        {1,0,0,1}
    };
    int n = isConnected.size();
    UnionFind uf(n);
    for (int i = 0; i < n; i++)
        for (int j = i+1; j < n; j++)
            if (isConnected[i][j]) uf.unite(i, j);
    int provinces = 0;
    for (int i = 0; i < n; i++) if (uf.find(i) == i) provinces++;
    cout << provinces << endl; // 2

    // Test LC200: 岛屿数量（并查集版）
    vector<vector<char>> grid = {
        {'1','1','1','1','0'},
        {'1','1','0','1','0'},
        {'1','1','0','0','0'},
        {'0','0','0','0','0'}
    };
    int m = grid.size(), mm = grid[0].size();
    UnionFind uf2(m * mm);
    auto idx = [&](int i, int j){ return i * mm + j; };
    int dirs[2][2] = {{0,1},{1,0}};
    for (int i = 0; i < m; i++)
        for (int j = 0; j < mm; j++)
            if (grid[i][j] == '1')
                for (auto& d : dirs) {
                    int ni = i + d[0], nj = j + d[1];
                    if (ni < m && nj < mm && grid[ni][nj] == '1')
                        uf2.unite(idx(i,j), idx(ni,nj));
                }
    int islands = 0;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < mm; j++)
            if (grid[i][j] == '1' && uf2.find(idx(i,j)) == idx(i,j))
                islands++;
    cout << islands << endl; // 1
    return 0;
}
