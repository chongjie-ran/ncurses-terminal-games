// LC70 Climbing Stairs
// DP: dp[i] = dp[i-1] + dp[i-2]
// 空间优化: 只保留前两个状态
#include <iostream>
#include <cassert>
using namespace std;

class Solution {
public:
    int climbStairs(int n) {
        if (n <= 2) return n;
        int prev2 = 1, prev1 = 2;
        for (int i = 3; i <= n; i++) {
            int curr = prev1 + prev2;
            prev2 = prev1;
            prev1 = curr;
        }
        return prev1;
    }
};

int main() {
    Solution s;
    assert(s.climbStairs(1) == 1);
    assert(s.climbStairs(2) == 2);
    assert(s.climbStairs(3) == 3);
    assert(s.climbStairs(4) == 5);
    assert(s.climbStairs(5) == 8);
    assert(s.climbStairs(6) == 13);
    assert(s.climbStairs(10) == 89);
    assert(s.climbStairs(20) == 10946); // Fibonacci(21)
    cout << "LC70: 8/8 tests passed ✅" << endl;
    return 0;
}
