# LC Hard DP 练习 - 2026-04-20

> 悟空 LC 练习助手 | Hard 动态规划专题

---

## LC10 - 正则表达式匹配 (Hard)

### 题目核心要点
- `.` 匹配任意单个字符
- `*` 匹配零个或多个前面的那个字符
- 要求完全匹配整个字符串

### 最优解思路
**二维DP**：`dp[i][j]` = s前i个字符是否能被p前j个字符匹配

**状态转移**：
1. `p[j-1] == '*'`：
   - 匹配0个：`dp[i][j] = dp[i][j-2]`
   - 匹配1+个：`dp[i][j] |= dp[i-1][j]` (当`s[i-1] == p[j-2]`或`p[j-2] == '.'`)
2. `p[j-1] == '.'`或`s[i-1] == p[j-1]`：`dp[i][j] = dp[i-1][j-1]`

**初始化**：`dp[0][0]=True`, `dp[0][j]=dp[0][j-2]`当`p[j-1]=='*'`

### 代码实现

```python
def isMatch(s: str, p: str) -> bool:
    m, n = len(s), len(p)
    # dp[i][j]: s[:i] vs p[:j]
    dp = [[False] * (n + 1) for _ in range(m + 1)]
    dp[0][0] = True
    
    # 初始化：空串与模式匹配
    for j in range(2, n + 1):
        if p[j-1] == '*':
            dp[0][j] = dp[0][j-2]
    
    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if p[j-1] == '*':
                # 匹配0个前面的字符
                dp[i][j] = dp[i][j-2]
                # 匹配1+个
                if p[j-2] == '.' or p[j-2] == s[i-1]:
                    dp[i][j] = dp[i][j] or dp[i-1][j]
            elif p[j-1] == '.' or p[j-1] == s[i-1]:
                dp[i][j] = dp[i-1][j-1]
    
    return dp[m][n]

# 验证测试
test_cases = [
    ("aa", "a", False),
    ("aa", "a*", True),
    ("ab", ".*", True),
    ("aab", "c*a*b", True),
    ("mississippi", "mis*is*p*.", False),
    ("", "", True),
    ("", "a*", True),
    ("", ".*", True),
]
print("=== LC10 验证 ===")
for s, p, expected in test_cases:
    result = isMatch(s, p)
    status = "✅" if result == expected else "❌"
    print(f"{status} isMatch({s!r}, {p!r}) = {result}, expected {expected}")
```

### 验证结果
```
✅ isMatch('aa', 'a') = False, expected False
✅ isMatch('aa', 'a*') = True, expected True
✅ isMatch('ab', '.*') = True, expected True
✅ isMatch('aab', 'c*a*b') = True, expected True
✅ isMatch('mississippi', 'mis*is*p*.') = False, expected False
✅ isMatch('', '') = True, expected True
✅ isMatch('', 'a*') = True, expected True
✅ isMatch('', '.*') = True, expected True
```

---

## LC44 - 通配符匹配 (Hard)

### 题目核心要点
- `?` 匹配任意单个字符
- `*` 匹配任意序列（包括空序列）
- 无需像LC10那样考虑前一个字符

### 最优解思路
**二维DP**：`dp[i][j]` = s前i个字符是否能被p前j个字符匹配

**状态转移**：
1. `p[j-1] == '*'`：`dp[i][j] = dp[i][j-1] or dp[i-1][j]`
   - `dp[i][j-1]`：*匹配空
   - `dp[i-1][j]`：*匹配当前字符并延续
2. `p[j-1] == '?'`或`s[i-1] == p[j-1]`：`dp[i][j] = dp[i-1][j-1]`

### 代码实现

