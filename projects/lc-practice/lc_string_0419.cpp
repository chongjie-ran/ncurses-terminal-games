#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

// LC5 最长回文子串 - 中心扩展法
string longestPalindrome(string s) {
    if (s.empty()) return "";
    int n = s.size(), start = 0, maxLen = 1;
    
    auto expand = [&](int l, int r) {
        while (l >= 0 && r < n && s[l] == s[r]) {
            if (r - l + 1 > maxLen) {
                start = l;
                maxLen = r - l + 1;
            }
            l--; r++;
        }
    };
    
    for (int i = 0; i < n; i++) {
        expand(i, i);     // 奇数
        expand(i, i+1);   // 偶数
    }
    return s.substr(start, maxLen);
}

// LC647 回文子串计数
int countSubstrings(string s) {
    int n = s.size(), cnt = 0;
    for (int i = 0; i < n; i++) {
        for (int l = i, r = i; l >= 0 && r < n && s[l] == s[r]; l--, r++) cnt++;
        for (int l = i, r = i+1; l >= 0 && r < n && s[l] == s[r]; l--, r++) cnt++;
    }
    return cnt;
}

// LC409 最长回文串
int longestPalindromeII(string s) {
    unordered_map<char, int> freq;
    for (char c : s) freq[c]++;
    int len = 0, odd = 0;
    for (auto& p : freq) {
        if (p.second % 2 == 0) len += p.second;
        else { len += p.second - 1; odd = 1; }
    }
    return len + odd;
}

int main() {
    // 测试
    vector<string> tests = {"babad", "cbbd", "abcba"};
    for (auto& t : tests) {
        cout << "LC5: \"" << t << "\" -> \"" << longestPalindrome(t) << "\"" << endl;
    }
    
    vector<string> pal = {"abc", "aaa", "aba"};
    for (auto& t : pal) {
        cout << "LC647: \"" << t << "\" -> " << countSubstrings(t) << endl;
    }
    
    vector<string> pal2 = {"abccccdd", "a"};
    for (auto& t : pal2) {
        cout << "LC409: \"" << t << "\" -> " << longestPalindromeII(t) << endl;
    }
    
    return 0;
}

// ========== KMP算法 ==========
// LC28 找出字符串中第一个匹配项的下标 (Implement strStr)
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

// LC459 重复的子字符串
bool repeatedSubstringPattern(string s) {
    string p = s + s;
    return p.find(s, 1) != string::npos && p.find(s, 1) < s.size();
}
