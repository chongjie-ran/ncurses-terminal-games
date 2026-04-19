#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// LC994 Rotting Oranges - BFS多源扩散
// 腐烂橘子从4个方向感染新鲜橘子，返回感染全部的时间
class Solution {
public:
    int orangesRotting(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        queue<pair<int,int>> q;
        int fresh = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 2) q.push({i,j});
                else if (grid[i][j] == 1) fresh++;
            }
        }
        if (fresh == 0) return 0;
        if (q.empty()) return -1;
        
        int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        int minutes = 0;
        while (!q.empty()) {
            int sz = q.size();
            while (sz-- > 0) {
                auto [x,y] = q.front(); q.pop();
                for (auto& d : dirs) {
                    int nx = x + d[0], ny = y + d[1];
                    if (nx>=0 && nx<m && ny>=0 && ny<n && grid[nx][ny]==1) {
                        grid[nx][ny] = 2;
                        fresh--;
                        q.push({nx,ny});
                    }
                }
            }
            if (!q.empty()) minutes++;
        }
        return fresh == 0 ? minutes : -1;
    }
};

int main() {
    Solution sol;
    
    // Test 1: 2 minutes
    vector<vector<int>> g1 = {{2,1,1},{1,1,0},{0,1,1}};
    cout << "Test1: " << sol.orangesRotting(g1) << " (expected 4)" << endl;
    
    // Test 2: -1 (impossible)
    vector<vector<int>> g2 = {{2,1,1},{0,1,1},{1,0,0}};
    cout << "Test2: " << sol.orangesRotting(g2) << " (expected -1)" << endl;
    
    // Test 3: 0 minutes
    vector<vector<int>> g3 = {{0,2}};
    cout << "Test3: " << sol.orangesRotting(g3) << " (expected 0)" << endl;
    
    // Test 4: all fresh
    vector<vector<int>> g4 = {{1,1},{1,1}};
    cout << "Test4: " << sol.orangesRotting(g4) << " (expected -1)" << endl;
    
    // Test 5: already all rotten
    vector<vector<int>> g5 = {{2,2},{2,2}};
    cout << "Test5: " << sol.orangesRotting(g5) << " (expected 0)" << endl;
    
    return 0;
}
