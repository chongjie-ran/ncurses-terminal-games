#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<int> buildKMP(string& p) {
    int m = p.size();
    vector<int> pi(m, 0);
    for (int i = 1; i < m; i++) {
        int j = pi[i-1];
        while (j > 0 && p[i] != p[j]) j = pi[j-1];
        if (p[i] == p[j]) j++;
        pi[i] = j;
    }
    return pi;
}

int strStr(string haystack, string needle) {
    if (needle.empty()) return 0;
    auto pi = buildKMP(needle);
    int j = 0;
    for (int i = 0; i < haystack.size(); i++) {
        while (j > 0 && haystack[i] != needle[j]) j = pi[j-1];
        if (haystack[i] == needle[j]) j++;
        if (j == needle.size()) return i - j + 1;
    }
    return -1;
}

bool repeatedSubstringPattern(string s) {
    string p = s + s;
    return p.find(s, 1) != string::npos && p.find(s, 1) < s.size();
}

int main() {
    // LC28
    cout << "LC28 strStr:" << endl;
    cout << "  hello/ll -> " << strStr("hello", "ll") << " (expect 2)" << endl;
    cout << "  aaa/aaa -> " << strStr("aaaaa", "aaa") << " (expect 0)" << endl;
    cout << "  abc/def -> " << strStr("abc", "def") << " (expect -1)" << endl;
    
    // LC459
    cout << "LC459 repeatedSubstringPattern:" << endl;
    cout << "  abab -> " << repeatedSubstringPattern("abab") << " (expect true)" << endl;
    cout << "  aba -> " << repeatedSubstringPattern("aba") << " (expect false)" << endl;
    cout << "  abcabcabc -> " << repeatedSubstringPattern("abcabcabc") << " (expect true)" << endl;
    
    return 0;
}
