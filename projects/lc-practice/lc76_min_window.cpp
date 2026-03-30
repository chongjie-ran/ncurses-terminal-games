/*
LC76 - Minimum Window Substring
分类: Sliding Window + Hash
难度: Hard
日期: 2026-03-31 01:45

思路:
  双指针 sliding window:
  - right 扩展窗口，直到窗口包含 t 的所有字符
  - left 收缩窗口，尝试最小化同时保持覆盖
  - 用 need[256] 和 have[256] 记录字符计数差
  
关键技巧:
  - formed = 当前窗口中已满足 need 要求的字符种类数
  - right 左开右闭 [r, l) 窗口模型
  - 每当 formed == required 时，尝试收缩 left

时间: O(|s|+|t|), 空间: O(1) (256 固定数组)
*/

#include <iostream>
#include <vector>
#include <string>
#include <climits>
using namespace std;

class Solution {
public:
    string minWindow(string s, string t) {
        if (s.empty() || t.empty() || s.size() < t.size()) return "";
        
        vector<int> need(256, 0), have(256, 0);
        for (char c : t) need[(unsigned char)c]++;
        
        int required = 0;
        for (int i = 0; i < 256; i++) if (need[i] > 0) required++;
        
        int left = 0, right = 0;
        int formed = 0;
        int minLen = INT_MAX, minL = 0;
        
        // right 扩展窗口
        while (right < s.size()) {
            char c = s[right];
            if (need[(unsigned char)c] > 0) {
                have[(unsigned char)c]++;
                if (have[(unsigned char)c] == need[(unsigned char)c]) {
                    formed++;
                }
            }
            right++;
            
            // 收缩 left 尝试最小化
            while (formed == required && left < right) {
                char d = s[left];
                if (right - left < minLen) {
                    minLen = right - left;
                    minL = left;
                }
                if (need[(unsigned char)d] > 0) {
                    if (have[(unsigned char)d] == need[(unsigned char)d]) {
                        formed--;
                    }
                    have[(unsigned char)d]--;
                }
                left++;
            }
        }
        
        return minLen == INT_MAX ? "" : s.substr(minL, minLen);
    }
};

int main() {
    Solution sol;
    
    struct Test { string s, t, expect; };
    Test tests[] = {
        {"ADOBECODEBANC", "ABC", "BANC"},
        {"a", "a", "a"},
        {"a", "aa", ""},
        {"abcde", "ae", "abcde"},
        {"abc", "abc", "abc"},
        {"ABC", "ABC", "ABC"},
    };
    
    for (int i = 0; i < 6; i++) {
        string out = sol.minWindow(tests[i].s, tests[i].t);
        bool ok = out == tests[i].expect;
        cout << "Test " << i+1 << " (expect '" << tests[i].expect << "'): '" 
             << out << "' " << (ok ? "✅" : "❌") << endl;
    }
    return 0;
}
