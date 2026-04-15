#include <iostream>
#include <vector>
#include <string>
using namespace std;

// LC1143: Longest Common Subsequence
// dp[i][j] = LCS长度，text1[0..i-1] 和 text2[0..j-1]
// dp[i][j] = dp[i-1][j-1] + 1 (相等)
// dp[i][j] = max(dp[i-1][j], dp[i][j-1]) (不等)

class Solution {
public:
    int longestCommonSubsequence(string text1, string text2) {
        int m = text1.size(), n = text2.size();
        vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
        
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                if (text1[i-1] == text2[j-1]) {
                    dp[i][j] = dp[i-1][j-1] + 1;
                } else {
                    dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
                }
            }
        }
        return dp[m][n];
    }
};

// 空间优化版本: O(min(m,n))
int lcs_optimized(string text1, string text2) {
    if (text1.size() < text2.size()) swap(text1, text2);
    int m = text1.size(), n = text2.size();
    vector<int> dp(n+1, 0), prev(n+1, 0);
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int temp = dp[j];
            if (text1[i-1] == text2[j-1]) {
                dp[j] = prev[j-1] + 1;
            } else {
                dp[j] = max(dp[j], prev[j]);
            }
            prev[j] = temp;
        }
    }
    return dp[n];
}

int main() {
    Solution s;
    
    // Test 1: "abcde" vs "ace" → 3 ("ace")
    cout << s.longestCommonSubsequence("abcde", "ace") << endl;  // 3
    
    // Test 2: "abc" vs "abc" → 3
    cout << s.longestCommonSubsequence("abc", "abc") << endl;  // 3
    
    // Test 3: "abc" vs "def" → 0
    cout << s.longestCommonSubsequence("abc", "def") << endl;  // 0
    
    // Test 4: "abcba" vs "abcbcba" → 5
    cout << s.longestCommonSubsequence("abcba", "abcbcba") << endl;  // 5
    
    return 0;
}
