# LeetCode 动态规划之数位DP综合专题

> 学习日期: 2026-04-19 | 类型: 算法专题 | 难度: Hard

---

## 一、数位DP通用模板

### 1.1 核心思想

数位DP（Digit DP）又称"按位DP"，用于解决**在[L, R]范围内满足某种数字特征的数量问题**。核心思想：

1. 将数字`n`转为字符串/数组 `digits[]`
2. 从高位到低位DFS/记忆化搜索
3. 通过`tight`标志控制是否受上界限制，通过`lead`标志处理前导零
4. 利用记忆化避免重复计算

### 1.2 通用框架（Python）

```python
from functools import lru_cache

def digit_dp(n: int) -> int:
    s = str(n)
    @lru_cache(None)
    def dfs(pos: int, tight: bool, lead: bool, ...) -> int:
        """
        pos: 当前处理到的位数（从高位到低位，0=最高位）
        tight: 是否受到上界限制（True=当前位不能超过s[pos]）
        lead: 是否有前导零（True=还没填过非零位）
        """
        if pos == len(s):
            return 1  # 递归终止，满足条件的计数+1

        limit = int(s[pos]) if tight else 9
        res = 0
        for digit in range(0, limit + 1):
            new_tight = tight and (digit == limit)
            new_lead = lead and (digit == 0)
            # 根据题目条件判断当前digit是否合法
            if is_valid(digit, state):
                res += dfs(pos + 1, new_tight, new_lead, new_state)
        return res

    return dfs(0, True, True, ...)
```

### 1.3 状态设计原则

| 维度 | 说明 |
|------|------|
| `pos` | 当前位数，必选 |
| `tight` | 是否受上界约束，必选 |
| `lead` | 是否有前导零，需看题意 |
| `state` | 题目相关状态（出现次数、奇偶性、模值等） |

**记忆化注意事项**：
- `tight` 和 `lead` 必须作为记忆化参数
- 状态维度不要过多（一般≤3），维度多则考虑状态压缩

---

## 二、经典问题详解

---

### 2.1 LC233 - 数字 1 的个数（计数问题）

**题目链接**: https://leetcode.cn/problems/number-of-digit-one/
**难度**: Hard ⭐⭐⭐
**Tags**: 数位DP | 计数问题

#### 核心思路

统计`[0, n]`范围内所有数字中`1`出现的总次数。核心按位分析：

- **高位权重**：`factor = 10^(len-1)`
- 当前位`digit = n // factor % 10`
- 高位部分 `higher = n // (factor * 10)`
- 低位部分 `lower = n % factor`

```
当前位对答案的贡献：
- digit == 0: higher * factor
- digit == 1: higher * factor + lower + 1
- digit > 1: (higher + 1) * factor
```

#### 关键代码

```python
class Solution:
    def countDigitOne(self, n: int) -> int:
        """按位分析，O(log10(n))"""
        if n <= 0:
            return 0
        ans = 0
        factor = 1  # 10^0, 10^1, 10^2, ...
        while factor <= n:
            # 分解n的当前位
            lower = n % factor           # 低位部分
            cur = (n // factor) % 10     # 当前位
            higher = n // (factor * 10) # 高位部分

            if cur == 0:
                ans += higher * factor
            elif cur == 1:
                ans += higher * factor + lower + 1
            else:
                ans += (higher + 1) * factor

            factor *= 10
        return ans

    def countDigitOne_dp(self, n: int) -> int:
        """数位DP通用模板"""
        if n <= 0:
            return 0
        s = str(n)
        @lru_cache(None)
        def dfs(pos: int, tight: bool, lead: bool, count: int) -> int:
            """统计[0,n]范围内恰好有count个1的数字个数"""
            if pos == len(s):
                return 1 if count > 0 else 0

            limit = int(s[pos]) if tight else 9
            res = 0
            for d in range(0, limit + 1):
                new_tight = tight and (d == limit)
                new_lead = lead and (d == 0)
                if new_lead:
                    # 仍是前导零，不计入1的个数
                    res += dfs(pos + 1, new_tight, new_lead, 0)
                else:
                    res += dfs(pos + 1, new_tight, new_lead, count + (1 if d == 1 else 0))
            return res

        @lru_cache(None)
        def total_ones(pos: int, tight: bool, lead: bool, curr_count: int) -> int:
            """返回从pos位到末尾，1的个数的总和"""
            if pos == len(s):
                return curr_count

            limit = int(s[pos]) if tight else 9
            res = 0
            for d in range(0, limit + 1):
                new_tight = tight and (d == limit)
                new_lead = lead and (d == 0)
                if new_lead:
                    res += total_ones(pos + 1, new_tight, new_lead, 0)
                else:
                    res += total_ones(pos + 1, new_tight, new_lead,
                                     curr_count + (1 if d == 1 else 0))
            return res

        return total_ones(0, True, True, 0)
```

