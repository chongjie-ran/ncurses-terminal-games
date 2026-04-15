// LC695 Max Area of Island
// Medium | DFS/BFS
// 问题: grid中1的连通区域面积，求最大面积
// 思路: 遍历每个格子，是1则DFS计算岛屿面积，取max
// 技巧: grid[i][j]=0标记已访问，避免额外visited数组
// 复杂度: O(mn)时间，O(mn)空间(递归栈)
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        if (grid.empty()) return 0;
        int m = grid.size(), n = grid[0].size(), maxArea = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    maxArea = max(maxArea, dfs(grid, i, j, m, n));
                }
            }
        }
        return maxArea;
    }
private:
    int dfs(vector<vector<int>>& grid, int i, int j, int m, int n) {
        if (i < 0 || i >= m || j < 0 || j >= n || grid[i][j] == 0) return 0;
        grid[i][j] = 0; // 标记已访问
        return 1 + dfs(grid, i+1, j, m, n) + dfs(grid, i-1, j, m, n)
                   + dfs(grid, i, j+1, m, n) + dfs(grid, i, j-1, m, n);
    }
};

int main() {
    Solution sol;
    vector<vector<int>> g1 = {{1,1},{1,1}};
    cout << sol.maxAreaOfIsland(g1) << endl; // 4
    vector<vector<int>> g2 = {{0,1,0},{1,1,1},{0,1,0}};
    cout << sol.maxAreaOfIsland(g2) << endl; // 5
    vector<vector<int>> g3 = {{1}};
    cout << sol.maxAreaOfIsland(g3) << endl; // 1
    vector<vector<int>> g4 = {{0,0},{0,0}};
    cout << sol.maxAreaOfIsland(g4) << endl; // 0
    return 0;
}
