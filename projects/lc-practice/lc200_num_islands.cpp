// LC200 Number of Islands
// BFS from each unvisited land cell, mark visited.
// Time: O(m*n), Space: O(min(m*n, BFS queue))

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        int m = grid.size(), n = grid[0].size();
        int count = 0;
        static const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == '1') {
                    count++;
                    queue<pair<int,int>> q;
                    q.emplace(i, j);
                    grid[i][j] = '0';
                    while (!q.empty()) {
                        auto [x, y] = q.front(); q.pop();
                        for (auto& d : dirs) {
                            int nx = x+d[0], ny = y+d[1];
                            if (nx>=0 && nx<m && ny>=0 && ny<n && grid[nx][ny]=='1') {
                                grid[nx][ny] = '0';
                                q.emplace(nx, ny);
                            }
                        }
                    }
                }
            }
        }
        return count;
    }
};

int main() {
    Solution s;
    vector<vector<char>> g1 = {
        {'1','1','1','1','0'},
        {'1','1','0','1','0'},
        {'1','1','0','0','0'},
        {'0','0','0','0','0'}
    };
    cout << s.numIslands(g1) << endl; // 1

    vector<vector<char>> g2 = {
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };
    cout << s.numIslands(g2) << endl; // 3
    return 0;
}
