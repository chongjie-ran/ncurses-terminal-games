// LC323 Count Connected Components in Undirected Graph
// 并查集统计无向图的连通分量数
#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

class UnionFind {
public:
    vector<int> parent, rank;
    int count;
    UnionFind(int n) : count(n) {
        parent.resize(n);
        rank.assign(n, 0);
        iota(parent.begin(), parent.end(), 0);
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
};

int countComponents(int n, vector<vector<int>>& edges) {
    UnionFind uf(n);
    for (auto& e : edges) {
        uf.unite(e[0], e[1]);
    }
    return uf.count;
}

int main() {
    // Test 1: n=5, edges=[[0,1],[1,2],[3,4]] -> 2
    {
        int n = 5;
        vector<vector<int>> edges = {{0,1},{1,2},{3,4}};
        cout << countComponents(n, edges) << endl; // 2
    }
    // Test 2: n=6, edges=[[0,1],[1,2],[2,3],[4,5]] -> 2
    {
        int n = 6;
        vector<vector<int>> edges = {{0,1},{1,2},{2,3},{4,5}};
        cout << countComponents(n, edges) << endl; // 2
    }
    // Test 3: n=5, edges=[] -> 5
    {
        int n = 5;
        vector<vector<int>> edges = {};
        cout << countComponents(n, edges) << endl; // 5
    }
    // Test 4: n=4, edges=[[0,1],[2,3],[1,2]] -> 1
    {
        int n = 4;
        vector<vector<int>> edges = {{0,1},{2,3},{1,2}};
        cout << countComponents(n, edges) << endl; // 1
    }
    return 0;
}
