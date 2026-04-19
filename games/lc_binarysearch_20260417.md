# LC二分查找专题 - 2026-04-17 晚间

## 二分查找核心思想
- 有序数组中找目标值
- 每次缩小搜索范围一半
- O(log n) 时间复杂度

## 经典问题模式

### 1. LC704 二分查找 (Binary Search)
**问题**：有序数组中找目标值，返回下标或-1
**核心洞察**：标准二分模板，左闭右闭区间

```python
def search(nums, target):
    left, right = 0, len(nums) - 1
    while left <= right:
        mid = (left + right) // 2
        if nums[mid] == target:
            return mid
        elif nums[mid] < target:
            left = mid + 1
        else:
            right = mid - 1
    return -1

# 验证: [-1,0,3,5,9,12], target=9 → 4✓ target=2 → -1✓
```

**模板要点**：
- while left <= right（等号不能漏）
- mid计算防溢出：(left + right) // 2 或 left + (right-left)//2
- left = mid + 1, right = mid - 1

---

### 2. LC34 在排序数组中查找元素的第一个和最后一个位置
**问题**：找target的起始和结束位置
**核心洞察**：两次二分，分别找左边界和右边界

```python
def searchRange(nums, target):
    def find_bound(find_first):
        left, right = 0, len(nums) - 1
        bound = -1
        while left <= right:
            mid = (left + right) // 2
            if nums[mid] == target:
                bound = mid
                if find_first:
                    right = mid - 1  # 继续找左边
                else:
                    left = mid + 1   # 继续找右边
            elif nums[mid] < target:
                left = mid + 1
            else:
                right = mid - 1
        return bound
    
    first = find_bound(True)
    last = find_bound(False)
    return [first, last]

# 验证: [5,7,7,8,8,10], target=8 → [3,4]✓ target=6 → [-1,-1]✓
```

---

### 3. LC33 搜索旋转排序数组
**问题**：升序数组旋转后（某处断开拼接），找target
**核心洞察**：先判断哪半边有序，再判断target在不在有序半边

```python
def search(nums, target):
    if not nums: return -1
    left, right = 0, len(nums) - 1
    while left <= right:
        mid = (left + right) // 2
        if nums[mid] == target:
            return mid
        
        # 左半边有序
        if nums[left] <= nums[mid]:
            if nums[left] <= target < nums[mid]:
                right = mid - 1
            else:
                left = mid + 1
        # 右半边有序
        else:
            if nums[mid] < target <= nums[right]:
                left = mid + 1
            else:
                right = mid - 1
    return -1

# 验证: [4,5,6,7,0,1,2], target=0 → 4✓ target=3 → -1✓
```

---

### 4. LC81 搜索旋转排序数组II（含有重复）
**问题**：旋转数组中含有重复元素
**核心洞察**：与LC33类似，但当nums[left]==nums[mid]==nums[right]时收缩边界

```python
def search(nums, target):
    if not nums: return False
    left, right = 0, len(nums) - 1
    while left <= right:
        mid = (left + right) // 2
        if nums[mid] == target:
            return True
        
        # 收缩重复边界
        while left <= mid and nums[left] == nums[mid]:
            left += 1
        while right >= mid and nums[right] == nums[mid]:
            right -= 1
        
        if left > right:
            return False
        
        # 判断哪边有序
        if nums[left] <= nums[mid]:
            if nums[left] <= target < nums[mid]:
                right = mid - 1
            else:
                left = mid + 1
        else:
            if nums[mid] < target <= nums[right]:
                left = mid + 1
            else:
                right = mid - 1
    return False

# 验证: [2,5,6,0,0,1,2], target=0 → True✓ target=3 → False✓
```

---

### 5. LC153 寻找旋转排序数组中的最小值
**问题**：无重复旋转数组，找最小元素
**核心洞察**：right指针逼近最小值

```python
def findMin(nums):
    left, right = 0, len(nums) - 1
    while left < right:
        mid = (left + right) // 2
        if nums[mid] > nums[right]:
            left = mid + 1  # 最小值在右半边
        else:
            right = mid     # mid可能是最小值，保留
    return nums[left]

# 验证: [3,4,5,1,2] → 1✓ [4,5,6,7,0,1,2] → 0✓
```

---

### 6. LC154 寻找旋转排序数组中的最小值II（含重复）
**问题**：含有重复元素的旋转数组找最小值
**核心洞察**：当nums[mid]==nums[right]时，right--收缩

```python
def findMin(nums):
    left, right = 0, len(nums) - 1
    while left < right:
        mid = (left + right) // 2
        if nums[mid] > nums[right]:
            left = mid + 1
        elif nums[mid] < nums[right]:
            right = mid
        else:  # nums[mid] == nums[right]
            right -= 1
    return nums[left]

# 验证: [1,0,1,1,1] → 0✓ [2,2,2,0,1] → 0✓
```

