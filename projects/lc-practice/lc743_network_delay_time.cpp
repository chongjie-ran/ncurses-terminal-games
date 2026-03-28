// LC743 Network Delay Time
// 图论：Dijkstra 单源最短路
// 题目：所有节点收到信号的最短时间？从 K 出发

#include <iostream>
#include <vector>
#include <queue>
#include <limits.h>
#include <utility>
using namespace std;

// Solution: Dijkstra with priority queue
// Time: O(E log V), Space: O(E+V)
// Test: times = [[2,1,1],[2,3,1],[3,4,1]], n=4, k=2 -> 2

int networkDelayTime(vector<vector<int>>& times, int n, int k) {
    vector<vector<pair<int,int>>> adj(n+1);
    for (auto& t : times) {
        adj[t[0]].push_back({t[1], t[2]});
    }
    
    vector<int> dist(n+1, INT_MAX);
    dist[k] = 0;
    using P = pair<int,int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    pq.push({0, k});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (auto [v, w] : adj[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    
    int ans = 0;
    for (int i = 1; i <= n; ++i) {
        if (dist[i] == INT_MAX) return -1;
        ans = max(ans, dist[i]);
    }
    return ans;
}

int main() {
    vector<vector<int>> times1 = {{2,1,1},{2,3,1},{3,4,1}};
    int r1 = networkDelayTime(times1, 4, 2);
    cout << "Test1: " << r1 << " (expected 2) -> " << (r1==2?"✅":"❌") << endl;
    
    vector<vector<int>> times2 = {{1,2,1}};
    int r2 = networkDelayTime(times2, 2, 1);
    cout << "Test2: " << r2 << " (expected 1) -> " << (r2==1?"✅":"❌") << endl;
    
    vector<vector<int>> times3 = {{1,2,1}};
    int r3 = networkDelayTime(times3, 3, 2);
    cout << "Test3: " << r3 << " (expected -1) -> " << (r3==-1?"✅":"❌") << endl;
    
    return 0;
}
