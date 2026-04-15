// LC1143: Longest Common Subsequence
// 核心: dp[i][j] = max(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]+1 if equal)
// 滚动数组优化 O(n)空间
#include <iostream>
#include <vector>
#include <string>
using namespace std;

int longestCommonSubsequence(string text1, string text2) {
    int m = text1.size(), n = text2.size();
    vector<int> dp(n + 1, 0);
    vector<int> prev(n + 1, 0);
    
    for (int i = 1; i <= m; i++) {
        swap(dp, prev);
        for (int j = 1; j <= n; j++) {
            if (text1[i-1] == text2[j-1]) {
                dp[j] = prev[j-1] + 1;
            } else {
                dp[j] = max(prev[j], dp[j-1]);
            }
        }
    }
    return dp[n];
}

int main() {
    cout << "LC1143 Longest Common Subsequence" << endl;
    cout << "abcde vs ace: " << longestCommonSubsequence("abcde", "ace") << endl; // 3
    cout << "abc vs abc: " << longestCommonSubsequence("abc", "abc") << endl;     // 3
    cout << "abc vs def: " << longestCommonSubsequence("abc", "def") << endl;     // 0
    return 0;
}
