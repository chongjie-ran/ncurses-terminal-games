#include <iostream>
#include <vector>
#include <string>
using namespace std;

int strangePrinter(const string& s) {
    string t;
    for (char c : s) {
        if (t.empty() || t.back() != c) t.push_back(c);
    }
    int n = t.size();
    if (n == 0) return 0;
    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int i = n-1; i >= 0; i--) {
        dp[i][i] = 1;
        for (int j = i+1; j < n; j++) {
            dp[i][j] = dp[i+1][j] + 1;
            for (int k = i+1; k < j; k++) {
                if (t[i] == t[k]) {
                    dp[i][j] = min(dp[i][j], dp[i+1][k-1] + dp[k+1][j]);
                }
            }
            if (t[i] == t[j]) {
                dp[i][j] = min(dp[i][j], dp[i+1][j-1] + 1);
            }
        }
    }
    return dp[0][n-1];
}

int main() {
    vector<pair<string,int>> tests = {
        {"abc",3},{"aaabbb",2},{"aba",2},{"aaa",1},
        {"abcd",4},{"abaabcaba",4}
    };
    for (auto& [s,expected] : tests) {
        int result = strangePrinter(s);
        cout << s << " -> " << result << " (expected " << expected << ") " << (result==expected?"✓":"✗") << endl;
    }
    return 0;
}
