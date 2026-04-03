// LC3: Longest Substring Without Repeating Characters
// Pattern: Sliding Window with HashMap/Array
// Difficulty: Medium
// Date: 2026-04-01

#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

// Approach 1: Sliding Window with HashMap
// Time: O(n), Space: O(min(m, n)) where m = charset size
int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> seen;
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < (int)s.size(); right++) {
        char c = s[right];
        if (seen.count(c) && seen[c] >= left) {
            left = seen[c] + 1; // shrink window
        }
        seen[c] = right;
        maxLen = max(maxLen, right - left + 1);
    }
    
    return maxLen;
}

// Approach 2: Sliding Window with Array (faster, 26 chars only)
// Time: O(n), Space: O(1)
int lengthOfLongestSubstringArray(string s) {
    int idx[256];
    fill(begin(idx), end(idx), -1);
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < (int)s.size(); right++) {
        char c = s[right];
        if (idx[(int)c] >= left) {
            left = idx[(int)c] + 1;
        }
        idx[(int)c] = right;
        maxLen = max(maxLen, right - left + 1);
    }
    
    return maxLen;
}

int main() {
    // Test cases
    struct Test { string s; int expected; };
    Test tests[] = {
        {"abcabcbb", 3},  // "abc"
        {"bbbbb", 1},     // "b"
        {"pwwkew", 3},    // "wke"
        {"", 0},          // empty
        {"au", 2},        // "au"
        {"dvdf", 3},      // "vdf"
    };
    
    for (auto& t : tests) {
        int got = lengthOfLongestSubstring(t.s);
        cout << "s='" << t.s << "' => " << got << " (expected " << t.expected << ") "
             << (got == t.expected ? "✓" : "✗") << endl;
    }
    
    return 0;
}
