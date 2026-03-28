// LC1631 - Minimum Effort Path
// 图论：Dijkstra 变体 + 二分查找
// 题目：grid 中从(0,0)到(m-1,n-1)，路径上相邻格差的绝对值的最大值最小化
// 思路1：二分答案 + BFS（检查是否可达）
// 思路2：Dijkstra 变体：dist[i][j] = 路径上最大差值的最小值，优先队列取 min
// 时间: O(m*n*log(m*n)) 空间: O(m*n)

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
using namespace std;

struct State {
    int effort;
    int x, y;
    bool operator<(State const& other) const {
        return effort > other.effort; // min-heap
    }
};

int minEffortPath(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<vector<int>> dist(m, vector<int>(n, INT_MAX));
    dist[0][0] = 0;
    priority_queue<State> pq;
    pq.push({0, 0, 0});
    
    const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    
    while (!pq.empty()) {
        State cur = pq.top(); pq.pop();
        if (cur.x == m-1 && cur.y == n-1) return cur.effort;
        if (cur.effort > dist[cur.x][cur.y]) continue;
        
        for (auto& d : dirs) {
            int nx = cur.x + d[0], ny = cur.y + d[1];
            if (nx < 0 || nx >= m || ny < 0 || ny >= n) continue;
            int nd = max(cur.effort, abs(grid[nx][ny] - grid[cur.x][cur.y]));
            if (nd < dist[nx][ny]) {
                dist[nx][ny] = nd;
                pq.push({nd, nx, ny});
            }
        }
    }
    return dist[m-1][n-1];
}

int main() {
    // Test 1
    vector<vector<int>> g1 = {
        {1,2,2},
        {3,8,2},
        {5,3,5}
    };
    cout << minEffortPath(g1) << endl; // 2
    
    // Test 2
    vector<vector<int>> g2 = {
        {1,2,3},
        {6,5,4},
        {9,8,7}
    };
    cout << minEffortPath(g2) << endl; // 4
    
    // Test 3
    vector<vector<int>> g3 = {
        {1,2,1,1,1},
        {1,2,1,2,1},
        {1,2,1,2,1},
        {1,2,1,2,1},
        {1,1,1,2,1}
    };
    cout << minEffortPath(g3) << endl; // 0
    
    return 0;
}
