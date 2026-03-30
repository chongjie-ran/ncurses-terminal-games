#include <iostream>
#include <vector>
#include <string>
using namespace std;

int numDistinct(string s, string t) {
    int m = s.size(), n = t.size();
    if (n > m) return 0;
    vector<long long> dp(n + 1, 0);
    dp[0] = 1;
    for (int i = 1; i <= m; ++i) {
        for (int j = n; j >= 1; --j) {
            if (s[i-1] == t[j-1])
                dp[j] += dp[j-1];
        }
    }
    return (int)dp[n];
}

int main() {
    cout << numDistinct("rabbbit", "rabbit") << endl; // 3
    cout << numDistinct("babgbag", "bag") << endl;     // 5
    cout << numDistinct("", "a") << endl;              // 0
    cout << numDistinct("a", "a") << endl;             // 1
    return 0;
}
