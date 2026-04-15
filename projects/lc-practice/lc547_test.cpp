// LC547 Number of Provinces
// Medium | Union-Find
#include <iostream>
#include <vector>
using namespace std;

class UnionFind {
public:
    UnionFind(int n) {
        parent.resize(n);
        rank.resize(n, 0);
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
    
    int getCount() { return count; }
    
private:
    vector<int> parent, rank;
    int count;
};

int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    UnionFind uf(n);
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            if (isConnected[i][j] == 1) {
                uf.unite(i, j);
            }
        }
    }
    return uf.getCount();
}

int main() {
    vector<vector<int>> c1 = {
        {1,1,0},
        {1,1,0},
        {0,0,1}
    };
    cout << findCircleNum(c1) << endl; // 2
    
    vector<vector<int>> c2 = {
        {1,0,0},
        {0,1,0},
        {0,0,1}
    };
    cout << findCircleNum(c2) << endl; // 3
    
    return 0;
}
