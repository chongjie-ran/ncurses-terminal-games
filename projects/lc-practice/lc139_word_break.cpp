#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
using namespace std;
// LC139 单词拆分 - 完全背包
bool wordBreak(string s, vector<string>& wordDict) {
    unordered_set<string> dict(wordDict.begin(), wordDict.end());
    int n = s.size();
    vector<bool> dp(n+1, false);
    dp[0] = true;
    for (int i=1; i<=n; i++) {
        for (int j=0; j<i; j++) {
            if (dp[j] && dict.count(s.substr(j, i-j))) {
                dp[i] = true; break;
            }
        }
    }
    return dp[n];
}
int main() {
    vector<string> d1 = {"leet","code"};
    cout << wordBreak("leetcode", d1) << endl;  // 1
    vector<string> d2 = {"apple","pen"};
    cout << wordBreak("applepenapple", d2) << endl;  // 1
    vector<string> d3 = {"cats","dog","sand","and","cat"};
    cout << wordBreak("catsandog", d3) << endl;  // 0
    vector<string> d4 = {"a","bc"};
    cout << wordBreak("abcd", d4) << endl;  // 0
}
