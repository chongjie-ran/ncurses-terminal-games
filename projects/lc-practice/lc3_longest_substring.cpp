// LC3 最长无重复子串 - 滑动窗口+双指针
#include <iostream>
#include <string>
#include <unordered_set>
using namespace std;

// 滑动窗口模板
int lengthOfLongestSubstring(string s) {
    unordered_set<char> seen;
    int left = 0, maxLen = 0;
    for (int right = 0; right < s.size(); right++) {
        while (seen.count(s[right])) {
            seen.erase(s[left]);
            left++;
        }
        seen.insert(s[right]);
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

int main() {
    string s1 = "abcabcbb";
    string s2 = "bbbbb";
    string s3 = "pwwkew";
    cout << "abcabcbb: " << lengthOfLongestSubstring(s1) << " (expect 3)" << endl;
    cout << "bbbbb: " << lengthOfLongestSubstring(s2) << " (expect 1)" << endl;
    cout << "pwwkew: " << lengthOfLongestSubstring(s3) << " (expect 3)" << endl;
    return 0;
}
