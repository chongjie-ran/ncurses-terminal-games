/*
LC76 最小覆盖子串
Hard - 滑动窗口

给你一个字符串 s 和一个字符串 t 。
返回 s 中涵盖 t 所有字符的最小子串。
如果 s 中不存在这样的子串，返回空字符串 ""。

思路：滑动窗口
1. need[t字符] = 需要次数
2. window[s字符] = 当前窗口内该字符出现次数
3. valid = 已满足的字符种类数
4. 右扩 → 满足后左缩 → 更新最小值
*/

#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;

    int left = 0, right = 0;
    int valid = 0; // 已满足的字符种类数
    int minLen = INT_MAX;
    int minStart = 0;

    while (right < s.size()) {
        char c = s[right];
        right++;

        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) {
                valid++;
            }
        }

        // 尝试收缩左边界
        while (valid == need.size()) {
            // 更新最小窗口
            if (right - left < minLen) {
                minLen = right - left;
                minStart = left;
            }

            char d = s[left];
            left++;

            if (need.count(d)) {
                if (window[d] == need[d]) {
                    valid--; // 即将失去一个满足的字符
                }
                window[d]--;
            }
        }
    }

    return minLen == INT_MAX ? "" : s.substr(minStart, minLen);
}

int main() {
    // 测试用例
    string tests[][2] = {
        {"ADOBECODEBANC", "ABC"},    // "BANC"
        {"a", "a"},                  // "a"
        {"a", "aa"},                 // ""
        {"ABBC", "ABC"},             // "BABC" 或 "BC"
        {"acbbaca", "abc"},          // "baca"
    };

    for (auto& test : tests) {
        string s = test[0], t = test[1];
        string result = minWindow(s, t);
        cout << "s=\"" << s << "\", t=\"" << t << "\" => \"" << result << "\"" << endl;
    }
    return 0;
}
