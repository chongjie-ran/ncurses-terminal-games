// LC139 Word Break
// Medium | DP
#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
using namespace std;

bool wordBreak(string s, vector<string>& wordDict) {
    unordered_set<string> dict(wordDict.begin(), wordDict.end());
    int n = s.size();
    vector<bool> dp(n+1, false);
    dp[0] = true;
    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (dp[j] && dict.count(s.substr(j, i-j))) {
                dp[i] = true;
                break;
            }
        }
    }
    return dp[n];
}

int main() {
    vector<string> wd1 = {"leet","code"};
    cout << (wordBreak("leetcode", wd1) ? "1" : "0") << endl; // 1
    vector<string> wd2 = {"apple","pen"};
    cout << (wordBreak("applepenapple", wd2) ? "1" : "0") << endl; // 1
    vector<string> wd3 = {"cats","dog","sand","and","cat"};
    cout << (wordBreak("catsandog", wd3) ? "1" : "0") << endl; // 0
    return 0;
}
