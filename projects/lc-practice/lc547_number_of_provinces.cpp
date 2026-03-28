// LC547 Number of Provinces
// 题目: n个城市，某些对相连，求连通分量数量
// 算法: 并查集 Union-Find
// 验证: ✅ Test1=2, Test2=3

#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

class Solution {
public:
    vector<int> parent, rank;
    
    int find(int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]);
    }
    
    void unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return;
        if (rank[a] < rank[b]) swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b]) ++rank[a];
    }
    
    int findCircleNum(vector<vector<int>>& isConnected) {
        int n = isConnected.size();
        parent.resize(n);
        rank.assign(n, 0);
        iota(parent.begin(), parent.end(), 0);
        
        // 只需遍历上三角，减少重复 unify
        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j < n; ++j)
                if (isConnected[i][j])
                    unite(i, j);
        
        int cnt = 0;
        for (int i = 0; i < n; ++i)
            if (parent[i] == i) ++cnt;
        return cnt;
    }
};

int main() {
    Solution s;
    vector<vector<int>> t1 = {{1,1,0},{1,1,0},{0,0,1}};
    vector<vector<int>> t2 = {{1,0,0},{0,1,0},{0,0,1}};
    cout << s.findCircleNum(t1) << endl; // 2
    cout << s.findCircleNum(t2) << endl; // 3
    return 0;
}
