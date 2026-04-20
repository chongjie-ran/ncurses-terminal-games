// LC261 Graph Valid Tree
// 并查集判断图是否为有效树
// 条件: 1)n-1条边 2)无环 3)连通
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
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false; // 已有环
        if (rank[px] < rank[py]) swap(px, py);
        parent[py] = px;
        if (rank[px] == rank[py]) rank[px]++;
        return true;
    }
};

bool validTree(int n, vector<vector<int>>& edges) {
    if (edges.size() != n - 1) return false;
    UnionFind uf(n);
    for (auto& e : edges) {
        if (!uf.unite(e[0], e[1])) return false; // 检测到环
    }
    // 检查连通性: 所有节点parent相同
    int root = uf.find(0);
    for (int i = 1; i < n; i++) {
        if (uf.find(i) != root) return false;
    }
    return true;
}

int main() {
    // Test 1: n=5, edges=[[0,1],[0,2],[0,3],[1,4]] -> true
    {
        int n = 5;
        vector<vector<int>> edges = {{0,1},{0,2},{0,3},{1,4}};
        cout << (validTree(n, edges) ? "true" : "false") << endl; // true
    }
    // Test 2: n=5, edges=[[0,1],[1,2],[2,3],[1,3],[1,4]] -> false (环)
    {
        int n = 5;
        vector<vector<int>> edges = {{0,1},{1,2},{2,3},{1,3},{1,4}};
        cout << (validTree(n, edges) ? "true" : "false") << endl; // false
    }
    // Test 3: n=4, edges=[[0,1],[2,3]] -> false (不连通)
    {
        int n = 4;
        vector<vector<int>> edges = {{0,1},{2,3}};
        cout << (validTree(n, edges) ? "true" : "false") << endl; // false
    }
    // Test 4: n=1, edges=[] -> true
    {
        int n = 1;
        vector<vector<int>> edges = {};
        cout << (validTree(n, edges) ? "true" : "false") << endl; // true
    }
    // Test 5: n=2, edges=[[0,1]] -> true
    {
        int n = 2;
        vector<vector<int>> edges = {{0,1}};
        cout << (validTree(n, edges) ? "true" : "false") << endl; // true
    }
    // Test 6: n=2, edges=[] -> false (不连通)
    {
        int n = 2;
        vector<vector<int>> edges = {};
        cout << (validTree(n, edges) ? "true" : "false") << endl; // false
    }
    return 0;
}
