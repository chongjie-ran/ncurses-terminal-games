// LC139 Word Break
// 给定字符串s和词典wordDict，判断s能否被空格拆分成字典中的词
// 方法：DP - dp[i]=s[0..i)能否拆分
#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
using namespace std;

class Solution {
public:
    bool wordBreak(string s, vector<string>& wordDict) {
        unordered_set<string> dict(wordDict.begin(), wordDict.end());
        int n = s.size();
        vector<bool> dp(n+1, false);
        dp[0] = true;
        for (int i = 1; i <= n; i++) {
            for (int j = 0; j < i; j++) {
                if (dp[j] && dict.count(s.substr(j, i-j))) {
                    dp[i] = true;
                    break;
                }
            }
        }
        return dp[n];
    }
};

int main() {
    Solution sol;
    vector<string> dict1 = {"leet","code"};
    cout << (sol.wordBreak("leetcode", dict1) ? "1" : "0") << endl; // 1
    vector<string> dict2 = {"apple","pen"};
    cout << (sol.wordBreak("applepenapple", dict2) ? "1" : "0") << endl; // 1
    vector<string> dict3 = {"cats","dog","sand","and","cat"};
    cout << (sol.wordBreak("catsandog", dict3) ? "1" : "0") << endl; // 0
    return 0;
}
