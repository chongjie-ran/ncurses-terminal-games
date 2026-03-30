#include <iostream>
#include <vector>
#include <string>
#include <climits>
using namespace std;

// LC76 Minimum Window Substring
// Hard: 最小覆盖子串 - 滑动窗口
// 思路: 右指针扩展，找满足条件后左指针收缩
// 时间: O(N+M), 空间: O(1)

class Solution {
public:
    string minWindow(string s, string t) {
        int need[256] = {0}, window[256] = {0};
        for (char c : t) need[(unsigned char)c]++;

        int left = 0, right = 0;
        int formed = 0, required = 0;
        for (int i = 0; i < 256; i++) if (need[i] > 0) required++;

        int best_len = INT_MAX, best_l = 0, best_r = 0;

        while (right < (int)s.size()) {
            char c = s[right];
            window[(unsigned char)c]++;
            if (need[(unsigned char)c] > 0 && window[(unsigned char)c] == need[(unsigned char)c]) {
                formed++;
            }

            while (formed == required && left <= right) {
                int len = right - left + 1;
                if (len < best_len) {
                    best_len = len;
                    best_l = left;
                    best_r = right;
                }
                char cl = s[left];
                if (need[(unsigned char)cl] > 0 && window[(unsigned char)cl] == need[(unsigned char)cl]) {
                    formed--;
                }
                window[(unsigned char)cl]--;
                left++;
            }
            right++;
        }

        if (best_len == INT_MAX) return "";
        return s.substr(best_l, best_len);
    }
};

int main() {
    Solution sol;
    cout << "Test1 (BANC): " << sol.minWindow("ADOBECODEBANC", "ABC") << endl;
    cout << "Test2 (a): " << sol.minWindow("a", "a") << endl;
    cout << "Test3 (''): '" << sol.minWindow("a", "aa") << "'" << endl;
    return 0;
}
