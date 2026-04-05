// LC746 Min Cost Climbing Stairs (Easy)
// DP: dp[i] = min(dp[i-1] + cost[i-1], dp[i-2] + cost[i-2])
#include <iostream>
#include <vector>
using namespace std;

int minCostClimbingStairs(vector<int>& cost) {
    int n = cost.size();
    if (n <= 1) return 0;
    int prev2 = 0, prev1 = 0;
    for (int i = 2; i <= n; i++) {
        int cur = min(prev1 + cost[i-1], prev2 + cost[i-2]);
        prev2 = prev1;
        prev1 = cur;
    }
    return prev1;
}

int main() {
    vector<int> c1 = {10,15,20};
    cout << "[10,15,20]: " << minCostClimbingStairs(c1) << " (expect 15)" << endl;
    
    vector<int> c2 = {1,100,1,1,1,100,1,1,100,1};
    cout << "[1,100,1,1,1,100,1,1,100,1]: " << minCostClimbingStairs(c2) << " (expect 6)" << endl;
    
    vector<int> c3 = {0,0,0,0};
    cout << "[0,0,0,0]: " << minCostClimbingStairs(c3) << " (expect 0)" << endl;
    return 0;
}