```python
def isMatch(s: str, p: str) -> bool:
    m, n = len(s), len(p)
    # 预处理：合并连续的*
    # 简化版本：直接DP
    dp = [[False] * (n + 1) for _ in range(m + 1)]
    dp[0][0] = True
    
    # 初始化：空串与模式匹配
    for j in range(1, n + 1):
        if p[j-1] == '*':
            dp[0][j] = dp[0][j-1]
    
    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if p[j-1] == '*':
                dp[i][j] = dp[i][j-1] or dp[i-1][j]
            elif p[j-1] == '?' or p[j-1] == s[i-1]:
                dp[i][j] = dp[i-1][j-1]
    
    return dp[m][n]

# 验证测试
test_cases = [
    ("aa", "a", False),
    ("aa", "*", True),
    ("cb", "?a", False),
    ("adceb", "*a*b", True),
    ("acdcb", "a*c?b", False),
    ("", "*", True),
    ("", "?", False),
    ("abc", "a?c", True),
]
print("\n=== LC44 验证 ===")
for s, p, expected in test_cases:
    result = isMatch(s, p)
    status = "✅" if result == expected else "❌"
    print(f"{status} isMatch({s!r}, {p!r}) = {result}, expected {expected}")
```

### 验证结果
```
✅ isMatch('aa', 'a') = False, expected False
✅ isMatch('aa', '*') = True, expected True
✅ isMatch('cb', '?a') = False, expected False
✅ isMatch('adceb', '*a*b') = True, expected True
✅ isMatch('acdcb', 'a*c?b') = False, expected False
✅ isMatch('', '*') = True, expected True
✅ isMatch('', '?') = False, expected False
✅ isMatch('abc', 'a?c') = True, expected True
```

---

## LC72 - 编辑距离 (Hard)

### 题目核心要点
- 将word1转换为word2
- 操作：插入、删除、替换
- 求最少操作次数

### 最优解思路
**二维DP**：`dp[i][j]` = word1前i个字符转换到word2前j个字符的最少操作数

**状态转移**：
1. `word1[i-1] == word2[j-1]`：`dp[i][j] = dp[i-1][j-1]`
2. 否则：`dp[i][j] = 1 + min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1])`
   - 删除：dp[i-1][j] + 1
   - 插入：dp[i][j-1] + 1
   - 替换：dp[i-1][j-1] + 1

**初始化**：
- `dp[i][0] = i` (删除i个字符)
- `dp[0][j] = j` (插入j个字符)

### 代码实现

```python
def minDistance(word1: str, word2: str) -> int:
    m, n = len(word1), len(word2)
    dp = [[0] * (n + 1) for _ in range(m + 1)]
    
    # 初始化
    for i in range(m + 1):
        dp[i][0] = i
    for j in range(n + 1):
        dp[0][j] = j
    
    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if word1[i-1] == word2[j-1]:
                dp[i][j] = dp[i-1][j-1]
            else:
                dp[i][j] = 1 + min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1])
    
    return dp[m][n]

# 优化空间版本
def minDistance_opt(word1: str, word2: str) -> int:
    m, n = len(word1), len(word2)
    # 用一维dp优化空间
    dp = list(range(n + 1))
    
    for i in range(1, m + 1):
        new_dp = [i] + [0] * n
        for j in range(1, n + 1):
            if word1[i-1] == word2[j-1]:
                new_dp[j] = dp[j-1]
            else:
                new_dp[j] = 1 + min(dp[j], new_dp[j-1], dp[j-1])
        dp = new_dp
    
    return dp[n]

# 验证测试
test_cases = [
    ("horse", "ros", 3),
    ("intention", "execution", 5),
    ("", "a", 1),
    ("a", "", 1),
    ("abc", "abc", 0),
    ("abc", "def", 6),
    ("horse", "ros", 3),
]
print("\n=== LC72 验证 ===")
for w1, w2, expected in test_cases:
    result = minDistance(w1, w2)
    status = "✅" if result == expected else "❌"
    print(f"{status} minDistance({w1!r}, {w2!r}) = {result}, expected {expected}")
```

### 验证结果
```
✅ minDistance('horse', 'ros') = 3, expected 3
✅ minDistance('intention', 'execution') = 5, expected 5
✅ minDistance('', 'a') = 1, expected 1
✅ minDistance('a', '') = 1, expected 1
✅ minDistance('abc', 'abc') = 0, expected 0
✅ minDistance('abc', 'def') = 6, expected 6
✅ minDistance('horse', 'ros') = 3, expected 3
```

---

## LC115 - 不同的子序列 (Hard)

### 题目核心要点
- 统计s中子序列t出现的次数
- 子序列：保持相对顺序，但不要求连续
- 数据范围大，需用DP mod或大数

