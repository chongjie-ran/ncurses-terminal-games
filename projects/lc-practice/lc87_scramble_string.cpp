// LC87 Scramble String — Hard
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <string>
using namespace std;

class Solution {
public:
    unordered_map<string, bool> memo;
    bool isScramble(string s1, string s2) {
        if (s1 == s2) return true;
        if (s1.size() != s2.size()) return false;
        string key = s1 + "#" + s2;
        if (memo.count(key)) return memo[key];
        int n = s1.size();
        int cnt[26] = {0};
        for (int i = 0; i < n; i++) cnt[s1[i]-'a']++;
        for (int i = 0; i < n; i++) cnt[s2[i]-'a']--;
        for (int i = 0; i < 26; i++) if (cnt[i] != 0) return memo[key] = false;
        bool result = false;
        for (int len = 1; len < n && !result; len++) {
            if (isScramble(s1.substr(0,len), s2.substr(0,len)) &&
                isScramble(s1.substr(len), s2.substr(len)))
                result = true;
            if (!result &&
                isScramble(s1.substr(0,len), s2.substr(n-len)) &&
                isScramble(s1.substr(len), s2.substr(0,n-len)))
                result = true;
        }
        return memo[key] = result;
    }
};

int main() {
    Solution s;
    printf("great→rgeat: %d (expect 1)\n", s.isScramble("great","rgeat"));
    printf("abcde→caebd: %d (expect 0)\n", s.isScramble("abcde","caebd"));
    printf("a→a: %d (expect 1)\n", s.isScramble("a","a"));
    printf("abc→bca: %d (expect 1)\n", s.isScramble("abc","bca"));
    printf("abc→cba: %d (expect 1)\n", s.isScramble("abc","cba"));
    return 0;
}
