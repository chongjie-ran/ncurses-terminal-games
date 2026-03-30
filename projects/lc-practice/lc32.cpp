// LC32 Longest Valid Parentheses - Hard
// 栈+dp[i]表示以i结尾的最长有效括号
// dp[i] = if s[i]==')':
//   if s[i-1]=='(': dp[i]=dp[i-2]+2
//   if s[i-1]==')': dp[i]=dp[i-1]+2+dp[i-dp[i-1]-2]
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    int longestValidParentheses(string s) {
        int n = s.size();
        if (n == 0) return 0;
        vector<int> dp(n, 0);
        int ans = 0;
        for (int i = 1; i < n; i++) {
            if (s[i] == ')') {
                if (s[i-1] == '(') {
                    dp[i] = (i >= 2 ? dp[i-2] : 0) + 2;
                } else {
                    int prev = dp[i-1];
                    int j = i - prev - 1;
                    if (j >= 0 && s[j] == '(') {
                        dp[i] = prev + 2 + (j >= 1 ? dp[j-1] : 0);
                    }
                }
                ans = max(ans, dp[i]);
            }
        }
        return ans;
    }
};
