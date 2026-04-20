// LC947 Most Stones Removed with Same Row or Column
// 并查集: 同行或同列的石头可以移除到只剩1个
// 答案 = 总石头数 - 连通分量数
#include <iostream>
#include <vector>
#include <unordered_set>
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
    void unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return;
        if (rank[px] < rank[py]) swap(px, py);
        parent[py] = px;
        if (rank[px] == rank[py]) rank[px]++;
    }
};

int removeStones(vector<vector<int>>& stones) {
    // 坐标范围: rows 0~999, cols 10000~19999 (2万足够)
    const int OFFSET = 10000;
    UnionFind uf(20000);
    for (auto& s : stones) {
        uf.unite(s[0], s[1] + OFFSET);
    }
    // 统计连通分量: 所有石头的唯一root数
    unordered_set<int> roots;
    for (auto& s : stones) {
        roots.insert(uf.find(s[0]));
    }
    return stones.size() - roots.size();
}

int main() {
    // Test 1: [[0,0],[0,1],[1,0],[1,2],[2,1],[2,2]] -> 5
    {
        vector<vector<int>> stones = {{0,0},{0,1},{1,0},{1,2},{2,1},{2,2}};
        cout << removeStones(stones) << endl; // 5
    }
    // Test 2: [[0,0],[0,2],[1,1],[2,0],[2,2]] -> 3
    {
        vector<vector<int>> stones = {{0,0},{0,2},{1,1},{2,0},{2,2}};
        cout << removeStones(stones) << endl; // 3
    }
    // Test 3: [[0,0]] -> 0
    {
        vector<vector<int>> stones = {{0,0}};
        cout << removeStones(stones) << endl; // 0
    }
    // Test 4: [[0,1],[1,2],[1,2],[2,2],[3,3],[3,4]] -> 4
    {
        vector<vector<int>> stones = {{0,1},{1,2},{3,3},{3,4}};
        cout << removeStones(stones) << endl; // 2
    }
    return 0;
}