#### 复杂度分析

| 方法 | 时间复杂度 | 空间复杂度 |
|------|-----------|-----------|
| 按位分析 | O(log₁₀ n) | O(1) |
| 数位DP | O(log₁₀ n × 状态数) | O(log₁₀ n × 状态数) |

---

### 2.2 LC902 - 数字范围枚数（数位DP通用模板）

**题目链接**: https://leetcode.cn/problems/numbers-at-most-n-given-digit-properties/
**难度**: Hard ⭐⭐⭐
**Tags**: 数位DP | 状态压缩

#### 核心思路

统计`[1, n]`范围内，**各位数字之和不超过`k`** 的数字个数。

- 状态：`pos, tight, lead, sum`（当前已填位的数字之和）
- 记忆化：`dp[pos][tight][lead][sum]` → 方法数
- 剪枝：若`sum > k`，直接返回0

#### 关键代码

```python
class Solution:
    def atMostNGivenDigitSet(self, digits: List[str], n: int) -> int:
        """
        统计用digits中的数字能组成多少<=n的数
        这道题是"digits限制"的数位DP，与标准数位DP略有不同
        """
        s = str(n)
        D = set(int(d) for d in digits)

        @lru_cache(None)
        def dfs(pos: int, tight: bool, lead: bool) -> int:
            """返回从pos位开始的合法方法数"""
            if pos == len(s):
                return 1  # 填完一位，合法计数+1

            if tight:
                limit = int(s[pos])
            else:
                limit = 9

            res = 0
            for d in range(0, limit + 1):
                new_tight = tight and (d == limit)
                new_lead = lead and (d == 0)

                if d not in D:
                    continue  # 当前digit不在允许集合中

                if new_lead:
                    # 前导零：占位但不消耗位数，不影响答案
                    res += dfs(pos + 1, new_tight, new_lead)
                else:
                    res += dfs(pos + 1, new_tight, new_lead)
            return res

        return dfs(0, True, True) - 1  # 减去全0的情况（n>=0时）

    def atMostNGivenDigitSet_v2(self, digits: List[str], n: int) -> int:
        """
        另一种思路：按数字长度分类
        - 长度 < len(n): 所有组合都合法
        - 长度 == len(n): 数位DP
        """
        s = str(n)
        L = len(s)
        D = [int(d) for d in digits]
        D_set = set(D)

        # 1. 长度小于L的，全部合法
        ans = 0
        for length in range(1, L):
            ans += len(D) ** length

        # 2. 长度等于L的，数位DP
        @lru_cache(None)
        def dfs(pos: int, tight: bool) -> int:
            if pos == L:
                return 1

            limit = int(s[pos]) if tight else 9
            res = 0
            for d in D_set:
                if d > limit:
                    continue
                res += dfs(pos + 1, tight and d == limit)
            return res

        ans += dfs(0, True)
        return ans
```

#### 复杂度分析

| 指标 | 值 |
|------|---|
| 时间复杂度 | O(len(n) × |digits| × k) |
| 空间复杂度 | O(len(n) × 2 × 2 × (k+1)) |

---

### 2.3 LC600 - 不含连续1的非负整数（特殊数字/状态压缩）

**题目链接**: https://leetcode.cn/problems/non-negative-integers-without-consecutive-ones/
**难度**: Hard ⭐⭐⭐
**Tags**: 数位DP | 状态压缩 | 二进制

#### 核心思路

统计`[0, n]`范围内，**二进制表示不含连续1** 的数字个数。

- 将`n`转为二进制字符串
- 状态：`pos, prev_one`（前一位是否为1）
- 转移：若前一位是1，当前位只能填0

#### 关键代码

