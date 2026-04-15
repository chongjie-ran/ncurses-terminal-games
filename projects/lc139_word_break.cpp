#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

bool wordBreak(const string& s, const vector<string>& wordDict) {
    unordered_set<string> dict(wordDict.begin(), wordDict.end());
    int n = s.size();
    vector<char> dp(n + 1, 0);
    dp[0] = 1;
    
    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (dp[j]) {
                string word = s.substr(j, i - j);
                if (dict.count(word)) {
                    dp[i] = 1;
                    break;
                }
            }
        }
    }
    return dp[n];
}

int main() {
    struct Test { string s; vector<string> dict; bool expected; };
    vector<Test> tests = {
        {"leetcode", {"leet","code"}, true},
        {"applepenapple", {"apple","pen"}, true},
        {"catsandog", {"cats","dog","sand","and","cat"}, false},
        {"catsanddog", {"cats","dog","sand","and","cat"}, true},
        {"", {}, true},
        {"a", {"a"}, true},
        {"a", {"b"}, false},
        {"bb", {"a","b","bbb","bbbb"}, true},
        {"aaaaaaa", {"aaaa","aaa"}, true},
        {"ab", {"a","b"}, true},
    };
    
    for (auto& t : tests) {
        bool r = wordBreak(t.s, t.dict);
        cout << (r == t.expected ? "✅" : "❌") 
             << " \"" << t.s << "\" => " << (r ? "true" : "false") << "\n";
    }
    return 0;
}
