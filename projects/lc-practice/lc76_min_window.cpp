// LC76 最小覆盖子串 - 滑动窗口
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;
    int left = 0, right = 0, valid = 0, start = 0, minLen = INT_MAX;
    int needCount = need.size();
    
    while (right < s.size()) {
        char c = s[right++];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;
        }
        
        while (valid == needCount) {
            if (right - left < minLen) {
                minLen = right - left;
                start = left;
            }
            char d = s[left++];
            if (need.count(d)) {
                if (window[d] == need[d]) valid--;
                window[d]--;
            }
        }
    }
    return minLen == INT_MAX ? "" : s.substr(start, minLen);
}

int main() {
    string s1 = "ADOBECODEBANC", t1 = "ABC";
    string s2 = "a", t2 = "a";
    string s3 = "a", t3 = "aa";
    cout << "ADOBECODEBANC+ABC: " << minWindow(s1, t1) << " (expect BANC)" << endl;
    cout << "a+a: " << minWindow(s2, t2) << " (expect a)" << endl;
    cout << "a+aa: " << minWindow(s3, t3) << " (expect )" << endl;
    return 0;
}
