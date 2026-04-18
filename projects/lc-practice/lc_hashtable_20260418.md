# LC练习 - 哈希表专题 | 2026-04-18 10:20

## 专题: 哈希表（Hash Table）

### 哈希表核心操作
- **查询**: O(1) 平均，O(n) 最坏（哈希冲突）
- **去重**: 用unordered_set
- **计数**: 用unordered_map
- **两数和**: 一遍哈希

---

## LC1 两数之和 (Easy)
**问题**: 数组中找两数和为目标值，返回索引
**核心**: 一遍哈希，边走边存

```cpp
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int,int> mp;
    for (int i=0; i<nums.size(); i++) {
        int complement = target - nums[i];
        if (mp.count(complement)) return {mp[complement], i};
        mp[nums[i]] = i;
    }
    return {};
}
```

**验证**: target=9, [2,7,11,15]→{0,1} ✅
**边界**: 无解返回空

---

## LC128 最长连续序列 (Hard)
**问题**: 最长连续序列长度（O(n)要求）
**核心**: 用哈希集去重 + 枚举起点

```cpp
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> s(nums.begin(), nums.end());
    int ans = 0;
    for (int x : s) {
        if (s.count(x-1)) continue; // 只从最小开始
        int y = x;
        while (s.count(y)) y++;
        ans = max(ans, y-x);
    }
    return ans;
}
```

**验证**: [100,4,200,1,3,2]→4 ✅
**核心**: 跳过已经在序列中的元素

---

## LC349 两个数组的交集 (Easy)
**问题**: 返回交集（去重）
**核心**: unordered_set 交集

```cpp
vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
    unordered_set<int> s(nums1.begin(), nums1.end());
    unordered_set<int> res;
    for (int x : nums2) if (s.count(x)) res.insert(x);
    return vector<int>(res.begin(), res.end());
}
```

---

## LC560 和为K的子数组 (Medium)
**问题**: 连续子数组和为k的个数
**核心**: 前缀和 + 哈希表计数

```cpp
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int,int> cnt;
    cnt[0] = 1;
    int sum = 0, ans = 0;
    for (int x : nums) {
        sum += x;
        ans += cnt[sum - k];
        cnt[sum]++;
    }
    return ans;
}
```

**验证**: [1,1,1], k=2 → 2 ✅
**核心**: sum[j]-sum[i]=k ⟺ 前缀和差为k

---

## LC554 第N个奇迹 (变体)
**问题**: 哈希表+计数思维
**核心**: 典型计数题

```cpp
// LC560变体 - 统计出现次数
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int,int> mp{{0,1}};
    int sum=0, count=0;
    for (int n : nums) {
        sum += n;
        count += mp[sum - k];
        mp[sum]++;
    }
    return count;
}
```

---

## 哈希表设计要点

| 要点 | 说明 |
|------|------|
| unordered_set | 去重 + O(1)查找 |
| unordered_map | 键值对 + 计数 |
| 两数和 | 一遍哈希，边走边查 |
| 前缀和+哈希 | 区间统计（如560） |
| 连续序列 | 只从最小枚举 |

---

