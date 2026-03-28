// LC787 Cheapest Flights Within K Stops
// 图论：有限次松弛（Bellman-Ford 变体）
// 题目：src 到 dst，最多停 K 站的最便宜价格

#include <iostream>
#include <vector>
#include <limits.h>
using namespace std;

// Solution: Bellman-Ford with K+1 iteration limit
// Time: O(K * E), Space: O(V)
// Key insight: 第 i 次迭代后的 dp[v] = 经过至多 i 条边到达 v 的最低价格

int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int K) {
    const int INF = INT_MAX / 2;
    vector<int> dist(n, INF);
    dist[src] = 0;
    
    for (int i = 0; i <= K; ++i) {
        vector<int> next = dist;
        for (auto& f : flights) {
            int u = f[0], v = f[1], price = f[2];
            if (dist[u] != INF && dist[u] + price < next[v]) {
                next[v] = dist[u] + price;
            }
        }
        dist.swap(next);
    }
    
    return dist[dst] == INF ? -1 : dist[dst];
}

int main() {
    // Test 1
    vector<vector<int>> flights1 = {{0,1,100},{1,2,100},{2,0,100},{1,3,600},{2,3,200}};
    int r1 = findCheapestPrice(4, flights1, 0, 3, 1);
    cout << "Test1: " << r1 << " (expected 700) -> " << (r1==700?"✅":"❌") << endl;
    
    // Test 2: 0->3 direct = 100 (0 stop)
    vector<vector<int>> flights2 = {{0,1,100},{0,2,500},{0,3,100},{1,2,100},{2,1,100},{2,3,100},{1,3,200}};
    int r2 = findCheapestPrice(4, flights2, 0, 3, 1);
    cout << "Test2: " << r2 << " (expected 100) -> " << (r2==100?"✅":"❌") << endl;
    
    // Test 3: no route within K stops
    vector<vector<int>> flights3 = {{0,1,100},{2,3,100}};
    int r3 = findCheapestPrice(4, flights3, 0, 3, 1);
    cout << "Test3: " << r3 << " (expected -1) -> " << (r3==-1?"✅":"❌") << endl;
    
    // Test 4: K=0 (direct flight only)
    vector<vector<int>> flights4 = {{0,1,100},{1,2,100},{2,3,100}};
    int r4 = findCheapestPrice(4, flights4, 0, 3, 0);
    cout << "Test4: " << r4 << " (expected -1) -> " << (r4==-1?"✅":"❌") << endl;
    
    // Test 5: LeetCode example 1 (from description)
    vector<vector<int>> flights5 = {{0,1,100},{1,2,100},{0,2,500},{0,3,100},{2,1,100},{2,3,100},{1,3,200}};
    int r5 = findCheapestPrice(4, flights5, 0, 3, 1);
    cout << "Test5 (LeetCode example): " << r5 << " (expected 100) -> " << (r5==100?"✅":"❌") << endl;
    
    return 0;
}
