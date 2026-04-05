// LC70 Climbing Stairs (Easy)
// DP: dp[i] = dp[i-1] + dp[i-2] (和Fibonacci一样)
// Space optimized: O(1)
#include <iostream>
using namespace std;

int climbStairs(int n) {
    if (n <= 2) return n;
    int prev2 = 1, prev1 = 2, cur = 2;
    for (int i = 3; i <= n; i++) {
        cur = prev1 + prev2;
        prev2 = prev1;
        prev1 = cur;
    }
    return cur;
}

// Recursive + memo
int helper(int n, int* memo) {
    if (n <= 2) return n;
    if (memo[n] != -1) return memo[n];
    memo[n] = helper(n-1, memo) + helper(n-2, memo);
    return memo[n];
}
int climbStairsMemo(int n) {
    int memo[46];
    for (int i = 0; i <= 45; i++) memo[i] = -1;
    return helper(n, memo);
}

int main() {
    cout << "stairs(1)=" << climbStairs(1) << " (expect 1)" << endl;
    cout << "stairs(2)=" << climbStairs(2) << " (expect 2)" << endl;
    cout << "stairs(3)=" << climbStairs(3) << " (expect 3)" << endl;
    cout << "stairs(4)=" << climbStairs(4) << " (expect 5)" << endl;
    cout << "stairs(5)=" << climbStairs(5) << " (expect 8)" << endl;
    cout << "stairs(10)=" << climbStairs(10) << " (expect 89)" << endl;
    cout << "stairs(44)=" << climbStairs(44) << " (expect 1134903170)" << endl;
    return 0;
}
