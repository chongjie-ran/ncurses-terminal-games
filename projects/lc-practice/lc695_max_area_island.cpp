// LC695 Max Area of Island
// Medium - DFS/BFS
// 给定二维网格grid,找最大岛屿的面积(岛屿=相连的1)
// Time: O(m*n) Space: O(m*n)

#include <vector>
#include <queue>
#include <iostream>
using namespace std;

// 方法1: DFS (推荐,代码简洁)
class Solution1 {
public:
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        int maxArea = 0;
        
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
        return 1 + dfs(grid, i+1, j, m, n) 
                 + dfs(grid, i-1, j, m, n)
                 + dfs(grid, i, j+1, m, n)
                 + dfs(grid, i, j-1, m, n);
    }
};

// 方法2: BFS (避免递归栈溢出)
class Solution2 {
public:
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        int maxArea = 0;
        vector<vector<int>> dirs = {{1,0}, {-1,0}, {0,1}, {0,-1}};
        
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    int area = 0;
                    queue<pair<int,int>> q;
                    q.emplace(i, j);
                    grid[i][j] = 0;
                    while (!q.empty()) {
                        auto [x, y] = q.front(); q.pop();
                        area++;
                        for (auto& d : dirs) {
                            int nx = x + d[0], ny = y + d[1];
                            if (nx >= 0 && nx < m && ny >= 0 && ny < n && grid[nx][ny] == 1) {
                                grid[nx][ny] = 0;
                                q.emplace(nx, ny);
                            }
                        }
                    }
                    maxArea = max(maxArea, area);
                }
            }
        }
        return maxArea;
    }
};

// 测试
int main() {
    Solution1 s1;
    Solution2 s2;
    
    // Test1: max island area=1 (single cell)
    vector<vector<int>> grid1 = {{1}};
    cout << "Test1: " << s1.maxAreaOfIsland(grid1) << " (expected 1)" << endl;
    
    // Test2: max island area=4 (2x2 block)
    vector<vector<int>> grid2 = {{1,1},{1,1}};
    cout << "Test2: " << s2.maxAreaOfIsland(grid2) << " (expected 4)" << endl;
    
    // Test3: max island area=6 (L-shaped: 2x2 + 2 extra)
    vector<vector<int>> grid3 = {{1,1,1},{1,0,1},{1,1,1}};
    cout << "Test3: " << s1.maxAreaOfIsland(grid3) << " (expected 8)" << endl;
    
    // Test4: no islands
    vector<vector<int>> grid4 = {{0,0,0},{0,0,0}};
    cout << "Test4: " << s2.maxAreaOfIsland(grid4) << " (expected 0)" << endl;
    
    // Test5: multiple islands, max area=1
    vector<vector<int>> grid5 = {{0,1,0},{0,0,0},{0,0,1}};
    cout << "Test5: " << s1.maxAreaOfIsland(grid5) << " (expected 1)" << endl;
    
    return 0;
}

// 核心洞察:
// 1. 与LC200 Number of Islands类似,但需要计算每个岛屿的面积并取最大值
// 2. DFS/BFS flood fill时累加面积: return 1 + 递归(四个方向)
// 3. 访问过的格子标记为0(grid[i][j]=0),避免重复计数
// 4. 遍历所有格子,对每个岛屿做一次flood fill,取最大面积
// 5. 边界情况: 全是水域(0)则返回0