```python
class Solution:
    def findIntegers(self, n: int) -> int:
        """
        二进制数位DP：不含连续1
        dp[pos][tight][prev_one]
        """
        s = bin(n)[2:]  # 去掉'0b'
        L = len(s)

        @lru_cache(None)
        def dfs(pos: int, tight: bool, prev_one: bool) -> int:
            """返回从pos位开始的合法方案数"""
            if pos == L:
                return 1

            limit = int(s[pos]) if tight else 1
            res = 0
            for bit in range(0, limit + 1):
                new_tight = tight and (bit == limit)
                if prev_one and bit == 1:
                    continue  # 连续1，禁止
                res += dfs(pos + 1, new_tight, bit == 1)
            return res

        return dfs(0, True, False)

    def findIntegers_fib(self, n: int) -> int:
        """
        非数位DP方法：预计算斐波那契数列
        规律：k位二进制不含连续1的数个数 = Fib(k+2)
        """
        # 预计算Fib
        fib = [0] * 32
        fib[0], fib[1] = 0, 1
        for i in range(2, 32):
            fib[i] = fib[i-1] + fib[i-2]

        # 找到最高位
        k = n.bit_length()

        # 预处理：所有小于n的合法数
        ans = 0
        for i in range(k):
            ans += fib[i + 2]  # i+1位二进制中不含连续1的个数

        # 从高位到低位逐位检查
        prev_bit = 0
        for i in range(k - 1, -1, -1):
            cur_bit = (n >> i) & 1
            if cur_bit == 1:
                ans += fib[i + 1]  # 这一位填0，后面的随便选
            if prev_bit == 1 and cur_bit == 1:
                break  # 出现连续1，后面的都不合法了
            prev_bit = cur_bit

        return ans + 1  # 加上n=0的情况
```

#### 复杂度分析

| 方法 | 时间复杂度 | 空间复杂度 |
|------|-----------|-----------|
| 数位DP | O(bit_length(n)) | O(bit_length(n)) |
| 斐波那契 | O(bit_length(n)) | O(1) |

---

### 2.4 LC1012 - 至少有1位重复的数字（容斥原理）

**题目链接**: https://leetcode.cn/problems/numbers-with-repeated-digits/
**难度**: Hard ⭐⭐⭐
**Tags**: 数位DP | 容斥原理 | 排列组合

#### 核心思路

统计`[1, n]`范围内，**至少有1位数字重复** 的数字个数。

**补集思想**：`结果 = 总数 - 没有重复的数`

- 先求`[1, n]`中所有数的个数 = `n`
- 再求`[1, n]`中**没有重复数字**的个数（用排列组合）
- 最后：`有重复 = n - 没有重复`

#### 关键代码

```python
class Solution:
    def numDupDigitsAtMostN(self, n: int) -> int:
        """
        补集思想：至少有1位重复 = 总数 - 各数位都不重复
        """
        s = list(map(int, str(n + 1)))  # 转为[0, n]闭区间
        L = len(s)

        # ---------- Part1: 长度小于L的 ----------
        # k位数（首位不为0），各数位都不重复的个数
        ans = 0
        for k in range(1, L):
            ans += 9 * self._perm(9, k - 1)  # 首位9选1，后面k-1位从剩下9个选

        # ---------- Part2: 长度等于L的 ----------
        # 用集合记录已经使用过的数字
        seen = set()
        for i, digit in enumerate(s):
            for d in range(0, 10):
                if i == 0 and d == 0:
                    continue  # 首位不能为0
                if d in seen:
                    continue
                if d < digit:
                    # 填一个比digit小的数，后面随便选
                    remaining = L - i - 1
                    ans += self._perm(10 - (i + 1), remaining)
                else:  # d == digit
                    seen.add(d)
                    break

        return ans  # 这就是"至少有1位重复"的个数

    def _perm(self, m: int, k: int) -> int:
        """计算 P(m, k) = m! / (m-k)!"""
        if k == 0:
            return 1
        res = 1
        for i in range(m, m - k, -1):
            res *= i
        return res
```

#### 复杂度分析

| 指标 | 值 |
|------|---|
| 时间复杂度 | O(len(n) × 10) |
| 空间复杂度 | O(10) |

---

### 2.5 LC788 - 旋转数字（特殊数字判定）

**题目链接**: https://leetcode.cn/problems/rotated-digits/
**难度**: Medium ⭐⭐
**Tags**: 数位DP | 特殊数字

#### 核心思路

统计`[1, n]`范围内，**旋转后与原数不同且仍在0-9范围内** 的数字个数。

**旋转规则**：
```
0 -> 0, 1 -> 1, 2 -> 5, 5 -> 2
6 -> 9, 9 -> 6, 8 -> 8
3, 4, 7 旋转后无效
```

- 状态：`pos, tight, lead, changed`（是否至少有一位发生了旋转）
- 有效数字：至少有一位旋转了，且没有无效位

#### 关键代码

