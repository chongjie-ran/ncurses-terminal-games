// LC10 正则表达式匹配
// 难度: Hard
// 分类: 字符串 + 动态规划
// 思路: dp[i][j] = s[0..i-1] 是否匹配 p[0..j-1]
// 关键: * 可以匹配0个或多个前面的字符
// 复杂度: O(mn) 时间, O(mn) 空间

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// 方法1: 2D DP
bool isMatch(string s, string p) {
    int m = s.size(), n = p.size();
    // dp[i][j]: s[0..i-1] 是否匹配 p[0..j-1]
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    
    // 空串匹配空模式
    dp[0][0] = 1;
    
    // 空串匹配模式: 处理 a* a*b* 等可生成空串的情况
    for (int j = 1; j <= n; j++) {
        if (p[j-1] == '*' && j >= 2) {
            dp[0][j] = dp[0][j-2]; // * 匹配0个字符
        }
    }
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (p[j-1] == '*') {
                // * 匹配0个: dp[i][j-2]
                // * 匹配>=1个: 需要 p[j-2] == s[i-1] 或 p[j-2] == '.'
                if (j >= 2) {
                    dp[i][j] = dp[i][j-2]; // 匹配0个
                    if (p[j-2] == s[i-1] || p[j-2] == '.') {
                        dp[i][j] = dp[i][j] || dp[i-1][j]; // 匹配1+个
                    }
                }
            } else if (p[j-1] == '.' || p[j-1] == s[i-1]) {
                dp[i][j] = dp[i-1][j-1];
            }
        }
    }
    
    return dp[m][n];
}

// 方法2: 递归 + 记忆化
bool isMatchRec(string& s, string& p, int i, int j, vector<vector<int>>& memo) {
    if (j == p.size()) return i == s.size();
    if (memo[i][j] != -1) return memo[i][j];
    
    bool first_match = (i < s.size() && (p[j] == s[i] || p[j] == '.'));
    bool ans;
    
    if (j + 1 < p.size() && p[j+1] == '*') {
        // * 组合: 匹配0个 或 匹配1个+继续
        ans = isMatchRec(s, p, i, j + 2, memo) || 
              (first_match && isMatchRec(s, p, i + 1, j, memo));
    } else {
        ans = first_match && isMatchRec(s, p, i + 1, j + 1, memo);
    }
    
    memo[i][j] = ans;
    return ans;
}

bool isMatch2(string s, string p) {
    vector<vector<int>> memo(s.size() + 1, vector<int>(p.size() + 1, -1));
    return isMatchRec(s, p, 0, 0, memo);
}

// 测试
int main() {
    struct TestCase {
        string s, p;
        bool expected;
    };
    
    vector<TestCase> tests = {
        {"aa", "a", false},
        {"aa", "a*", true},        // a* 匹配 aa
        {"ab", ".*", true},        // .* 匹配任意
        {"aab", "c*a*b", true},    // c* 匹配0个, a* 匹配 aab
        {"mississippi", "mis*is*p*.", false},
        {"", "a*", true},          // a* 匹配空串
        {"", "", true},
        {"a", ".", true},
        {"", ".*", true},
        {"ab", ".*c", false},
        {"aaa", "a*a", true},      // a* 匹配 aa, a 匹配 a
    };
    
    for (auto& tc : tests) {
        bool result = isMatch(tc.s, tc.p);
        bool result2 = isMatch2(tc.s, tc.p);
        cout << "s=\"" << tc.s << "\" p=\"" << tc.p << "\" => "
             << (result ? "true" : "false") 
             << " (expected: " << (tc.expected ? "true" : "false") << ")"
             << " [Rec2: " << (result2 ? "true" : "false") << "]"
             << (result == tc.expected && result2 == tc.expected ? " ✅" : " ❌")
             << endl;
    }
    
    return 0;
}
