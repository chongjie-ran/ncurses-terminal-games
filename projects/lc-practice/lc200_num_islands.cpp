#include <iostream>
#include <vector>
using namespace std;

// 200. Number of Islands
// 洪水填充(DFS/BFS)：遍历网格，碰到'1'则DFS填掉整个岛屿，计数+1
// 复杂度：O(M*N) 时间, O(M*N) 空间(递归栈)

// 方向：上下左右
const int DIR[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

void dfs(vector<vector<char>>& grid, int i, int j, int m, int n) {
    if (i<0||i>=m||j<0||j>=n||grid[i][j]!='1') return;
    grid[i][j] = '0'; // 填掉
    for (auto& d : DIR) {
        dfs(grid, i+d[0], j+d[1], m, n);
    }
}

int numIslands(vector<vector<char>>& grid) {
    if (grid.empty()) return 0;
    int m=grid.size(), n=grid[0].size(), count=0;
    for (int i=0;i<m;i++) {
        for (int j=0;j<n;j++) {
            if (grid[i][j]=='1') {
                count++;
                dfs(grid, i, j, m, n);
            }
        }
    }
    return count;
}

int main() {
    // 测试
    vector<vector<char>> grid = {
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };
    cout << "Islands: " << numIslands(grid) << endl; // 3
    return 0;
}
