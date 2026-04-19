// LC323 Count Connected Components - 并查集
// 给定n个节点和边列表，返回连通分量数量
#include <iostream>
#include <vector>
using namespace std;

// ============ 并查集 ============
class UnionFind {
public:
    vector<int> parent, rank;
    int count;
    
    UnionFind(int n) {
        parent.resize(n);
        rank.assign(n, 0);
        count = n;
        for (int i = 0; i < n; i++) parent[i] = i;
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
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

int countComponents(int n, vector<vector<int>>& edges) {
    UnionFind uf(n);
    for (auto& e : edges) {
        uf.unite(e[0], e[1]);
    }
    return uf.count;
}

// ============ 测试 ============
int main() {
    // Test 1: n=5, edges=[[0,1],[1,2],[3,4]] → 2个连通分量
    vector<vector<int>> e1 = {{0,1},{1,2},{3,4}};
    int r1 = countComponents(5, e1);
    cout << "Test1 (expect 2): " << r1 << endl;
    
    // Test 2: n=5, edges=[] → 5个孤立点
    vector<vector<int>> e2 = {};
    int r2 = countComponents(5, e2);
    cout << "Test2 (expect 5): " << r2 << endl;
    
    // Test 3: n=6, edges=[[0,1],[2,3],[4,5]] → 3个连通分量
    vector<vector<int>> e3 = {{0,1},{2,3},{4,5}};
    int r3 = countComponents(6, e3);
    cout << "Test3 (expect 3): " << r3 << endl;
    
    // Test 4: n=4, edges=[[0,1],[1,2],[2,3]] → 1个连通分量
    vector<vector<int>> e4 = {{0,1},{1,2},{2,3}};
    int r4 = countComponents(4, e4);
    cout << "Test4 (expect 1): " << r4 << endl;
    
    // Test 5: n=2, edges=[[1,0]] → 1个连通分量
    vector<vector<int>> e5 = {{1,0}};
    int r5 = countComponents(2, e5);
    cout << "Test5 (expect 1): " << r5 << endl;
    
    return 0;
}