```python
class Solution:
    def rotatedDigits(self, n: int) -> int:
        """
        数位DP：旋转数字
        good_digits = {0, 1, 2, 5, 6, 8, 9}
        bad_digits = {3, 4, 7}  # 旋转后无效
        rotatable = {2, 5, 6, 9}  # 旋转后与原数不同
        """
        s = str(n)
        valid = {0, 1, 2, 5, 6, 8, 9}
        rotatable = {2, 5, 6, 9}

        @lru_cache(None)
        def dfs(pos: int, tight: bool, lead: bool, changed: bool) -> int:
            if pos == len(s):
                return 1 if changed else 0  # 至少有一位旋转了

            limit = int(s[pos]) if tight else 9
            res = 0
            for d in range(0, limit + 1):
                new_tight = tight and (d == limit)
                new_lead = lead and (d == 0)
                if d not in valid:
                    continue  # 包含无效位3,4,7，跳过
                new_changed = changed or (d in rotatable)
                res += dfs(pos + 1, new_tight, new_lead, new_changed)
            return res

        return dfs(0, True, True, False)
```

#### 复杂度分析

| 指标 | 值 |
|------|---|
| 时间复杂度 | O(len(n) × 10 × 2) |
| 空间复杂度 | O(len(n) × 2 × 2 × 2) |

---

### 2.6 LC1397 - 找到所有好字符串（数位DP + KMP/记忆化）

**题目链接**: https://leetcode.cn/problems/find-all-good-strings/
**难度**: Hard ⭐⭐⭐
**Tags**: 数位DP | 字符串 | KMP

#### 核心思路

统计长度等于`len(s1)=len(s2)=n`的数字字符串数量：
- 在`[s1, s2]`范围内
- 不包含evil字符串作为子串

**核心技巧**：
- 状态：`pos, tight_low, tight_high, matched`（已匹配到evil的前缀长度）
- 用KMP预处理`next`数组，在DFS过程中动态更新`matched`

#### 关键代码

```python
class Solution:
    def findGoodStrings(self, n: int, s1: str, s2: str, evil: str) -> int:
        """
        数位DP + KMP：求在[s1,s2]范围内不包含evil的所有字符串
        状态：pos, tight_low, tight_high, matched(已匹配到evil前缀长度)
        """
        if s1 > s2:
            return 0

        MOD = 10**9 + 7

        # ---------- KMP预处理：next[i] = 当匹配到evil[i]时，下一个状态 ----------
        def build_kmp(pattern: str) -> List[int]:
            n = len(pattern)
            next_arr = [0] * n
            j = 0
            for i in range(1, n):
                while j > 0 and pattern[i] != pattern[j]:
                    j = next_arr[j - 1]
                if pattern[i] == pattern[j]:
                    j += 1
                next_arr[i] = j
            return next_arr

        next_arr = build_kmp(evil)
        s1_list = list(s1)
        s2_list = list(s2)

        @lru_cache(None)
        def dfs(pos: int, low_tight: bool, high_tight: bool, matched: int) -> int:
            """返回从pos位开始的合法字符串个数"""
            if matched == len(evil):
                return 0  # 已经匹配到evil，说明出现了evil子串，不合法
            if pos == n:
                return 1  # 填完n位，合法

            low_limit = ord(s1_list[pos]) if low_tight else ord('a')
            high_limit = ord(s2_list[pos]) if high_tight else ord('z')
            res = 0

            for code in range(low_limit, high_limit + 1):
                c = chr(code)
                new_low = low_tight and (code == low_limit)
                new_high = high_tight and (code == high_limit)

                # 计算新的matched（加入字符c后匹配到evil的前缀长度）
                new_matched = matched
                while new_matched > 0 and c != evil[new_matched]:
                    new_matched = next_arr[new_matched - 1]
                if c == evil[new_matched]:
                    new_matched += 1

                res += dfs(pos + 1, new_low, new_high, new_matched)

            return res % MOD

        return dfs(0, True, True, 0)
```

#### 复杂度分析

| 指标 | 值 |
|------|---|
| 时间复杂度 | O(n × 26 × len(evil) × 状态数) |
| 空间复杂度 | O(n × 状态数 × len(evil)) |

---

### 2.7 LC2376 - 统计特殊数字数目（Hard约束数位DP）

**题目链接**: https://leetcode.cn/problems/count-special-numbers/
**难度**: Hard ⭐⭐⭐
**Tags**: 数位DP | 集合 | 去重

#### 核心思路

统计`[1, n]`范围内，**任意两个相邻数位没有相同** 的数字个数。

- `n ≤ 10^10`，数字位数最多10位
- 状态：`pos, tight, lead, used_mask`（使用过的数字位掩码）
- `used_mask`用10位掩码表示`{0-9}`的使用情况

#### 关键代码

