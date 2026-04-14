// LC567 Permutation in String
// 判断s2是否包含s1的某个排列作为子串
// 滑动窗口 + 字符计数比较
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
using namespace std;

class Solution {
public:
    bool checkInclusion(string s1, string s2) {
        if (s1.size() > s2.size()) return false;
        
        vector<int> cnt1(26, 0), cnt2(26, 0);
        int n1 = s1.size(), n2 = s2.size();
        
        // 初始化第一个窗口
        for (int i = 0; i < n1; i++) {
            cnt1[s1[i] - 'a']++;
            cnt2[s2[i] - 'a']++;
        }
        
        // 滑动窗口
        for (int i = 0; i < n2 - n1; i++) {
            if (matches(cnt1, cnt2)) return true;
            // 右移窗口: 左边出一字符,右边进一字符
            cnt2[s2[i + n1] - 'a']++;
            cnt2[s2[i] - 'a']--;
        }
        return matches(cnt1, cnt2);
    }
    
    bool matches(vector<int>& a, vector<int>& b) {
        for (int i = 0; i < 26; i++)
            if (a[i] != b[i]) return false;
        return true;
    }
};

int main() {
    Solution sol;
    
    // Test 1: s1="ab", s2="eidbaooo" -> true
    assert(sol.checkInclusion("ab", "eidbaooo") == true);
    
    // Test 2: s1="ab", s2="eidboaoo" -> false  
    assert(sol.checkInclusion("ab", "eidboaoo") == false);
    
    // Test 3: s1="hello", s2="oohellooo" -> true
    assert(sol.checkInclusion("hello", "oohellooo") == true);
    
    // Test 4: s1="abc", s2="bcab" -> true
    assert(sol.checkInclusion("abc", "bcab") == true);
    
    // Test 5: s1="a", s2="bc" -> false
    assert(sol.checkInclusion("a", "bc") == false);
    
    printf("5/5 tests passed\n");
    return 0;
}
