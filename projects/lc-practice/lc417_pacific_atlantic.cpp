#include <bits/stdc++.h>
using namespace std;

// LC417 Pacific Atlantic Water Flow
// 多源BFS/DFS：从边界向内DFS，能流到Pacific和Atlantic的格子即为答案

class Solution {
public:
    vector<vector<int>> heights;
    int m, n;
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
        this->heights = heights;
        m = heights.size();
        n = heights[0].size();
        vector<vector<int>> pacific(m, vector<int>(n, 0));
        vector<vector<int>> atlantic(m, vector<int>(n, 0));
        queue<pair<int,int>> pq, aq;
        
        // 从4条边界出发
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
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (pacific[i][j] && atlantic[i][j])
                    result.push_back({i, j});
            }
        }
        return result;
    }
    
    void dfs(int r, int c, vector<vector<int>>& visited) {
        if (visited[r][c]) return;
        visited[r][c] = 1;
        int dr[4] = {-1, 0, 1, 0};
        int dc[4] = {0, 1, 0, -1};
        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= m || nc < 0 || nc >= n) continue;
            if (heights[nr][nc] >= heights[r][c])
                dfs(nr, nc, visited);
        }
    }
};

// 测试
int main() {
    Solution sol;
    
    // Test 1: 基本示例
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
    
    // Test 2: 全是高地
    vector<vector<int>> h2 = {{1}};
    auto r2 = sol.pacificAtlantic(h2);
    cout << "Test 2: ";
    for (auto& p : r2) cout << "(" << p[0] << "," << p[1] << ") ";
    cout << endl;
    
    // Test 3: 2x2
    vector<vector<int>> h3 = {{1,2},{3,4}};
    auto r3 = sol.pacificAtlantic(h3);
    cout << "Test 3: ";
    for (auto& p : r3) cout << "(" << p[0] << "," << p[1] << ") ";
    cout << endl;
    
    return 0;
}
