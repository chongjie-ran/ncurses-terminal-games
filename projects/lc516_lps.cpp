// LeetCode 516 - Longest Palindromic Subsequence (Medium-Hard)
// 动态规划：2D dp[i][j] = s[i..j] 的最长回文子序列长度
// 状态转移：
//   s[i] == s[j] → dp[i][j] = dp[i+1][j-1] + 2
//   s[i] != s[j] → dp[i][j] = max(dp[i+1][j], dp[i][j-1])

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// ========== 方法1: 2D DP（标准版）==========
int longestPalindromeSubseq_v1(const string& s) {
    int n = s.size();
    if (n == 0) return 0;
    vector<vector<int>> dp(n, vector<int>(n, 0));
    // 单字符是回文，长度为 1
    for (int i = 0; i < n; ++i) dp[i][i] = 1;
    // 按长度递增：len=2..n
    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                dp[i][j] = (len == 2) ? 2 : dp[i+1][j-1] + 2;
            } else {
                dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
            }
        }
    }
    return dp[0][n-1];
}

// ========== 方法2: LCS 视角（空间可优化到 O(n)）==========
// 将 s 与 reverse(s) 做 LCS，即为 s 的 LPS
int longestPalindromeSubseq_lcs(const string& s) {
    string rs(s.rbegin(), s.rend());
    int n = s.size();
    vector<int> prev(n+1, 0), cur(n+1, 0);
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s[i-1] == rs[j-1]) {
                cur[j] = prev[j-1] + 1;
            } else {
                cur[j] = max(prev[j], cur[j-1]);
            }
        }
        swap(prev, cur);
    }
    return prev[n];
}

// ========== 测试 ==========
int main() {
    struct Test { string s; int expected; };
    vector<Test> tests = {
        {"bbbab", 4},        // "bbbb"
        {"cbbd", 2},         // "bb"
        {"a", 1},
        {"abc", 1},          // "a"/"b"/"c"
        {"abcba", 5},        // 整个字符串
        {"aacdefcaa", 7},    // "aacdc aa" = 7
        {"", 0},
        {"aaa", 3},
        {"racecar", 7},      // "racecar"
        {"abcabcabc", 5},    // "abcba" 或 "abcabca"
    };
    
    cout << "========== 方法1: 2D DP ==========\n";
    for (auto& t : tests) {
        int result = longestPalindromeSubseq_v1(t.s);
        cout << (result == t.expected ? "✅" : "❌") 
             << " \"" << t.s << "\" => " << result 
             << " (expected " << t.expected << ")\n";
    }
    
    cout << "\n========== 方法2: LCS 视角 ==========\n";
    for (auto& t : tests) {
        int result = longestPalindromeSubseq_lcs(t.s);
        cout << (result == t.expected ? "✅" : "❌") 
             << " \"" << t.s << "\" => " << result 
             << " (expected " << t.expected << ")\n";
    }
    
    return 0;
}