```python
class Solution:
    def countSpecialNumbers(self, n: int) -> int:
        """
        统计[1,n]范围内任意两个相邻数位都不相同的数字个数
        状态：pos, tight, lead, used_mask（10位掩码）
        """
        s = str(n)
        L = len(s)

        @lru_cache(None)
        def dfs(pos: int, tight: bool, lead: bool, mask: int) -> int:
            if pos == L:
                # lead=True 表示填的全是前导零，即数字0，不计数
                return 0 if lead else 1

            limit = int(s[pos]) if tight else 9
            res = 0
            for d in range(0, limit + 1):
                new_tight = tight and (d == limit)
                new_lead = lead and (d == 0)

                if new_lead:
                    # 继续前导零，不占用mask
                    res += dfs(pos + 1, new_tight, new_lead, mask)
                else:
                    bit = 1 << d
                    if mask & bit:
                        continue  # d已在高位使用过，相邻重复
                    res += dfs(pos + 1, new_tight, new_lead, mask | bit)
            return res

        return dfs(0, True, True, 0)

    def countSpecialNumbers_formula(self, n: int) -> int:
        """
        数学公式方法：按数字长度分类讨论
        """
        s = str(n)
        L = len(s)
        MOD = 10**9 + 7

        # part1: 长度小于L
        ans = 0
        for k in range(1, L):
            if k == 1:
                ans += 9  # 1位数：1-9
            else:
                # k位数：首位9选1，后面k-1位从剩余9个选（相邻不同）
                ans += 9 * self._perm(9, k - 1)

        # part2: 长度等于L
        seen = set()
        for i, ch in enumerate(s):
            digit = int(ch)
            for d in range(0, digit):
                if i == 0 and d == 0:
                    continue
                if d in seen:
                    continue
                remaining = L - i - 1
                ans += self._perm(10 - (len(seen) + 1), remaining)
            if digit in seen:
                break
            seen.add(digit)
        else:
            # 完整遍历完，没有break，说明n本身就是特殊数字
            ans += 1

        return ans

    def _perm(self, m: int, k: int) -> int:
        if k == 0:
            return 1
        res = 1
        for i in range(m, m - k, -1):
            res *= i
        return res
```

#### 复杂度分析

| 方法 | 时间复杂度 | 空间复杂度 |
|------|-----------|-----------|
| 数位DP | O(len(n) × 10 × 2^10) | O(len(n) × 2^10) |
| 数学公式 | O(len(n) × 10) | O(10) |

---

## 三、模板总结

### 3.1 数位DP四要素

```
┌─────────────────────────────────────────────────┐
│                  数位DP 框架                     │
├─────────────────────────────────────────────────┤
│  1. 状态设计：pos + tight + lead + 题目状态       │
│  2. 递归出口：pos == len(s) → 统计答案            │
│  3. 剪枝优化：提前过滤不合法状态（如sum超限）       │
│  4. 记忆化：tight=0时的结果可复用                │
└─────────────────────────────────────────────────┘
```

### 3.2 前导零处理

```python
# 前导零的三种处理方式
if new_lead:
    # 方式1: 跳过，继续填下一位（不计入状态）
    dfs(pos+1, ...)
    # 方式2: 计入状态，但不影响答案
    dfs(pos+1, ..., state=0)
    # 方式3: 直接从pos+1开始（去除前导零的影响）
    skip_leading_zero()
```

### 3.3 状态压缩技巧

| 题目特征 | 状态压缩方式 |
|---------|-------------|
| 出现次数 | 直接计数`count` |
| 相邻不同 | 位掩码`mask`（≤10位用int） |
| 模值/求和 | 模运算`mod_sum %= MOD` |
| 已匹配前缀 | KMP的`next`数组跳转 |

---

## 四、练习建议

### 必做题目（按难度排序）

| 顺序 | 题目 | 难度 | 重点 |
|------|------|------|------|
| 1 | LC788 旋转数字 | Medium | 前导零+changed标志 |
| 2 | LC902 数字范围枚数 | Hard | digits限制模板 |
| 3 | LC600 不含连续1 | Hard | 二进制数位DP |
| 4 | LC233 数字1的个数 | Hard | 按位分析+数位DP双解 |
| 5 | LC1012 数字重复 | Hard | 补集+排列组合 |
| 6 | LC2376 特殊数字 | Hard | 状态压缩到位掩码 |
| 7 | LC1397 好字符串 | Hard | 数位DP+KMP联合 |

### 延伸题目

- LC1067: 范围内的数字计数
- LC357: 计算各个位数不同的数字个数
- LC1201: 丑数 III（数位DP + 容斥）

---

*本文由悟空整理 | 2026-04-19*
