// LC200: Number of Islands
#include <vector>
#include <iostream>
using namespace std;

void dfs(vector<vector<char>>& grid, int i, int j, int m, int n) {
    if (i<0||i>=m||j<0||j>=n||grid[i][j]=='0') return;
    grid[i][j] = '0';
    dfs(grid, i+1, j, m, n);
    dfs(grid, i-1, j, m, n);
    dfs(grid, i, j+1, m, n);
    dfs(grid, i, j-1, m, n);
}

int numIslands(vector<vector<char>>& grid) {
    int m=grid.size(), n=grid[0].size(), count=0;
    for (int i=0;i<m;i++)
        for (int j=0;j<n;j++)
            if (grid[i][j]=='1') {
                count++;
                dfs(grid,i,j,m,n);
            }
    return count;
}

int main() {
    vector<vector<char>> g1 = {{'1','1','1','1','0'},{'1','1','0','1','0'},{'1','1','0','0','0'},{'0','0','0','0','0'}};
    cout << (numIslands(g1)==1?"PASS":"FAIL") << " ";
    
    vector<vector<char>> g2 = {{'1','1','0','0','0'},{'1','1','0','0','0'},{'0','0','1','0','0'},{'0','0','0','1','1'}};
    cout << (numIslands(g2)==3?"PASS":"FAIL") << " ";
    
    vector<vector<char>> g3 = {{'1'}};
    cout << (numIslands(g3)==1?"PASS":"FAIL") << " ";
    
    vector<vector<char>> g4 = {{'0'}};
    cout << (numIslands(g4)==0?"PASS":"FAIL") << endl;
    return 0;
}