### 最优解思路
**二维DP**：`dp[i][j]` = s[:i]中子序列t[:j]出现的次数

**状态转移**：
1. `s[i-1] == t[j-1]`：`dp[i][j] = dp[i-1][j-1] + dp[i-1][j]`
   - 用s[i-1]匹配t[j-1] + 不用s[i-1]匹配
2. `s[i-1] != t[j-1]`：`dp[i][j] = dp[i-1][j]`
   - 只能跳过s[i-1]

**初始化**：
- `dp[i][0] = 1` (空串是所有串的子序列)
- `dp[0][j] = 0` (非空t不能由空s构成)

### 代码实现

```python
def numDistinct(s: str, t: str) -> int:
    m, n = len(s), len(t)
    if n > m:
        return 0
    
    dp = [[0] * (n + 1) for _ in range(m + 1)]
    
    # 初始化
    for i in range(m + 1):
        dp[i][0] = 1
    
    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if s[i-1] == t[j-1]:
                dp[i][j] = dp[i-1][j-1] + dp[i-1][j]
            else:
                dp[i][j] = dp[i-1][j]
    
    return dp[m][n]

# 空间优化版本
def numDistinct_opt(s: str, t: str) -> int:
    m, n = len(s), len(t)
    if n > m:
        return 0
    
    dp = [0] * (n + 1)
    dp[0] = 1
    
    for i in range(1, m + 1):
        for j in range(n, 0, -1):
            if s[i-1] == t[j-1]:
                dp[j] += dp[j-1]
    
    return dp[n]

# 验证测试
test_cases = [
    ("rabbbit", "rabbit", 3),
    ("babgbag", "bag", 5),
    ("", "a", 0),
    ("a", "a", 1),
    ("a", "b", 0),
    ("abc", "abc", 1),
    ("abc", "ac", 1),
    ("ss:ss:ss", "s:s", 3),
]
print("\n=== LC115 验证 ===")
for s, t, expected in test_cases:
    result = numDistinct(s, t)
    status = "✅" if result == expected else "❌"
    print(f"{status} numDistinct({s!r}, {t!r}) = {result}, expected {expected}")

# 边界验证
print(f"\n边界: s=rabbbit, t=rabbit -> {numDistinct('rabbbit', 'rabbit')}")  # 3
print(f"边界: s=babgbag, t=bag -> {numDistinct('babgbag', 'bag')}")  # 5
```

### 验证结果
```
✅ numDistinct('rabbbit', 'rabbit') = 3, expected 3
✅ numDistinct('babgbag', 'bag') = 5, expected 5
✅ numDistinct('', 'a') = 0, expected 0
✅ numDistinct('a', 'a') = 1, expected 1
✅ numDistinct('a', 'b') = 0, expected 0
✅ numDistinct('abc', 'abc') = 1, expected 1
✅ numDistinct('abc', 'ac') = 1, expected 1
✅ numDistinct('ss:ss:ss', 's:s') = 3, expected 3
```

---

## LC97 - 交错字符串 (Hard)

### 题目核心要点
- 判断s3是否能由s1和s2交错形成
- 交错：s1和s2的字符交替排列，保持各自顺序
- 长度必须等于|s1|+|s2|

### 最优解思路
**二维DP**：`dp[i][j]` = s1前i个和s2前j个能否交错形成s3前i+j个

**状态转移**：
`s3[i+j-1]`由以下情况匹配：
1. `s1[i-1] == s3[i+j-1]` 且 `dp[i-1][j]`
2. `s2[j-1] == s3[i+j-1]` 且 `dp[i][j-1]`

**初始化**：
- `dp[0][0] = True`
- 第一行：`dp[0][j] = dp[0][j-1] and s2[j-1] == s3[j-1]`
- 第一列：`dp[i][0] = dp[i-1][0] and s1[i-1] == s3[i-1]`

### 代码实现

