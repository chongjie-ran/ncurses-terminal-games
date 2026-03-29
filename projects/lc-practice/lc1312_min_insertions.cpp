// LC1312: Minimum Insertion Steps to Make a String Palindrome
// Hard: DP — 转换为"最长回文子序列"问题
// 思路: 先求 LCS(s, reverse(s)) = 最长回文子序列长度
//       答案 = n - lcs_len
// 时间: O(n²), 空间: O(n²) 可优化到 O(n)

#include <vector>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;

// 二维 DP 版本
int minInsertions(const string& s) {
    int n = s.size();
    string rs(s.rbegin(), s.rend());
    vector<vector<int>> dp(n+1, vector<int>(n+1, 0));
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s[i-1] == rs[j-1]) dp[i][j] = dp[i-1][j-1] + 1;
            else dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
        }
    }
    int lcs = dp[n][n];
    return n - lcs;
}

// 空间优化版 O(n)
int minInsertionsOpt(const string& s) {
    int n = s.size();
    string rs(s.rbegin(), s.rend());
    vector<int> prev(n+1, 0), curr(n+1, 0);
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s[i-1] == rs[j-1]) curr[j] = prev[j-1] + 1;
            else curr[j] = max(prev[j], curr[j-1]);
        }
        swap(prev, curr);
    }
    int lcs = prev[n];
    return n - lcs;
}

int main() {
    cout << minInsertions("zzazz") << endl;     // 0 (已是回文)
    cout << minInsertions("mbadm") << endl;     // 2 (mbadbm 或)
    cout << minInsertions("leetcode") << endl;   // 5
    cout << minInsertions("abc") << endl;        // 2 (bca 或 acb)
    cout << minInsertions("a") << endl;         // 0
    cout << minInsertions("") << endl;          // 0
    return 0;
}
