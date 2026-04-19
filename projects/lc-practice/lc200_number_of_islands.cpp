#include <iostream>
#include <vector>
using namespace std;

// LC200 Number of Islands - BFS
// 找到grid中被水包围的陆地块数
class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        int count = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == '1') {
                    count++;
                    bfs(grid, i, j);
                }
            }
        }
        return count;
    }
    
    void bfs(vector<vector<char>>& grid, int i, int j) {
        int m = grid.size(), n = grid[0].size();
        queue<pair<int,int>> q;
        q.push({i,j});
        grid[i][j] = '0';
        int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        while (!q.empty()) {
            auto [x,y] = q.front(); q.pop();
            for (auto& d : dirs) {
                int nx = x + d[0], ny = y + d[1];
                if (nx>=0 && nx<m && ny>=0 && ny<n && grid[nx][ny]=='1') {
                    grid[nx][ny] = '0';
                    q.push({nx,ny});
                }
            }
        }
    }
};

int main() {
    Solution sol;
    
    // Test 1: 3 islands
    vector<vector<char>> grid1 = {
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };
    cout << "Test1: " << sol.numIslands(grid1) << " (expected 3)" << endl;
    
    // Test 2: 1 island
    vector<vector<char>> grid2 = {
        {'1','1','1'},
        {'0','1','0'},
        {'1','1','1'}
    };
    cout << "Test2: " << sol.numIslands(grid2) << " (expected 1)" << endl;
    
    // Test 3: 0 islands
    vector<vector<char>> grid3 = {
        {'0','0'},
        {'0','0'}
    };
    cout << "Test3: " << sol.numIslands(grid3) << " (expected 0)" << endl;
    
    // Test 4: all islands
    vector<vector<char>> grid4 = {
        {'1','1','1'},
        {'1','1','1'},
        {'1','1','1'}
    };
    cout << "Test4: " << sol.numIslands(grid4) << " (expected 1)" << endl;
    
    return 0;
}
