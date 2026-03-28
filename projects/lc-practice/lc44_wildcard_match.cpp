// LC44 Wildcard Matching - Hard
// '?' matches any single char, '*' matches any sequence (including empty)
// 解法1: 2D DP  | 解法2: 贪心双指针
#include <iostream>
#include <vector>
#include <string>
using namespace std;

bool isMatchDP(const string& s, const string& p) {
    int m = s.size(), n = p.size();
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    dp[0][0] = 1;
    for (int j = 1; j <= n; ++j)
        if (p[j-1] == '*') dp[0][j] = dp[0][j-1];
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j-1] == '*')
                dp[i][j] = dp[i][j-1] || dp[i-1][j];
            else if (p[j-1] == '?' || p[j-1] == s[i-1])
                dp[i][j] = dp[i-1][j-1];
        }
    }
    return dp[m][n];
}

// 贪心: 记录最后一个 '*' 的位置，匹配失败时回退
bool isMatchGreedy(const string& s, const string& p) {
    int m = s.size(), n = p.size();
    int i = 0, j = 0;
    int star_i = -1, star_j = -1;
    while (i < m) {
        if (j < n && (p[j] == '?' || p[j] == s[i])) {
            ++i; ++j;
        } else if (j < n && p[j] == '*') {
            star_i = i; star_j = j;
            ++j;
        } else if (star_j != -1) {
            j = star_j + 1;
            ++star_i;
            i = star_i;
        } else {
            return false;
        }
    }
    while (j < n && p[j] == '*') ++j;
    return j == n;
}

int main() {
    struct Test { string s, p; bool exp; };
    vector<Test> tests = {
        {"aa","a",false},
        {"aa","*",true},
        {"cb","?a",false},
        {"adceb","*a*b",true},
        {"acdcb","a*c?b",false},
        {"","*",true},
        {"", "", true},
        {"aab","c*a*b",false},
        {"mississippi","m*si*ppi",false},
    };
    int pass = 0;
    for (auto& t : tests) {
        bool r1 = isMatchDP(t.s, t.p);
        bool r2 = isMatchGreedy(t.s, t.p);
        bool ok = (r1 == t.exp && r2 == t.exp);
        if (ok) ++pass;
        cout << "s=\"" << t.s << "\" p=\"" << t.p << "\" exp=" << t.exp 
             << " dp=" << r1 << " greedy=" << r2 << " " << (ok ? "✅" : "❌") << "\n";
    }
    cout << "\n" << pass << "/" << tests.size() << " passed\n";
    return 0;
}
