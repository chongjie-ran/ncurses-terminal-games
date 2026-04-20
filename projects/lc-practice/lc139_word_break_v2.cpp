#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
using namespace std;

// DP: dp[i] = s[0:i]是否可拆分
bool wordBreak(string s, vector<string>& wordDict) {
    unordered_set<string> dict(wordDict.begin(), wordDict.end());
    int n = s.size();
    vector<char> dp(n+1, 0);
    dp[0] = 1;
    
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            if (dp[j] && dict.count(s.substr(j, i-j))) {
                dp[i] = 1;
                break;
            }
        }
    }
    return dp[n];
}

int main() {
    cout << "LC139 Word Break - DP Solution" << endl;
    
    // Test 1
    string s1 = "leetcode";
    vector<string> dict1 = {"leet", "code"};
    cout << "Test1 (leetcode, [leet,code]): " << wordBreak(s1, dict1) << " (expected: 1)" << endl;
    
    // Test 2
    string s2 = "applepenapple";
    vector<string> dict2 = {"apple", "pen"};
    cout << "Test2 (applepenapple, [apple,pen]): " << wordBreak(s2, dict2) << " (expected: 1)" << endl;
    
    // Test 3
    string s3 = "catsandog";
    vector<string> dict3 = {"cats", "dog", "sand", "and", "cat"};
    cout << "Test3 (catsandog, [cats,dog,sand,and,cat]): " << wordBreak(s3, dict3) << " (expected: 0)" << endl;
    
    // Test 4
    string s4 = "a";
    vector<string> dict4 = {"b"};
    cout << "Test4 (a, [b]): " << wordBreak(s4, dict4) << " (expected: 0)" << endl;
    
    return 0;
}
