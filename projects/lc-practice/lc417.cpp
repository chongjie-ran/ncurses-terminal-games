#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// LC417 Pacific Atlantic Water Flow
// 多源DFS：从边界向内DFS，能流到Pacific和Atlantic的格子即为答案

class Solution {
public:
    vector<vector<int>> heights;
    int m, n;
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& h) {
        heights = h;
        m = heights.size();
        n = heights[0].size();
        vector<vector<int>> pacific(m, vector<int>(n, 0));
        vector<vector<int>> atlantic(m, vector<int>(n, 0));
        
        // 从4条边界出发DFS
        for (int i = 0; i < m; i++) {
            dfs(i, 0, pacific);
            dfs(i, n-1, atlantic);
        }
        for (int j = 0; j < n; j++) {
            dfs(0, j, pacific);
            dfs(m-1, j, atlantic);
        }
        
        // 找交集
        vector<vector<int>> result;
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                if (pacific[i][j] && atlantic[i][j])
                    result.push_back({i, j});
        return result;
    }
    
    void dfs(int r, int c, vector<vector<int>>& vis) {
        if (vis[r][c]) return;
        vis[r][c] = 1;
        int dr[4] = {-1, 0, 1, 0};
        int dc[4] = {0, 1, 0, -1};
        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= m || nc < 0 || nc >= n) continue;
            if (heights[nr][nc] >= heights[r][c])
                dfs(nr, nc, vis);
        }
    }
};

int main() {
    Solution sol;
    
    // Test 1: LeetCode example
    vector<vector<int>> h1 = {
        {1,2,2,3,5},
        {3,2,3,4,4},
        {2,4,5,3,1},
        {6,7,1,4,5},
        {5,1,1,2,4}
    };
    auto r1 = sol.pacificAtlantic(h1);
    cout << "Test 1: ";
    for (auto& p : r1) cout << "(" << p[0] << "," << p[1] << ") ";
    cout << endl;
    // Expected: (0,4) (1,3) (1,4) (2,2) (3,0) (3,1) (4,0)
    
    // Test 2: Single cell
    vector<vector<int>> h2 = {{1}};
    auto r2 = sol.pacificAtlantic(h2);
    cout << "Test 2: ";
    for (auto& p : r2) cout << "(" << p[0] << "," << p[1] << ") ";
    cout << endl;
    
    // Test 3: 2x2 - all can flow to both
    vector<vector<int>> h3 = {{1,2},{3,4}};
    auto r3 = sol.pacificAtlantic(h3);
    cout << "Test 3: ";
    for (auto& p : r3) cout << "(" << p[0] << "," << p[1] << ") ";
    cout << endl;
    
    return 0;
}
