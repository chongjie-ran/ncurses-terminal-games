// LC3: Longest Substring Without Repeating Characters
// 滑动窗口+哈希集合
#include <unordered_set>
#include <string>
#include <cassert>
#include <iostream>
using namespace std;

int lengthOfLongestSubstring(string s) {
    unordered_set<char> win;
    int l = 0, ans = 0;
    for (int r = 0; r < (int)s.size(); r++) {
        while (win.count(s[r])) {
            win.erase(s[l]);
            l++;
        }
        win.insert(s[r]);
        ans = max(ans, r - l + 1);
    }
    return ans;
}

int main() {
    // abcabcbb -> 3
    assert(lengthOfLongestSubstring("abcabcbb") == 3);
    // bbbbb -> 1
    assert(lengthOfLongestSubstring("bbbbb") == 1);
    // pwwkew -> 3
    assert(lengthOfLongestSubstring("pwwkew") == 3);
    // "" -> 0
    assert(lengthOfLongestSubstring("") == 0);
    // dvdf -> 3
    assert(lengthOfLongestSubstring("dvdf") == 3);
    cout << "LC3: 5/5 all passed ✅" << endl;
    return 0;
}
