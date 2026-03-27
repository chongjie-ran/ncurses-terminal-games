// LeetCode 10 - Regular Expression Matching (Hard)
// 动态规划：二维 dp
// dp[i][j] = s[0..i) 与 p[0..j) 是否匹配

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// ========== 方法1: 二维 DP ==========
bool isMatch_v1(const string& s, const string& p) {
    int m = s.size(), n = p.size();
    // dp[i][j]: s[0..i) vs p[0..j)
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    dp[0][0] = 1; // 空串匹配空模式

    // 初始化：空串与模式匹配（如 a*, a*b*, a*b*c* 等能匹配空串）
    for (int j = 1; j <= n; ++j) {
        if (p[j-1] == '*' && j >= 2) {
            dp[0][j] = dp[0][j-2]; // x* 消掉，依赖 dp[0][j-2]
        }
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j-1] == '*') {
                // case 1: * 匹配0次 → 消掉 x*，看 dp[i][j-2]
                dp[i][j] = dp[i][j-2];
                // case 2: * 匹配1+次 → 前一个字符匹配 s[i-1]，且 dp[i-1][j] 为真
                if (p[j-2] == '.' || p[j-2] == s[i-1]) {
                    dp[i][j] = dp[i][j] || dp[i-1][j];
                }
            } else if (p[j-1] == '.' || p[j-1] == s[i-1]) {
                // 直接匹配
                dp[i][j] = dp[i-1][j-1];
            }
        }
    }
    return dp[m][n];
}

// ========== 方法2: 二维 DP（标准版）==========
bool isMatch(const string& s, const string& p) {
    int m = s.size(), n = p.size();
    // dp[i][j]: s[0..i) vs p[0..j)
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    dp[0][0] = 1;

    // 初始化：空串与模式（如 a*, a*b*, .* 等能匹配空串）
    for (int j = 1; j <= n; ++j) {
        if (p[j-1] == '*' && j >= 2) dp[0][j] = dp[0][j-2];
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j-1] == '*') {
                // * 匹配0次：消掉 x*，继承 dp[i][j-2]
                dp[i][j] = dp[i][j-2];
                // * 匹配1+次：前驱 p[j-2] 能匹配 s[i-1]，且 dp[i-1][j] 为真
                // 注意：这里用 dp[i-1][j]（而非 dp[i-1][j-1]）是因为 a* 可以匹配任意个
                if (p[j-2] == '.' || p[j-2] == s[i-1]) {
                    dp[i][j] = dp[i][j] || dp[i-1][j];
                }
            } else if (p[j-1] == '.' || p[j-1] == s[i-1]) {
                dp[i][j] = dp[i-1][j-1];
            }
            // else: dp[i][j] 保持 0（不匹配）
        }
    }
    return dp[m][n];
}

// ========== 测试 ==========
int main() {
    struct Test { string s, p; bool expected; };
    Test tests[] = {
        {"aa",    "a",    false},
        {"aa",    "a*",   true},
        {"ab",    ".*",   true},
        {"aab",   "c*a*b",true},
        {"mississippi","mis*is*p*.",false},
        {"",      "",     true},
        {"",      "a*",   true},
        {"",      "a*b*c*",true},
        {"a",     ".",    true},
        {"a",     "a*",   true},
        {"ab",    ".*c",  false},
        {"aaa",   "a*a",  true},  // a* = aa, then a
        {"aaa",   "a*aa", true},  // a* = "a", then "aa" = "aa"
        {"a",     "ab*",  true},  // b* = 0次 b
    };

    int pass = 0;
    for (auto& t : tests) {
        bool result = isMatch(t.s, t.p);
        bool ok = (result == t.expected);
        cout << (ok ? "✅" : "❌") << " isMatch(\"" << t.s << "\", \"" << t.p << "\") = "
             << (result ? "true" : "false")
             << " (expected " << (t.expected ? "true" : "false") << ")\n";
        if (ok) ++pass;
    }
    cout << "\n结果: " << pass << "/" << (int)(sizeof(tests)/sizeof(tests[0])) << " 通过\n";
    return 0;
}