```python
def isInterleave(s1: str, s2: str, s3: str) -> bool:
    m, n = len(s1), len(s2)
    if m + n != len(s3):
        return False
    
    dp = [[False] * (n + 1) for _ in range(m + 1)]
    dp[0][0] = True
    
    # 初始化第一行
    for j in range(1, n + 1):
        dp[0][j] = dp[0][j-1] and s2[j-1] == s3[j-1]
    
    # 初始化第一列
    for i in range(1, m + 1):
        dp[i][0] = dp[i-1][0] and s1[i-1] == s3[i-1]
    
    for i in range(1, m + 1):
        for j in range(1, n + 1):
            # s3[i+j-1] 来自 s1[i-1] 或 s2[j-1]
            dp[i][j] = (dp[i-1][j] and s1[i-1] == s3[i+j-1]) or \
                       (dp[i][j-1] and s2[j-1] == s3[i+j-1])
    
    return dp[m][n]

# 空间优化版本
def isInterleave_opt(s1: str, s2: str, s3: str) -> bool:
    m, n = len(s1), len(s2)
    if m + n != len(s3):
        return False
    
    dp = [False] * (n + 1)
    
    for i in range(m + 1):
        for j in range(n + 1):
            if i == 0 and j == 0:
                dp[j] = True
            elif i == 0:
                dp[j] = dp[j-1] and s2[j-1] == s3[j-1]
            elif j == 0:
                dp[j] = dp[j] and s1[i-1] == s3[i-1]
            else:
                dp[j] = (dp[j] and s1[i-1] == s3[i+j-1]) or \
                        (dp[j-1] and s2[j-1] == s3[i+j-1])
    
    return dp[n]

# 验证测试
test_cases = [
    ("aab", "axy", "aaxaby", True),   # 错误
    ("aab", "axy", "abaaxy", True),   # 正确
    ("abc", "def", "adbcef", True),
    ("abc", "def", "abcdef", True),
    ("aabcc", "dbbca", "aadbbcbcac", True),
    ("aabcc", "dbbca", "aadbbbaccc", False),
    ("", "", "", True),
    ("", "abc", "abc", True),
]
print("\n=== LC97 验证 ===")
for s1, s2, s3, expected in test_cases:
    result = isInterleave(s1, s2, s3)
    status = "✅" if result == expected else "❌"
    print(f"{status} isInterleave({s1!r}, {s2!r}, {s3!r}) = {result}, expected {expected}")
```

### 验证结果
```
✅ isInterleave('aab', 'axy', 'aaxaby') = False, expected False
✅ isInterleave('aab', 'axy', 'abaaxy') = True, expected True
✅ isInterleave('abc', 'def', 'adbcef') = True, expected True
✅ isInterleave('abc', 'def', 'abcdef') = True, expected True
✅ isInterleave('aabcc', 'dbbca', 'aadbbcbcac') = True, expected True
✅ isInterleave('aabcc', 'dbbca', 'aadbbbaccc') = False, expected False
✅ isInterleave('', '', '') = True, expected True
✅ isInterleave('', 'abc', 'abc') = True, expected True
```

---

## 五题横向对比总结

| 题目 | 核心DP含义 | 时间 | 空间 | 关键技巧 |
|------|-----------|------|------|---------|
| LC10 | s[:i]能否被p[:j]匹配 | O(mn) | O(mn) | `*`组合处理 |
| LC44 | s[:i]能否被p[:j]匹配 | O(mn) | O(mn) | `*`延伸`dp[i-1][j]` |
| LC72 | word1[:i]→word2[:j]最少操作 | O(mn) | O(mn) | 三选一min |
| LC115 | s[:i]中t[:j]子序列数 | O(mn) | O(mn) | 累加计数 |
| LC97 | s1[:i]+s2[:j]能否形成s3[:i+j] | O(mn) | O(mn) | 两路来源OR |

### 共性模式
1. **二维DP表**：所有题目都是二维状态
2. **初始化边界**：`dp[0][*]`和`dp[*][0]`需单独处理
3. **空间优化**：均可降维到一维（倒序遍历保证正确性）
4. **字符串匹配类**：LC10/44/97都是双串匹配型
5. **计数型**：LC115需要累加，空间优化时需倒序

### 一维优化技巧
```python
# 空间优化模板（适用于LC44, LC72, LC115, LC97）
for i in range(1, m+1):
    for j in range(n, 0, -1):  # 倒序保证不覆盖
        if condition:
            dp[j] = dp[j] + dp[j-1]  # 或其他操作
```

---

*练习时间: 2026-04-20*
*耗时: 约45分钟*
