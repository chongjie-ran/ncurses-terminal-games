#include <iostream>
#include <string>
using namespace std;

// 直接内联测试
class Solution {
public:
    string minWindow(string s, string t) {
        if (s.empty() || t.empty() || s.size() < t.size()) return "";
        unordered_map<char, int> need;
        for (char c : t) need[c]++;
        int required = need.size(), formed = 0;
        unordered_map<char, int> window;
        int left = 0, right = 0, minLen = INT_MAX, minL = 0, minR = 0;
        while (right < s.size()) {
            char c = s[right];
            window[c]++;
            if (need.count(c) && window[c] == need[c]) formed++;
            while (left <= right && formed == required) {
                char cl = s[left];
                int curLen = right - left + 1;
                if (curLen < minLen) { minLen = curLen; minL = left; minR = right; }
                if (need.count(cl) && window[cl] == need[cl]) formed--;
                window[cl]--; left++;
            }
            right++;
        }
        return minLen == INT_MAX ? "" : s.substr(minL, minLen);
    }
};

int main() {
    Solution sol;
    // Test 6: 全是相同字符
    string r6 = sol.minWindow("aaa", "aa");
    cout << "Test6: '" << r6 << "' len=" << r6.size() << " (expect 2)" << endl;
    
    // Test 7: t包含s没有的字符
    string r7 = sol.minWindow("abc", "d");
    cout << "Test7: '" << r7 << "' (expect empty)" << endl;
    
    // Test 8: 完全覆盖
    string r8 = sol.minWindow("abc", "abc");
    cout << "Test8: '" << r8 << "' (expect abc)" << endl;
    
    // Test 9: t="A"大写
    string r9 = sol.minWindow("AB", "A");
    cout << "Test9: '" << r9 << "' (expect A)" << endl;
    
    return 0;
}
