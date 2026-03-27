// LC44 Wildcard Matching
// Hard - DP with 2D table
// 2026-03-27 下午

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// dp[i][j] = s[0..i-1] matches p[0..j-1]
bool isMatch(string s, string p) {
    int m = s.size(), n = p.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    dp[0][0] = 1; // empty matches empty
    
    // Pattern like "a*", "a*b*" can match empty string
    for (int j = 1; j <= n; ++j) {
        if (p[j-1] == '*') {
            dp[0][j] = (j == 1) ? true : dp[0][j-2];
        }
    }
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j-1] == '*') {
                // * matches 0 chars: dp[i][j-2]
                // * matches 1+ chars: dp[i-1][j] (if p[j-2] matches s[i-1])
                dp[i][j] = dp[i][j-2];
                if (p[j-2] == '.' || p[j-2] == s[i-1]) {
                    dp[i][j] = dp[i][j] || dp[i-1][j];
                }
            } else if (p[j-1] == '.' || p[j-1] == s[i-1]) {
                dp[i][j] = dp[i-1][j-1];
            }
        }
    }
    return dp[m][n];
}

int main() {
    struct Test { string s, p; bool expected; };
    vector<Test> tests = {
        {"aa", "a", false},
        {"aa", "*", true},
        {"cb", "?a", false},
        {"adceb", "*a*b", true},
        {"acdcb", "a*c?b", false},
        {"", "*", true},
        {"", "?", false},
        {"a", "*?*", true},
    };
    
    int passed = 0;
    for (auto& t : tests) {
        bool result = isMatch(t.s, t.p);
        bool ok = (result == t.expected);
        cout << (ok ? "✅" : "❌") << " isMatch(\"" << t.s << "\", \"" << t.p << "\") = " 
             << (result ? "true" : "false") 
             << " (expected " << (t.expected ? "true" : "false") << ")\n";
        if (ok) ++passed;
    }
    cout << "\nResult: " << passed << "/" << tests.size() << " passed\n";
    return 0;
}
