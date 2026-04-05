/*
LC72 - Edit Distance
分类: 2D DP
难度: Hard
日期: 2026-04-05

思路:
  dp[i][j] = word1[0..i-1] → word2[0..j-1] 的最少操作数
  转移:
    if word1[i-1] == word2[j-1]: dp[i][j] = dp[i-1][j-1]
    else: dp[i][j] = 1 + min(dp[i-1][j],   // delete
                            dp[i][j-1],     // insert
                            dp[i-1][j-1])  // replace
  
  空间优化: 2行滚动数组

时间: O(m*n), 空间: O(min(m,n)) 优化后
*/

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
using namespace std;

// 原始二维DP
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
                dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
        }
    }
    return dp[m][n];
}

int main() {
    // Test 1: horse -> ros
    assert(minDistance("horse", "ros") == 3); cout << "Test1=3 ✅ horse→ros";
    
    // Test 2: intention -> execution
    assert(minDistance("intention", "execution") == 5); cout << ", Test2=5 ✅ intention→execution";
    
    // Test 3: empty
    assert(minDistance("", "abc") == 3); cout << ", Test3=3 ✅ empty→abc";
    
    // Test 4: same
    assert(minDistance("abc", "abc") == 0); cout << ", Test4=0 ✅ abc→abc";
    
    // Test 5: delete all
    assert(minDistance("abc", "") == 3); cout << ", Test5=3 ✅ abc→empty";
    
    cout << "\nAll tests PASS\n";
    return 0;
}
