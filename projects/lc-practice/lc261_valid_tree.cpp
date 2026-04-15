#include <vector>
using namespace std;

// LC261 Graph Valid Tree
// Union-Find判断是否为有效树
// 条件：n个节点连通 + 无环
class UnionFind {
    vector<int> parent, rank;
public:
    UnionFind(int n): parent(n), rank(n, 0) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int a, int b) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return false; // 已连通，有环
        if (rank[ra] < rank[rb]) swap(ra, rb);
        parent[rb] = ra;
        if (rank[ra] == rank[rb]) rank[ra]++;
        return true;
    }
};

bool validTree(int n, vector<vector<int>>& edges) {
    if (edges.size() != n - 1) return false; // 边数不对
    
    UnionFind uf(n);
    for (auto& e : edges) {
        if (!uf.unite(e[0], e[1])) return false; // 有环
    }
    return true; // n个节点，n-1条边，无环 => 树
}

// 测试
#include <iostream>
int main() {
    // Test1: n=5, edges=[[0,1],[0,2],[0,3],[1,4]] -> true
    vector<vector<int>> e1 = {{0,1},{0,2},{0,3},{1,4}};
    cout << (validTree(5, e1) == true ? "✓" : "✗") << " Test1: true" << endl;
    
    // Test2: n=5, edges=[[0,1],[0,2],[1,3],[1,4]] -> true (树)
    vector<vector<int>> e2 = {{0,1},{0,2},{1,3},{1,4}};
    cout << (validTree(5, e2) == true ? "✓" : "✗") << " Test2: true" << endl;
    
    // Test3: n=4, edges=[[0,1],[2,3]] -> false (不连通)
    vector<vector<int>> e3 = {{0,1},{2,3}};
    cout << (validTree(4, e3) == false ? "✓" : "✗") << " Test3: false (disconnected)" << endl;
    
    // Test4: n=4, edges=[[0,1],[1,2],[2,1]] -> false (有环)
    vector<vector<int>> e4 = {{0,1},{1,2},{2,1}};
    cout << (validTree(4, e4) == false ? "✓" : "✗") << " Test4: false (cycle)" << endl;
    
    // Test5: n=1, edges=[] -> true
    vector<vector<int>> e5 = {};
    cout << (validTree(1, e5) == true ? "✓" : "✗") << " Test5: true (single node)" << endl;
    
    return 0;
}
