// LC509 Fibonacci Number (Easy)
// DP: dp[n] = dp[n-1] + dp[n-2]
// Space optimized: O(1)
#include <iostream>
using namespace std;

int fib(int n) {
    if (n <= 1) return n;
    int prev2 = 0, prev1 = 1, cur = 1;
    for (int i = 2; i <= n; i++) {
        cur = prev1 + prev2;
        prev2 = prev1;
        prev1 = cur;
    }
    return cur;
}

// Recursive with memoization
int fibMemo(int n, int* memo) {
    if (n <= 1) return n;
    if (memo[n] != -1) return memo[n];
    memo[n] = fibMemo(n-1, memo) + fibMemo(n-2, memo);
    return memo[n];
}

int main() {
    cout << "fib(0)=" << fib(0) << " (expect 0)" << endl;
    cout << "fib(1)=" << fib(1) << " (expect 1)" << endl;
    cout << "fib(5)=" << fib(5) << " (expect 5)" << endl;
    cout << "fib(10)=" << fib(10) << " (expect 55)" << endl;
    cout << "fib(20)=" << fib(20) << " (expect 6765)" << endl;
    
    // Memo version
    int memo[21];
    for (int i = 0; i <= 20; i++) memo[i] = -1;
    cout << "fibMemo(20)=" << fibMemo(20, memo) << " (expect 6765)" << endl;
    return 0;
}
