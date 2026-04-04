// LC695 - Max Area of Island
// 分类: DFS/BFS
// 难度: Medium
// 依赖: LC200 Number of Islands
// 思路: 和LC200类似，但用DFS返回岛屿面积，求最大值
// 时间: O(m*n), 空间: O(m*n)

#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        int maxArea = 0;
        
        function<int(int,int)> dfs = [&](int r, int c) -> int {
            if (r < 0 || r >= m || c < 0 || c >= n || grid[r][c] == 0)
                return 0;
            grid[r][c] = 0; // 标记已访问
            return 1 + dfs(r+1, c) + dfs(r-1, c) + dfs(r, c+1) + dfs(r, c-1);
        };
        
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    int area = dfs(i, j);
                    maxArea = max(maxArea, area);
                }
            }
        }
        return maxArea;
    }
};

int main() {
    Solution sol;
    
    // Test 1: single island
    vector<vector<int>> g1 = {{1,1,0,0,0},{1,1,0,0,0},{0,0,0,1,1},{0,0,0,0,0}};
    cout << sol.maxAreaOfIsland(g1) << endl; // 4
    
    // Test 2: all 1s 2x3
    vector<vector<int>> g2 = {{1,1,1},{1,1,1}};
    cout << sol.maxAreaOfIsland(g2) << endl; // 6
    
    // Test 3: single cell
    vector<vector<int>> g3 = {{1}};
    cout << sol.maxAreaOfIsland(g3) << endl; // 1
    
    // Test 4: all zeros
    vector<vector<int>> g4 = {{0,0},{0,0}};
    cout << sol.maxAreaOfIsland(g4) << endl; // 0
    
    return 0;
}
