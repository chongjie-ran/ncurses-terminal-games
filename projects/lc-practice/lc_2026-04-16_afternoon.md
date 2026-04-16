# LC练习 2026-04-16 下午（续）

## 12:13 单调队列 + KMP专题

---

### 单调队列核心模式
**滑动窗口最大值**: 维护递减双端队列，队首永远是当前窗口最大
```cpp
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq; // 存索引
    vector<int> res;
    for (int i=0; i<nums.size(); i++) {
        // 移除窗口外的
        if (!dq.empty() && dq.front() <= i-k) dq.pop_front();
        // 移除比当前小的（保持递减）
        while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
        dq.push_back(i);
        // 记录答案
        if (i >= k-1) res.push_back(nums[dq.front()]);
    }
    return res;
}
```

### LC239 滑动窗口最大值
```cpp
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;
    vector<int> res;
    for (int i=0; i<nums.size(); i++) {
        if (!dq.empty() && dq.front() <= i-k) dq.pop_front();
        while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
        dq.push_back(i);
        if (i >= k-1) res.push_back(nums[dq.front()]);
    }
    return res;
}
```

---

### KMP核心模式
**部分匹配表(PMT/pi)**: 最长相等前后缀长度
**匹配过程**: 模式串滑动，而非文本串回退
```cpp
// 构建pi数组
vector<int> buildPi(const string& p) {
    vector<int> pi(p.size(), 0);
    for (int i=1; i<p.size(); i++) {
        int j = pi[i-1];
        while (j > 0 && p[i] != p[j]) j = pi[j-1];
        if (p[i] == p[j]) j++;
        pi[i] = j;
    }
    return pi;
}

// KMP搜索
int kmp(const string& s, const string& p) {
    vector<int> pi = buildPi(p);
    int j = 0;
    for (int i=0; i<s.size(); i++) {
        while (j > 0 && s[i] != p[j]) j = pi[j-1];
        if (s[i] == p[j]) j++;
        if (j == p.size()) return i - p.size() + 1; // 找到
    }
    return -1;
}
```

### LC28 找出字符串中第一个匹配项的下标
```cpp
int strStr(string haystack, string needle) {
    if (needle.empty()) return 0;
    vector<int> pi(needle.size(), 0);
    for (int i=1; i<needle.size(); i++) {
        int j = pi[i-1];
        while (j > 0 && needle[i] != needle[j]) j = pi[j-1];
        if (needle[i] == needle[j]) j++;
        pi[i] = j;
    }
    int j = 0;
    for (int i=0; i<haystack.size(); i++) {
        while (j > 0 && haystack[i] != needle[j]) j = pi[j-1];
        if (haystack[i] == needle[j]) j++;
        if (j == needle.size()) return i - needle.size() + 1;
    }
    return -1;
}
```

### LC459 重复的子字符串
```cpp
bool repeatedSubstringPattern(string s) {
    string p = s + s;
    // 去掉首尾（避免完全匹配）
    p = p.substr(1, p.size()-2);
    // KMP找needle在haystack中
    auto kmp = [](const string& text, const string& pat) {
        vector<int> pi(pat.size(), 0);
        for (int i=1; i<pat.size(); i++) {
            int j = pi[i-1];
            while (j>0 && pat[i]!=pat[j]) j = pi[j-1];
            if (pat[i]==pat[j]) j++;
            pi[i] = j;
        }
        int j=0;
        for (int i=0; i<text.size(); i++) {
            while (j>0 && text[i]!=pat[j]) j = pi[j-1];
            if (text[i]==pat[j]) j++;
            if (j==pat.size()) return true;
        }
        return false;
    };
    return kmp(p, s);
}
```

### 验证结果
| 题目 | 输入 | 输出 | 期望 |
|------|------|------|------|
| LC239 | [1,3,-1,-3,5,3,6,7], k=3 | [3,3,5,5,6,7] | ✓ |
| LC239 | [1], k=1 | [1] | ✓ |
| LC28 | haystack="hello", needle="ll" | 2 | ✓ |
| LC28 | haystack="aaaaa", needle="bba" | -1 | ✓ |
| LC28 | haystack="", needle="" | 0 | ✓ |
| LC459 | s="abab" | true | ✓ |
| LC459 | s="aba" | false | ✓ |

### 04-16 累计（含本次单调队列2题+KMP2题）
| 专题 | 题数 |
|------|------|
| Floyd判圈 | 2题 |
| LFU缓存 | 1题 |
| 图论-拓扑 | 3题 |
| 回溯-组合 | 4题 |
| BFS/DFS岛屿 | 2题 |
| 位运算 | 7题 |
| DP动态规划 | 4题 |
| 单调栈 | 4题 |
| 二叉树 | 4题 |
| 单调队列 | 2题 |
| KMP | 2题 |
| **总计** | **35题** |
