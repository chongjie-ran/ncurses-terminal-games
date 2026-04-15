// LC516 - 最长回文子序列 (Longest Palindromic Subsequence)
// 难度: Medium | 分类: 字符串/DP
// 2026-04-15 悟通

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// dp[i][j]: s[i..j]的最长回文子序列长度
// 转移: 相等 → dp[i+1][j-1]+2; 不等 → max(dp[i+1][j], dp[i][j-1])
// 初始化: dp[i][i]=1

int longestPalindromicSubseq(string s) {
    int n = s.size();
    vector<vector<int>> dp(n, vector<int>(n, 0));
    
    // 初始化：长度为1的子串
    for (int i = 0; i < n; i++) dp[i][i] = 1;
    
    // 长度从2到n
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i + len - 1 < n; i++) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                dp[i][j] = dp[i+1][j-1] + 2;
            } else {
                dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
            }
        }
    }
    return dp[0][n-1];
}

// 空间优化版: 空间O(n)
int longestPalindromicSubseq_opt(string s) {
    int n = s.size();
    vector<int> dp(n, 1), new_dp(n, 0);
    
    for (int i = n-1; i >= 0; i--) {
        new_dp[i] = 1;  // dp[i][i] = 1
        for (int j = i+1; j < n; j++) {
            if (s[i] == s[j]) {
                new_dp[j] = dp[j-1] + 2;
            } else {
                new_dp[j] = max(dp[j], new_dp[j-1]);
            }
        }
        dp = new_dp;
    }
    return dp[n-1];
}

// 获取最长回文子序列
string getLPS(string s) {
    int n = s.size();
    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) dp[i][i] = 1;
    
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i + len - 1 < n; i++) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                dp[i][j] = dp[i+1][j-1] + 2;
            } else {
                dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
            }
        }
    }
    
    // 回溯
    string lps;
    int i = 0, j = n - 1;
    while (i <= j) {
        if (i == j) {
            lps = s[i] + lps;
            break;
        }
        if (s[i] == s[j]) {
            lps = s[i] + lps;
            lps += s[j];
            i++; j--;
        } else if (dp[i+1][j] >= dp[i][j-1]) {
            i++;
        } else {
            j--;
        }
    }
    return lps;
}

int main() {
    struct TestCase { string s; int expected; };
    
    vector<TestCase> tests = {
        {"bbbab", 4},           // bbb -> "bbbb"
        {"cbbd", 2},            // bb
        {"a", 1},
        {"abc", 1},             // a or b or c
        {"abba", 4},            // abba
        {"aabbaa", 6},          // aabbaa
        {"racecar", 7},         // racecar
        {"bgbg", 3},            // bgb or gbg
    };
    
    cout << "=== LC516 最长回文子序列 ===" << endl;
    for (int idx = 0; idx < tests.size(); idx++) {
        auto& tc = tests[idx];
        int result = longestPalindromicSubseq(tc.s);
        int result_opt = longestPalindromicSubseq_opt(tc.s);
        bool pass = (result == tc.expected && result_opt == tc.expected);
        
        cout << "Test " << idx+1 << ": \"" << tc.s << "\""
             << " → " << result << " (expected " << tc.expected << ") "
             << (pass ? "✅" : "❌") << endl;
        if (result > 0) {
            cout << "  LPS: \"" << getLPS(tc.s) << "\"" << endl;
        }
    }
    return 0;
}
