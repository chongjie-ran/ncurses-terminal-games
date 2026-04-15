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
                    int area = dfs(grid, i, j, m, n);
                    maxArea = max(maxArea, area);
                }
            }
        }
        return maxArea;
    }
private:
    int dfs(vector<vector<int>>& grid, int i, int j, int m, int n) {
        if (i < 0 || i >= m || j < 0 || j >= n || grid[i][j] == 0) return 0;
        grid[i][j] = 0;
        return 1 + dfs(grid, i+1, j, m, n) + dfs(grid, i-1, j, m, n)
                   + dfs(grid, i, j+1, m, n) + dfs(grid, i, j-1, m, n);
    }
};

int main() {
    Solution sol;
    vector<vector<int>> g3 = {
        {1,1,0,1,1},
        {1,0,0,0,1},
        {0,0,0,1,0},
        {1,1,0,1,1}
    };
    cout << "g3 max area: " << sol.maxAreaOfIsland(g3) << " (expected 5)" << endl;
    return 0;
}
