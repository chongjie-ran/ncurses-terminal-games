/**
 * LeetCode 3 - Longest Substring Without Repeating Characters (无重复字符的最长子串)
 * 
 * 给定一个字符串 s，找出其中不含重复字符的最长子串的长度。
 * 
 * 示例:
 *   输入: "abcabcbb"  输出: 3  (子串 "abc")
 *   输入: "bbbbb"     输出: 1  (子串 "b")
 *   输入: "pwwkew"    输出: 3  (子串 "wke")
 * 
 * 核心思想: 滑动窗口 (Sliding Window) + 哈希集合
 * - 用 left/right 双指针维护一个无重复字符的窗口
 * - right 不断扩展，left 在出现重复时收缩
 * - 用 unordered_set 或数组记录窗口内的字符
 * 
 * 优化: 用数组替代 unordered_set，可将 O(n) 降至 O(1) 均摊
 */

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

/**
 * 方法1: 滑动窗口 + unordered_set
 * 
 * 时间复杂度: O(2n) = O(n)，每个字符最多被访问2次
 * 空间复杂度: O(min(m, n))，m 为字符集大小
 */
int lengthOfLongestSubstring_set(const string& s) {
    unordered_set<char> window;
    int left = 0, right = 0;
    int best = 0;
    
    while (right < static_cast<int>(s.size())) {
        char c = s[right];
        // 收缩左边界直到没有重复
        while (window.count(c)) {
            window.erase(s[left]);
            ++left;
        }
        window.insert(c);
        best = max(best, right - left + 1);
        ++right;
    }
    
    return best;
}

/**
 * 方法2: 滑动窗口 + 数组（最优）
 * 
 * 优化：用 int[256] 记录每个字符最后一次出现的位置
 * 当发现重复字符时，直接将 left 跳跃到上次出现位置的下一位
 * 
 * 时间复杂度: O(n) - 每个字符最多访问2次
 * 空间复杂度: O(min(m, n))
 */
int lengthOfLongestSubstring_optimized(const string& s) {
    // last_seen[c] = c 上次出现的索引，-1 表示未出现
    vector<int> last_seen(256, -1);
    int left = 0, right = 0;
    int best = 0;
    
    while (right < static_cast<int>(s.size())) {
        unsigned char ch = s[right];
        // 如果字符 ch 曾在窗口内出现过，且位置 >= left
        if (last_seen[ch] >= left) {
            // 收缩左边界到上次出现位置的下一位
            left = last_seen[ch] + 1;
        }
        last_seen[ch] = right;
        best = max(best, right - left + 1);
        ++right;
    }
    
    return best;
}

/**
 * 手动模拟: s = "abcabcbb"
 * 
 *  right  ch   last_seen[ch]  left  window        best
 *  -----  ---  -------------  ----  ----------    ----
 *    0    a        0          0     "a"            1
 *    1    b        1          0     "ab"           2
 *    2    c        2          0     "abc"          3
 *    3    a        3          1     "bca"          3  (last_seen[a]=0>=left=0 → left=1)
 *    4    b        4          2     "cab"          3  (last_seen[b]=1>=left=1 → left=2)
 *    5    c        5          3     "abc"          3  (last_seen[c]=2>=left=2 → left=3)
 *    6    b        6          4     "cb"           3  (last_seen[b]=4>=left=3 → left=5? Wait...)
 *                                          No: last_seen[b]=4 >= left=3 → left=5 → window="b" → best=3
 *    7    b        7          6     "b"           3  (last_seen[b]=6>=left=6? No, 6>=6 → left=7? wait)
 * 
 * 正确模拟（方法2）:
 *  ch='a': last_seen['a']=-1<left=0 → ok, last_seen['a']=0, best=max(0,0-0+1)=1
 *  ch='b': last_seen['b']=-1<left=0 → ok, last_seen['b']=1, best=2
 *  ch='c': last_seen['c']=-1<left=0 → ok, last_seen['c']=2, best=3
 *  ch='a': last_seen['a']=0 >= left=0 → left=0+1=1, last_seen['a']=3, best=3 (r-l+1=3)
 *  ch='b': last_seen['b']=1 >= left=1 → left=1+1=2, last_seen['b']=4, best=max(3,5-2+1)=3
 *  ch='c': last_seen['c']=2 >= left=2 → left=2+1=3, last_seen['c']=5, best=max(3,6-3+1)=3
 *  ch='b': last_seen['b']=4 >= left=3 → left=4+1=5, last_seen['b']=6, best=max(3,7-5+1)=3
 *  ch='b': last_seen['b']=6 >= left=5 → left=6+1=7, last_seen['b']=7, best=max(3,8-7+1)=2
 * 
 * 最终结果: 3
 */

// 测试
int main() {
    vector<string> test_cases = {
        "abcabcbb",   // expected: 3
        "bbbbb",      // expected: 1
        "pwwkew",     // expected: 3
        "",           // expected: 0
        " ",          // expected: 1
        "au",         // expected: 2
        "dvdf",       // expected: 3 (vdf)
    };
    
    for (const string& s : test_cases) {
        int r1 = lengthOfLongestSubstring_set(s);
        int r2 = lengthOfLongestSubstring_optimized(s);
        cout << "\"" << s << "\" -> set: " << r1 << ", optimized: " << r2;
        if (r1 != r2) cout << "  [MISMATCH!]";
        cout << endl;
    }
    
    return 0;
}
