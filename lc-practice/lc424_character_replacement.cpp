// LC424: Longest Repeating Character Replacement
// 滑动窗口 + 计数数组
// 核心: 维护窗口内最多字符的频次, 当 window_size - max_freq > k 时收缩

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

int characterReplacement(string s, int k) {
    vector<int> cnt(26, 0);
    int left = 0, maxFreq = 0, result = 0;
    
    for (int right = 0; right < (int)s.size(); right++) {
        cnt[s[right] - 'A']++;
        maxFreq = max(maxFreq, cnt[s[right] - 'A']);
        
        while (right - left + 1 - maxFreq > k) {
            cnt[s[left] - 'A']--;
            left++;
        }
        
        result = max(result, right - left + 1);
    }
    return result;
}

int main() {
    struct Test { string s; int k; int expected; };
    vector<Test> tests = {
        {"ABAB", 2, 4},
        {"AABABBA", 1, 4},
        {"AAAA", 0, 4},
        {"ABCBA", 2, 4},
        {"", 0, 0}
    };
    
    int passed = 0;
    for (auto& t : tests) {
        int got = characterReplacement(t.s, t.k);
        bool ok = (got == t.expected);
        if (ok) passed++;
        cout << (ok ? "✅" : "❌") << " \"" << t.s << "\" k=" << t.k 
             << " → " << got << " (expected " << t.expected << ")\n";
    }
    cout << "\n" << passed << "/" << tests.size() << " passed\n";
    return (passed == tests.size()) ? 0 : 1;
}
