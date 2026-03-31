// LC547 - Number of Provinces
// 分类: Union-Find / BFS DFS
// 难度: Medium
// 思路: N 个城市，部分城市直接相连形成省份
// 方法1: Union-Find 统计连通分量
// 方法2: BFS 从每个未访问节点开始扩展

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// Union-Find
class UF {
public:
    vector<int> parent, rank;
    UF(int n): parent(n), rank(n, 0) {
        for(int i=0;i<n;i++) parent[i]=i;
    }
    int find(int x) {
        if(parent[x]!=x) parent[x]=find(parent[x]);
        return parent[x];
    }
    void unite(int a, int b) {
        int ra=find(a), rb=find(b);
        if(ra==rb) return;
        if(rank[ra]<rank[rb]) swap(ra,rb);
        parent[rb]=ra;
        if(rank[ra]==rank[rb]) rank[ra]++;
    }
};

class Solution {
public:
    int findCircleNum(vector<vector<int>>& isConnected) {
        int n = isConnected.size();
        UF uf(n);
        for(int i=0;i<n;i++)
            for(int j=i+1;j<n;j++)
                if(isConnected[i][j]) uf.unite(i,j);
        int provinces = 0;
        for(int i=0;i<n;i++) if(uf.find(i)==i) provinces++;
        return provinces;
    }
};
