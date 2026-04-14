// LC76 Minimum Window Substring
// 最小覆盖子串：找s中包含t所有字符的最小子串
// 滑动窗口进阶：双指针 + 字符计数 + 扩张+收缩模式
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <climits>
using namespace std;

class Solution {
public:
    string minWindow(string s, string t) {
        if (s.empty() || t.empty() || s.size() < t.size()) return "";
        
        // 需要的字符计数
        unordered_map<char, int> need;
        for (char c : t) need[c]++;
        
        int required = need.size();  // 需要的不同字符种类数
        int formed = 0;               // 当前窗口已满足的种类数
        unordered_map<char, int> window;
        
        int left = 0, right = 0;
        int minLen = INT_MAX, minL = 0, minR = 0;
        
        while (right < s.size()) {
            char c = s[right];
            window[c]++;
            
            // 检查这个字符是否刚好满足需要
            if (need.count(c) && window[c] == need[c]) {
                formed++;
            }
            
            // 尝试收缩窗口
            while (left <= right && formed == required) {
                char cl = s[left];
                
                // 更新最小窗口
                int curLen = right - left + 1;
                if (curLen < minLen) {
                    minLen = curLen;
                    minL = left;
                    minR = right;
                }
                
                // 收缩前先检查是否会影响满足条件
                if (need.count(cl) && window[cl] == need[cl]) {
                    formed--;
                }
                window[cl]--;
                left++;
            }
            
            right++;
        }
        
        return minLen == INT_MAX ? "" : s.substr(minL, minLen);
    }
};

// 测试
int main() {
    Solution sol;
    
    // Test 1: s="ADOBECODEBANC", t="ABC"
    string result1 = sol.minWindow("ADOBECODEBANC", "ABC");
    cout << "Test1: " << result1 << " (expect BANC)" << endl;
    
    // Test 2: s="a", t="a"
    string result2 = sol.minWindow("a", "a");
    cout << "Test2: " << result2 << " (expect a)" << endl;
    
    // Test 3: s="a", t="aa"
    string result3 = sol.minWindow("a", "aa");
    cout << "Test3: " << result3 << " (expect )" << endl;
    
    // Test 4: s="ab", t="b"
    string result4 = sol.minWindow("ab", "b");
    cout << "Test4: " << result4 << " (expect b)" << endl;
    
    // Test 5: s="bbaa", t="aba"
    string result5 = sol.minWindow("bbaa", "aba");
    cout << "Test5: " << result5 << " (expect baa or aba)" << endl;
    
    return 0;
}
