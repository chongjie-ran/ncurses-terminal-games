// LC76: 最小覆盖子串 - 滑动窗口+双指针+哈希
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

string minWindow(string s, string t) {
    if (s.empty() || t.empty() || s.size() < t.size()) return "";
    
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;
    
    int left = 0, right = 0, valid = 0;
    int start = 0, len = INT_MAX;
    int needCount = need.size();
    
    while (right < s.size()) {
        char c = s[right];
        right++;
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;
        }
        
        while (valid == needCount) {
            if (right - left < len) {
                start = left;
                len = right - left;
            }
            char d = s[left];
            left++;
            if (need.count(d)) {
                if (window[d] == need[d]) valid--;
                window[d]--;
            }
        }
    }
    return len == INT_MAX ? "" : s.substr(start, len);
}

int main() {
    cout << minWindow("ADOBECODEBANC", "ABC") << endl; // "BANC"
    cout << minWindow("a", "a") << endl;               // "a"
    cout << minWindow("a", "aa") << endl;              // ""
    return 0;
}
