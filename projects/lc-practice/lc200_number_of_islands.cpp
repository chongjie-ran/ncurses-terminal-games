// LC200 Number of Islands - BFS/DFS
// 2026-03-31 07:40
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    // 方法1: BFS
    int numIslands(vector<vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        int count = 0;
        vector<vector<int>> dirs = {{1,0},{-1,0},{0,1},{0,-1}};
        
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == '1') {
                    count++;
                    // BFS flood fill
                    queue<pair<int,int>> q;
                    q.push({i,j});
                    grid[i][j] = '0';
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
            }
        }
        return count;
    }
    
    // 方法2: DFS (递归)
    void dfs(vector<vector<char>>& grid, int i, int j, int m, int n) {
        if (i<0||i>=m||j<0||j>=n||grid[i][j]=='0') return;
        grid[i][j] = '0';
        dfs(grid, i+1, j, m, n);
        dfs(grid, i-1, j, m, n);
        dfs(grid, i, j+1, m, n);
        dfs(grid, i, j-1, m, n);
    }
    
    int numIslandsDFS(vector<vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        int count = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == '1') {
                    count++;
                    dfs(grid, i, j, m, n);
                }
            }
        }
        return count;
    }
};
