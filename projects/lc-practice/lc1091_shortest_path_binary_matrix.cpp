// LC1091 Shortest Path in Binary Matrix - BFS
#include <vector>
#include <queue>
#include <climits>
#include <iostream>
using namespace std;

int shortestPathBinaryMatrix(vector<vector<int>>& grid) {
    int n = grid.size();
    if (grid[0][0] != 0 || grid[n-1][n-1] != 0) return -1;
    if (n == 1) return 1;
    
    const int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    vector<vector<int>> dist(n, vector<int>(n, INT_MAX));
    queue<pair<int,int>> q;
    
    dist[0][0] = 1;
    q.emplace(0,0);
    
    while (!q.empty()) {
        auto [x,y] = q.front(); q.pop();
        int d = dist[x][y];
        
        for (auto& dir : dirs) {
            int nx = x + dir[0], ny = y + dir[1];
            if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
            if (grid[nx][ny] != 0) continue;
            if (dist[nx][ny] <= d + 1) continue;
            dist[nx][ny] = d + 1;
            if (nx == n-1 && ny == n-1) return d + 1;
            q.emplace(nx, ny);
        }
    }
    return -1;
}

int main() {
    vector<vector<int>> g1 = {{0,1},{1,0}};
    cout << shortestPathBinaryMatrix(g1) << endl; // 2
    vector<vector<int>> g2 = {{0,0,0},{1,1,0},{1,1,0}};
    cout << shortestPathBinaryMatrix(g2) << endl; // 4
    return 0;
}
