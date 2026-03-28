// LC5 最长回文子串
// 难度: Medium
// 分类: 字符串 + 动态规划
// 思路1: 中心扩展 - 对每个位置向两边扩展 O(n²)
// 思路2: DP - dp[i][j] = s[i]==s[j] && (j-i<3 || dp[i+1][j-1])
// 复杂度: O(n²) 时间, O(n²) 空间
// 技巧: Manacher 算法 O(n) 但实现复杂

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// 方法1: 中心扩展法 (推荐 - 简单高效)
string longestPalindrome(string s) {
    if (s.size() < 2) return s;
    
    int n = s.size();
    int start = 0, maxLen = 1;
    
    for (int center = 0; center < n; center++) {
        // 奇数长度: 以 center 为中心
        int l = center, r = center;
        while (l >= 0 && r < n && s[l] == s[r]) {
            if (r - l + 1 > maxLen) {
                start = l;
                maxLen = r - l + 1;
            }
            l--; r++;
        }
        
        // 偶数长度: 以 center, center+1 为中心
        l = center; r = center + 1;
        while (l >= 0 && r < n && s[l] == s[r]) {
            if (r - l + 1 > maxLen) {
                start = l;
                maxLen = r - l + 1;
            }
            l--; r++;
        }
    }
    
    return s.substr(start, maxLen);
}

// 方法2: 动态规划
string longestPalindromeDP(string s) {
    int n = s.size();
    if (n < 2) return s;
    
    vector<vector<bool>> dp(n, vector<bool>(n, false));
    int start = 0, maxLen = 1;
    
    // 所有单字符是回文
    for (int i = 0; i < n; i++) dp[i][i] = true;
    
    // 按长度递增
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i + len - 1 < n; i++) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                if (len == 2) {
                    dp[i][j] = true;
                } else {
                    dp[i][j] = dp[i+1][j-1];
                }
                if (dp[i][j] && len > maxLen) {
                    start = i;
                    maxLen = len;
                }
            }
        }
    }
    
    return s.substr(start, maxLen);
}

// 测试
int main() {
    vector<pair<string, string>> tests = {
        {"babad", "bab"},   // bab 或 aba 均可
        {"cbbd", "bb"},
        {"a", "a"},
        {"ac", "a"},        // a 或 c
        {"racecar", "racecar"},
        {"aaaa", "aaaa"},
        {"abcba", "abcba"},
    };
    
    for (auto& [s, expected] : tests) {
        string r1 = longestPalindrome(s);
        string r2 = longestPalindromeDP(s);
        cout << "s=\"" << s << "\" => \"" << r1 << "\" (DP: \"" << r2 << "\")" << endl;
    }
    
    return 0;
}
