// LC338 比特位计数 - DP: dp[i] = dp[i>>1] + (i&1)
#include <iostream>
#include <vector>
using namespace std;

vector<int> countBits(int n) {
    vector<int> dp(n+1, 0);
    for (int i = 1; i <= n; i++) dp[i] = dp[i>>1] + (i & 1);
    return dp;
}

int main() {
    auto r = countBits(5);
    for (int i = 0; i <= 5; i++) cout << i << "→" << r[i] << " ";
    cout << "\n0→0 1→1 2→1 3→2 4→1 5→2 ✓" << endl;
    return 0;
}
