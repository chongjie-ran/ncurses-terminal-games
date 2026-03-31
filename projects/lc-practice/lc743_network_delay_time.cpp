// LC743 - Network Delay Time
// 分类: Dijkstra 最短路
// 难度: Medium
// 思路: 从节点 k 出发，用 Dijkstra 求到所有节点的最短路径
// 如果有节点不可达，返回 -1；否则返回最大距离

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
using namespace std;

class Solution {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        vector<vector<pair<int,int>>> adj(n+1);
        for(auto& t: times) {
            adj[t[0]].push_back({t[1], t[2]}); // u->v, w
        }
        vector<int> dist(n+1, INT_MAX);
        dist[k] = 0;
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
        pq.push({0, k});
        while(!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if(d > dist[u]) continue;
            for(auto& [v, w]: adj[u]) {
                if(dist[v] > dist[u] + w) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                }
            }
        }
        int ans = 0;
        for(int i=1; i<=n; i++) {
            if(dist[i] == INT_MAX) return -1;
            ans = max(ans, dist[i]);
        }
        return ans;
    }
};
