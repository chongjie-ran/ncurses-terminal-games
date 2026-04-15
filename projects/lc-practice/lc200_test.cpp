// LC200 Number of Islands - Fixed
#include <iostream>
#include <vector>
using namespace std;

// Union-Find
class UnionFind {
public:
    UnionFind(int m, int n, const vector<vector<char>>& grid) {
        parent.resize(m * n);
        rank.resize(m * n, 0);
        count = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                int id = i * n + j;
                parent[id] = id;
                if (grid[i][j] == '1') count++;
            }
        }
    }
    
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    
    void unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return;
        if (rank[px] < rank[py]) swap(px, py);
        parent[py] = px;
        if (rank[px] == rank[py]) rank[px]++;
        count--;
    }
    
    int getCount() { return count; }
    
private:
    vector<int> parent, rank;
    int count;
};

int numIslandsUF(vector<vector<char>>& grid) {
    if (grid.empty()) return 0;
    int m = grid.size(), n = grid[0].size();
    UnionFind uf(m, n, grid);
    vector<pair<int,int>> dirs = {{1,0},{-1,0},{0,1},{0,-1}};
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '1') {
                int id = i * n + j;
                for (auto d : dirs) {
                    int ni = i + d.first, nj = j + d.second;
                    if (ni >= 0 && ni < m && nj >= 0 && nj < n && grid[ni][nj] == '1') {
                        uf.unite(id, ni * n + nj);
                    }
                }
            }
        }
    }
    return uf.getCount();
}

int main() {
    vector<vector<char>> g1 = {
        {'1','1','1','1','0'},
        {'1','1','0','1','0'},
        {'1','1','0','0','0'},
        {'0','0','0','0','0'}
    };
    cout << numIslandsUF(g1) << endl; // 1
    
    vector<vector<char>> g2 = {
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };
    cout << numIslandsUF(g2) << endl; // 3
    
    return 0;
}
