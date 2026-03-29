// LC685 Redundant Connection II — 冗余连接 II
// Hard 并查集变体：有向图，最多一笔边导致冲突
// 两种情况：
// 1. 一个节点有两个父节点（入度为2），其中一条边是冗余的
// 2. 整个图有环（所有边入度<=1，但DAG检测到环）
// 策略：先检查入度为2的情况（优先级更高）

#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

class DSU {
public:
    vector<int> parent, rank;
    DSU(int n): parent(n+1), rank(n+1, 0) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    // 检测 x 和 y 是否已连通（用于环检测）
    bool unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return false;  // 已连通，会成环
        if (rank[x] < rank[y]) swap(x, y);
        parent[y] = x;
        if (rank[x] == rank[y]) ++rank[x];
        return true;
    }
    // 重置：重新初始化 parent
    void reset(int n) {
        iota(parent.begin(), parent.end(), 0);
        fill(rank.begin(), rank.end(), 0);
    }
};

class Solution {
public:
    vector<int> findRedundantDirectedConnection(vector<vector<int>>& edges) {
        int n = edges.size();
        vector<int> indegree(n + 1, 0);
        int conflict = -1;  // 入度为2的节点
        int conflict_edge = -1;  // 导致入度为2的那条边索引
        
        // 第一遍：找入度为2的节点和对应的边
        for (int i = 0; i < n; ++i) {
            int u = edges[i][0], v = edges[i][1];
            indegree[v]++;
            if (indegree[v] == 2) {
                conflict = v;
                conflict_edge = i;
            }
        }
        
        // 情况1：存在入度为2的节点
        // 优先尝试删除冲突边本身（后者），因为题目要求"最后出现的冗余边"
        if (conflict != -1) {
            // 先尝试删除冲突边（靠后的那条），如果成树则返回
            if (isTreeAfterRemove(edges, conflict_edge))
                return edges[conflict_edge];
            // 否则删除第一条（导致冲突的第一条边）也必然成树
            return edges[conflict_edge - 1];
        }
        
        // 情况2：无入度为2，但有环 → 直接在原图中找成环的边
        DSU dsu(n);
        for (const auto& e : edges) {
            if (!dsu.unite(e[0], e[1]))
                return e;  // 第二次试图 unite 已连通的节点 = 环
        }
        return {};  // 不应该走到这里
    }
    
    // 删除第 remove_idx 条边后，检查图是否变成树（无环且连通）
    bool isTreeAfterRemove(const vector<vector<int>>& edges, int remove_idx) {
        int n = edges.size();
        DSU dsu(n);
        for (int i = 0; i < n; ++i) {
            if (i == remove_idx) continue;
            if (!dsu.unite(edges[i][0], edges[i][1]))
                return false;  // 成环了
        }
        // 检查连通分量数是否为1
        int components = 0;
        for (int i = 1; i <= n; ++i)
            if (dsu.find(i) == i) ++components;
        return components == 1;
    }
};

int main() {
    Solution sol;
    
    // Test 1: [[1,2], [1,3], [2,3]] — 节点3入度为2，删除[2,3]
    // 期望: [2,3]
    vector<vector<int>> t1 = {{1,2},{1,3},{2,3}};
    auto r1 = sol.findRedundantDirectedConnection(t1);
    cout << r1[0] << "," << r1[1] << " (期望 2,3)" << endl;
    
    // Test 2: [[1,2], [2,3], [3,4], [4,1], [1,5]] — 环+多余边
    // 删除 [4,1] 后成树
    vector<vector<int>> t2 = {{1,2},{2,3},{3,4},{4,1},{1,5}};
    auto r2 = sol.findRedundantDirectedConnection(t2);
    cout << r2[0] << "," << r2[1] << " (期望 4,1)" << endl;
    
    // Test 3: [[1,2],[2,3],[3,1]] — 简单环
    vector<vector<int>> t3 = {{1,2},{2,3},{3,1}};
    auto r3 = sol.findRedundantDirectedConnection(t3);
    cout << r3[0] << "," << r3[1] << " (期望 3,1)" << endl;
    
    return 0;
}
