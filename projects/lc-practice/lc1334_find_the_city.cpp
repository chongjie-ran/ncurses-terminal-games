// LC1334 - Find the City With the Smallest Number of Neighbors at a Threshold Distance
// Medium: Floyd-Warshall All-Pairs Shortest Paths
// 思路: 先 Floyd-Warshall 求所有点对最短路，再统计每个点可达且距离<=threshold的邻居数
// 时间: O(n³) 空间: O(n²)

#include <iostream>
#include <vector>
using namespace std;

int findTheCity(int n, vector<vector<int>>& edges, int distanceThreshold) {
    const int INF = 1e9;
    vector<vector<int>> dist(n, vector<int>(n, INF));
    for (int i = 0; i < n; ++i) dist[i][i] = 0;
    for (auto& e : edges) {
        dist[e[0]][e[1]] = min(dist[e[0]][e[1]], e[2]);
        dist[e[1]][e[0]] = min(dist[e[1]][e[0]], e[2]);
    }
    
    // Floyd-Warshall
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (dist[i][j] > dist[i][k] + dist[k][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
    
    int ans = -1, best = n;
    for (int i = 0; i < n; ++i) {
        int cnt = 0;
        for (int j = 0; j < n; ++j)
            if (i != j && dist[i][j] <= distanceThreshold) ++cnt;
        if (cnt <= best) {
            best = cnt;
            ans = i;
        }
    }
    return ans;
}

int main() {
    // Test1: n=4, edges=[[0,1,3],[1,2,1],[1,3,4],[2,3,1]], threshold=4
    vector<vector<int>> e1 = {{0,1,3},{1,2,1},{1,3,4},{2,3,1}};
    cout << "Test1: " << findTheCity(4, e1, 4) << " (expected: 3)" << endl;
    
    // Test2: n=5, edges=[[0,1,2],[0,4,8],[1,2,3],[2,3,1],[1,4,2],[3,4,1]], threshold=GOOG
    vector<vector<int>> e2 = {{0,1,2},{0,4,8},{1,2,3},{2,3,1},{1,4,2},{3,4,1}};
    cout << "Test2: " << findTheCity(5, e2, 2) << " (expected: 0)" << endl;

    return 0;
}
