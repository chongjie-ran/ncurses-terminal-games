// LC407 Trapping Rain Water II
// Hard - 2D Rain Water Trapping
// Approach: Min-Heap BFS (like Dijkstra)
// Key insight: Start from boundary, flood inward. The water at any cell
// is trapped by the MINIMUM boundary height that surrounds it.
// Water at (r,c) = max(0, minBoundary - heightMap[r][c])
//
// Time: O(m*n*log(m*n)) - each cell pushed once into heap
// Space: O(m*n) for visited + heap

#include <vector>
#include <queue>
using namespace std;

struct Cell {
    int h;
    int r;
    int c;
    bool operator<(const Cell& other) const {
        return h > other.h; // min-heap via reverse comparison
    }
};

int trapRainWater(vector<vector<int>>& heightMap) {
    if (heightMap.empty() || heightMap[0].empty()) return 0;
    
    int m = heightMap.size();
    int n = heightMap[0].size();
    if (m <= 2 || n <= 2) return 0;
    
    vector<vector<int>> visited(m, vector<int>(n, 0));
    priority_queue<Cell> pq;
    
    // Add all boundary cells
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (i == 0 || i == m-1 || j == 0 || j == n-1) {
                visited[i][j] = 1;
                pq.push({heightMap[i][j], i, j});
            }
        }
    }
    
    int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    int water = 0;
    
    while (!pq.empty()) {
        Cell cur = pq.top(); pq.pop();
        
        for (auto& d : dirs) {
            int nr = cur.r + d[0];
            int nc = cur.c + d[1];
            if (nr < 0 || nr >= m || nc < 0 || nc >= n || visited[nr][nc]) continue;
            
            visited[nr][nc] = 1;
            int nh = heightMap[nr][nc];
            // Water can be trapped if boundary min is higher than this cell
            water += max(0, cur.h - nh);
            // Push with max of current height and this cell's height
            // (flood level rises to the taller of boundary/cell)
            pq.push({max(cur.h, nh), nr, nc});
        }
    }
    
    return water;
}

// Test - Use LC official example
int main() {
    // LC407 official example
    vector<vector<int>> h1 = {
        {1,4,3,1,3,2},
        {3,2,1,3,2,4},
        {2,3,3,2,3,1}
    };
    printf("Test1: %d (expected 4)\n", trapRainWater(h1)); // 4
    
    // Another test - basin where boundary=4, inner=1
    vector<vector<int>> h2 = {
        {5,5,5,5,5},
        {5,1,1,1,5},
        {5,1,1,1,5},
        {5,5,5,5,5}
    };
    // Inner 4 cells at height 1, boundary min=5, water per cell=4, total=16
    printf("Test2: %d (expected 16)\n", trapRainWater(h2)); // 16
    
    // Simple 3x3 with 0 trapped
    vector<vector<int>> h3 = {
        {3,3,3},
        {3,2,3},
        {3,3,3}
    };
    // Inner cell at height 2, boundary min=3, water=1
    printf("Test3: %d (expected 1)\n", trapRainWater(h3)); // 1
    
    return 0;
}
