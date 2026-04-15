#include <vector>
using namespace std;

// LC323 Count Connected Components in Undirected Graph
// Union-Find模板直接应用
class UnionFind {
    vector<int> parent, rank;
    int count;
public:
    UnionFind(int n): parent(n), rank(n, 0), count(n) {
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
        count--;
    }
    int getCount() { return count; }
};

int countComponents(int n, vector<vector<int>>& edges) {
    UnionFind uf(n);
    for (auto& e : edges) uf.unite(e[0], e[1]);
    return uf.getCount();
}

#include <iostream>
int main() {
    // Test1: n=5, edges=[[0,1],[1,2],[3,4]] -> 2
    vector<vector<int>> e1 = {{0,1},{1,2},{3,4}};
    cout << (countComponents(5, e1) == 2 ? "✓" : "✗") << " Test1: 2" << endl;
    
    // Test2: n=5, edges=[[0,1],[1,2],[2,3],[3,4]] -> 1
    vector<vector<int>> e2 = {{0,1},{1,2},{2,3},{3,4}};
    cout << (countComponents(5, e2) == 1 ? "✓" : "✗") << " Test2: 1" << endl;
    
    // Test3: n=6, edges=[] -> 6
    vector<vector<int>> e3 = {};
    cout << (countComponents(6, e3) == 6 ? "✓" : "✗") << " Test3: 6" << endl;
    
    // Test4: n=4, edges=[[0,1],[2,3],[1,2]] -> 1
    vector<vector<int>> e4 = {{0,1},{2,3},{1,2}};
    cout << (countComponents(4, e4) == 1 ? "✓" : "✗") << " Test4: 1" << endl;
    
    return 0;
}
