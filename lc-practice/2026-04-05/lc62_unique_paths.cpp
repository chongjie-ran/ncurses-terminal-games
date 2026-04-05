// LC62 Unique Paths (Medium)
// DP: dp[i][j] = dp[i-1][j] + dp[i][j-1]
// Space optimized: O(n)
#include <iostream>
#include <vector>
using namespace std;

int uniquePaths(int m, int n) {
    vector<int> dp(n, 1);
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[j] += dp[j-1];
        }
    }
    return dp[n-1];
}

// Math approach: C(m+n-2, m-1) = (m+n-2)! / ((m-1)! * (n-1)!)
int uniquePathsMath(int m, int n) {
    long long res = 1;
    int N = m + n - 2;
    int r = min(m - 1, n - 1);
    for (int i = 1; i <= r; i++) {
        res = res * (N - r + i) / i;
    }
    return (int)res;
}

int main() {
    cout << "3x7: " << uniquePaths(3, 7) << " (expect 28)" << endl;
    cout << "3x2: " << uniquePaths(3, 2) << " (expect 3)" << endl;
    cout << "7x3: " << uniquePaths(7, 3) << " (expect 28)" << endl;
    cout << "3x3: " << uniquePaths(3, 3) << " (expect 6)" << endl;
    cout << "10x10: " << uniquePaths(10, 10) << " (expect 48620)" << endl;
    cout << "math(10,10): " << uniquePathsMath(10, 10) << " (expect 48620)" << endl;
    return 0;
}
