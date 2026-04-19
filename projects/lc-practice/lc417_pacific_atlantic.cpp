#include <iostream>
#include <vector>
using namespace std;

// LC417 Pacific Atlantic Water Flow - 多源DFS洪水填充
// 找到能同时流到Pacific和Atlantic的格子
class Solution {
public:
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
        if (heights.empty() || heights[0].empty()) return {};
        int m = heights.size(), n = heights[0].size();
        vector<vector<bool>> pacific(m, vector<bool>(n, false));
        vector<vector<bool>> atlantic(m, vector<bool>(n, false));
        
        // 从4条边界DFS
        for (int i = 0; i < m; i++) {
            dfs(heights, pacific, i, 0);       // left boundary -> pacific
            dfs(heights, atlantic, i, n-1);    // right boundary -> atlantic
        }
        for (int j = 0; j < n; j++) {
            dfs(heights, pacific, 0, j);         // top boundary -> pacific
            dfs(heights, atlantic, m-1, j);    // bottom boundary -> atlantic
        }
        
        // 找交集
        vector<vector<int>> result;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (pacific[i][j] && atlantic[i][j])
                    result.push_back({i,j});
            }
        }
        return result;
    }
    
    void dfs(const vector<vector<int>>& h, vector<vector<bool>>& visited, int i, int j) {
        int m = h.size(), n = h[0].size();
        visited[i][j] = true;
        int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (auto& d : dirs) {
            int nx = i + d[0], ny = j + d[1];
            if (nx>=0 && nx<m && ny>=0 && ny<n && !visited[nx][ny] && h[nx][ny] >= h[i][j])
                dfs(h, visited, nx, ny);
        }
    }
};

int main() {
    Solution sol;
    
    // Test 1
    vector<vector<int>> h1 = {
        {1,2,2,3,5},
        {3,2,3,4,4},
        {2,4,5,3,1},
        {6,7,1,4,5},
        {5,1,1,2,4}
    };
    auto r1 = sol.pacificAtlantic(h1);
    cout << "Test1: " << r1.size() << " cells (expected 7)" << endl;
    cout << "Cells: ";
    for (auto& c : r1) cout << "(" << c[0] << "," << c[1] << ") ";
    cout << endl;
    
    // Test 2: single cell
    vector<vector<int>> h2 = {{1}};
    auto r2 = sol.pacificAtlantic(h2);
    cout << "Test2: " << r2.size() << " (expected 1)" << endl;
    
    return 0;
}
