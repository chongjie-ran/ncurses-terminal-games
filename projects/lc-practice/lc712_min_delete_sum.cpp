#include <iostream>
#include <vector>
#include <string>
using namespace std;

// LC712: Minimum ASCII Delete Sum for Two Strings
// 相当于编辑距离的变体：删除字符的ASCII码和最小
// dp[i][j] = text1[0..i-1]和text2[0..j-1]的最小删除和
// 相等: dp[i][j] = dp[i-1][j-1]
// 不等: dp[i][j] = min(dp[i-1][j] + text1[i-1], dp[i][j-1] + text2[j-1])

int minimumDeleteSum(string s1, string s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    
    // 初始化第一行/列：只能全部删除
    for (int j = 1; j <= n; j++) dp[0][j] = dp[0][j-1] + s2[j-1];
    for (int i = 1; i <= m; i++) dp[i][0] = dp[i-1][0] + s1[i-1];
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = min(
                    dp[i-1][j] + s1[i-1],  // 删除s1[i-1]
                    dp[i][j-1] + s2[j-1]   // 删除s2[j-1]
                );
            }
        }
    }
    return dp[m][n];
}

int main() {
    // Test 1: "sea" vs "eat" → 231 (删除's'和't': 115+116=231)
    cout << minimumDeleteSum("sea", "eat") << endl;  // 231
    
    // Test 2: "delete" vs "leet" → 403
    cout << minimumDeleteSum("delete", "leet") << endl;  // 403
    
    return 0;
}
