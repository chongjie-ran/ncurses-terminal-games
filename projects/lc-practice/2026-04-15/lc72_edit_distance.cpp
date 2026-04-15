// LC72 编辑距离 - 困难
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// DP: dp[i][j] = word1前i个变成word2前j个的最少操作数
int minDistance(string word1, string word2) {
    int m = word1.size(), n = word2.size();
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (word1[i-1] == word2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = min({dp[i-1][j-1], dp[i-1][j], dp[i][j-1]}) + 1;
            }
        }
    }
    return dp[m][n];
}

// 空间优化O(n)
int minDistance1D(string word1, string word2) {
    int m = word1.size(), n = word2.size();
    vector<int> dp(n+1, 0);
    for (int j = 0; j <= n; j++) dp[j] = j;
    
    for (int i = 1; i <= m; i++) {
        int prev = dp[0];
        dp[0] = i;
        for (int j = 1; j <= n; j++) {
            int temp = dp[j];
            dp[j] = (word1[i-1] == word2[j-1]) ? prev : min({prev, dp[j], dp[j-1]}) + 1;
            prev = temp;
        }
    }
    return dp[n];
}

int main() {
    cout << "horse->ros: " << minDistance("horse", "ros") << endl;
    cout << "horse->ros(1D): " << minDistance1D("horse", "ros") << endl;
    cout << "intention->execution: " << minDistance("intention", "execution") << endl;
    cout << "->a: " << minDistance("", "a") << endl;
    return 0;
}
