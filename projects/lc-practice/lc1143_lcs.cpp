// LC1143 Longest Common Subsequence
// Medium | DP + prev variable
#include <iostream>
#include <vector>
#include <string>
using namespace std;

int longestCommonSubseq(string text1, string text2) {
    int m = text1.size(), n = text2.size();
    vector<int> dp(n+1, 0);
    for (int i = 1; i <= m; ++i) {
        int prev = 0;
        for (int j = 1; j <= n; ++j) {
            int temp = dp[j];
            if (text1[i-1] == text2[j-1])
                dp[j] = prev + 1;
            else
                dp[j] = max(dp[j], dp[j-1]);
            prev = temp;
        }
    }
    return dp[n];
}

int main() {
    cout << longestCommonSubseq("abcde","ace") << endl; // 3
    cout << longestCommonSubseq("abc","abc") << endl; // 3
    cout << longestCommonSubseq("abc","def") << endl; // 0
    return 0;
}