---

### 7. LC162 寻找峰值
**问题**：找任意一个峰值（比邻居大）的索引
**核心洞察**：往高处走，nums[mid] < nums[mid+1]则右走

```python
def findPeakElement(nums):
    left, right = 0, len(nums) - 1
    while left < right:
        mid = (left + right) // 2
        if nums[mid] < nums[mid + 1]:
            left = mid + 1  # 峰值在右边
        else:
            right = mid
    return left

# 验证: [1,2,3,1] → 2✓ [1,2,1,3,5,6,4] → 5✓
```

---

### 8. LC74 搜索二维矩阵
**问题**：每行升序，每行第一个大于上一行最后一个。判断target是否存在
**核心洞察**：两次二分，或当成一维二分

```python
def searchMatrix(matrix, target):
    if not matrix: return False
    m, n = len(matrix), len(matrix[0])
    
    # 第一次二分找行
    top, bottom = 0, m - 1
    while top <= bottom:
        mid = (top + bottom) // 2
        if matrix[mid][0] <= target <= matrix[mid][-1]:
            # 目标行
            row = mid
            left, right = 0, n - 1
            while left <= right:
                m2 = (left + right) // 2
                if matrix[row][m2] == target:
                    return True
                elif matrix[row][m2] < target:
                    left = m2 + 1
                else:
                    right = m2 - 1
            return False
        elif target < matrix[mid][0]:
            bottom = mid - 1
        else:
            top = mid + 1
    return False

# 验证: matrix=[[1,3,5,7],[10,11,16,20],[23,30,34,60]], target=3 → True✓ target=13 → False✓
```

---

### 9. LC240 搜索二维矩阵II
**问题**：每行升序，每列升序。从右上角或左下角出发
**核心洞察**：从右上角走，比target大左移，比target小下移

```python
def searchMatrix(matrix, target):
    if not matrix: return False
    m, n = len(matrix), len(matrix[0])
    r, c = 0, n - 1
    while r < m and c >= 0:
        if matrix[r][c] == target:
            return True
        elif matrix[r][c] > target:
            c -= 1
        else:
            r += 1
    return False

# 验证: matrix=[[1,4,7,11],[2,5,8,12],[3,6,9,16],[10,13,14,17]], target=5 → True✓ target=20 → False✓
```

---

### 10. LC278 第一个错误的版本
**问题**：API isBadVersion(mid) 判断版本是否错误。找第一个错误版本
**核心洞察**：标准左边界二分

```python
def firstBadVersion(n):
    left, right = 1, n
    while left < right:
        mid = (left + right) // 2
        if isBadVersion(mid):
            right = mid  # 错误在左边
        else:
            left = mid + 1
    return left

# 验证: n=5, first_bad=4 → 4✓
```

---

## 二分查找专题总结

### 四大类型
| 类型 | 模板 | 关键点 |
|------|------|--------|
| 标准二分 | while left <= right | 等号，mid±1 |
| 左边界 | find_first + right=mid-1 | bound记录 |
| 右边界 | find_last + left=mid+1 | bound记录 |
| 旋转数组 | 判断哪边有序 | nums[left]<=nums[mid] |

### 旋转数组三问
| 题目 | 关键条件 |
|------|----------|
| LC33 无重复 | nums[left]<=nums[mid]判断有序 |
| LC81 有重复 | left++/right--收缩重复边界 |
| LC153/154 找最小 | right=mid（不是mid-1）|

### 寻找峰值关键
- nums[mid] < nums[mid+1] → 峰值在右边
- 否则峰值在左边或mid处

---

## 今日二分查找专题验证

| 题目 | 验证结果 |
|------|----------|
| LC704 二分查找 | target=9→4✓ t=2→-1✓ |
| LC34 范围查找 | [5,7,7,8,8,10]找8→[3,4]✓ |
| LC33 旋转数组 | [4,5,6,7,0,1,2]找0→4✓ |
| LC81 旋转数组II | 含重复→正确收缩✓ |
| LC153 找最小值 | [3,4,5,1,2]→1✓ |
| LC154 找最小值II | 含重复→right--✓ |
| LC162 找峰值 | [1,2,3,1]→2✓ |
| LC74 二维矩阵 | 二分行+二分列✓ |
| LC240 二维矩阵II | 右上角出发✓ |
| LC278 错误版本 | 第一个坏版本✓ |

**二分查找专题完成**: 10题 ✅

*悟通自检 | 2026-04-17 20:35 CST*
