// LC115. Distinct Subsequences
// Hard — 字符串 DP
// 题目: 给定字符串 s 和 t，计算 s 中子序列等于 t 的个数
// 核心: dp[i][j] = s[0..i-1] 中 t[0..j-1] 出现的次数
// 递推: s[i-1] == t[j-1] ? dp[i-1][j-1] : 0
//       dp[i][j] += dp[i-1][j] (不选s[i-1]的情况)
#include <bits/stdc++.h>
using namespace std;

// 2D DP 解法: O(mn) 时间, O(mn) 空间
int numDistinct2D(const string& s, const string& t) {
    int m = s.size(), n = t.size();
    if (n > m) return 0;
    vector<vector<unsigned int>> dp(m + 1, vector<unsigned int>(n + 1, 0));
    // t 为空，s 的任何前缀都有 1 个子序列（空串）
    for (int i = 0; i <= m; ++i) dp[i][0] = 1;
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            dp[i][j] = dp[i - 1][j]; // 不选 s[i-1]
            if (s[i - 1] == t[j - 1])
                dp[i][j] += dp[i - 1][j - 1];
        }
    }
    return dp[m][n];
}

// 空间优化: O(n) 空间
int numDistinct(const string& s, const string& t) {
    int m = s.size(), n = t.size();
    if (n > m) return 0;
    vector<unsigned int> dp(n + 1, 0);
    dp[0] = 1;
    for (int i = 1; i <= m; ++i) {
        // 必须从后向前遍历，否则 dp[j-1] 会被本轮更新污染
        for (int j = n; j >= 1; --j) {
            if (s[i - 1] == t[j - 1])
                dp[j] += dp[j - 1];
        }
    }
    return dp[n];
}

// 测试
int main() {
    struct Test { string s, t; int expected; };
    vector<Test> tests = {
        {"rabbbit", "rabbit", 3},
        {"babgbag", "bag", 5},
        {"", "a", 0},
        {"a", "a", 1},
        {"a", "b", 0},
        {"abc", "abc", 1},
        {"abc", "ac", 1},
        {"ssss", "s", 4},
    };
    
    for (auto& tc : tests) {
        int result = numDistinct(tc.s, tc.t);
        cout << "s=\"" << tc.s << "\" t=\"" << tc.t << "\" => " 
             << result << " (expected " << tc.expected << ") "
             << (result == tc.expected ? "✅" : "❌") << endl;
    }
    return 0;
}
