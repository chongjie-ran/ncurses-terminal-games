# LeetCode 练习 - 2026-04-16 晚间

## 练习主题：二分查找变体

### 1. LC33 搜索旋转排序数组
**问题**：O(log n)时间找到旋转数组中的目标值
**核心洞察**：二分查找变体，一边有序时判断target在哪个区间

```python
def search(nums, target):
    lo, hi = 0, len(nums) - 1
    while lo <= hi:
        mid = (lo + hi) // 2
        if nums[mid] == target:
            return mid
        # 左半边有序
        if nums[lo] <= nums[mid]:
            if nums[lo] <= target < nums[mid]:
                hi = mid - 1
            else:
                lo = mid + 1
        # 右半边有序
        else:
            if nums[mid] < target <= nums[hi]:
                lo = mid + 1
            else:
                hi = mid - 1
    return -1
```

### 2. LC34 在排序数组中查找元素的第一个和最后一个位置
**核心**：两次二分，分别找左边界和右边界

```python
def findFirst(nums, target):
    lo, hi = 0, len(nums) - 1
    while lo <= hi:
        mid = (lo + hi) // 2
        if nums[mid] < target:
            lo = mid + 1
        else:
            hi = mid - 1
    return lo if lo < len(nums) and nums[lo] == target else -1

def findLast(nums, target):
    lo, hi = 0, len(nums) - 1
    while lo <= hi:
        mid = (lo + hi) // 2
        if nums[mid] > target:
            hi = mid - 1
        else:
            lo = mid + 1
    return hi if hi >= 0 and nums[hi] == target else -1
```

### 3. LC74 搜索二维矩阵
**核心**：将二维转一维索引，或从右上角开始搜索

```python
def searchMatrix(matrix, target):
    if not matrix: return False
    m, n = len(matrix), len(matrix[0])
    lo, hi = 0, m * n - 1
    while lo <= hi:
        mid = (lo + hi) // 2
        val = matrix[mid // n][mid % n]
        if val == target:
            return True
        elif val < target:
            lo = mid + 1
        else:
            hi = mid - 1
    return False
```

## 练习收获

**二分查找模板**：
- 标准：lo <= hi, lo = mid+1, hi = mid-1
- 左边界：nums[mid] < target → lo = mid+1 else hi = mid-1
- 右边界：nums[mid] > target → hi = mid-1 else lo = mid+1

**旋转数组判断**：哪边有序决定了target的位置

---

## 04-16 22:22 晚间追加 - 二分答案专题

### 4. LC875 Koko Eating Bananas
**核心**：二分答案，检验函数计算以mid速度吃完需要多少小时

```python
def minEatingSpeed(piles, h):
    lo, hi = 1, max(piles)
    while lo < hi:
        mid = lo + (hi - lo) // 2
        hours = sum((p + mid - 1) // mid for p in piles)  # ceil
        if hours <= h: hi = mid  # mid够快，可以更慢
        else: lo = mid + 1
    return lo
```
**结果**: 4 30 23 ✅

### 5. LC410 Split Array Largest Sum
**核心**：找最大子数组和的最小值，转化为"最多分k份，每份和≤mid"

```python
def splitArray(nums, k):
    lo, hi = max(nums), sum(nums)
    while lo < hi:
        mid = lo + (hi - lo) // 2
        splits, cur = 1, 0
        for n in nums:
            if cur + n > mid: splits += 1; cur = n
            else: cur += n
        if splits <= k: hi = mid  # mid够大
        else: lo = mid + 1
    return lo
```
**结果**: 18 9 ✅

### 6. LC1011 Capacity To Ship Within D Days
**核心**：与LC410完全相同模式，只是验证条件是天数≤D

```python
def shipWithinDays(weights, days):
    lo, hi = max(weights), sum(weights)
    while lo < hi:
        mid = lo + (hi - lo) // 2
        d, cur = 1, 0
        for w in weights:
            if cur + w > mid: d += 1; cur = w
            else: cur += w
        if d <= days: hi = mid
        else: lo = mid + 1
    return lo
```
**结果**: 15 6 ✅

### 二分答案模式总结
| 问题 | 验证条件 | 方向 |
|------|----------|------|
| LC875 吃香蕉 | hours ≤ h | mid够快→hi=mid |
| LC410 分数组 | splits ≤ k | mid够大→hi=mid |
| LC1011 运货 | days ≤ D | mid够大→hi=mid |

**模板**:
- lo=最小候选, hi=最大候选
- while lo < hi: mid=(lo+hi)//2
- if feasible(mid): hi=mid else lo=mid+1
