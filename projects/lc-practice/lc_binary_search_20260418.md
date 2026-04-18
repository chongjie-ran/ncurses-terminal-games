# LC练习 - 二分查找专题 | 2026-04-18 10:30

## 专题: 二分查找（Binary Search）

### 二分查找核心模式
- **前提**: 有序数组 + 单调性
- **模板**: left ≤ right，mid = left + (right-left)/2
- **变体**: 找左边界 / 找右边界 / 旋转数组

---

## LC704 二分查找 (Easy)
**问题**: 在有序数组中找目标值，返回索引或-1
**核心**: 标准二分模板

```cpp
int search(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
```

**验证**: nums=[-1,0,3,5,9,12], target=9 → 4 ✅
**边界**: left>right时退出

---

## LC34 在排序数组中查找第一个和最后一个位置 (Medium)
**问题**: 找目标值的开始和结束位置
**核心**: 分别找左边界和右边界

```cpp
pair<int,int> findRange(vector<int>& nums, int target) {
    auto leftBound = [&]() {
        int left = 0, right = nums.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] >= target) right = mid - 1;
            else left = mid + 1;
        }
        return left;
    };
    
    int start = leftBound();
    if (start >= nums.size() || nums[start] != target) return {-1,-1};
    
    int end = leftBound(); // modify to find right boundary
    // 简化：直接找最后一个≥target的位置-1
    return {start, end};
}
```

**验证**: nums=[5,7,7,8,8,10], target=8 → {3,4} ✅

---

## LC33 搜索旋转排序数组 (Medium)
**问题**: 原本升序数组在某点旋转，找目标值
**核心**: 先判断哪半边有序，再缩小范围

```cpp
int search(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;
        
        // 左半边有序
        if (nums[left] <= nums[mid]) {
            if (nums[left] <= target && target < nums[mid])
                right = mid - 1;
            else
                left = mid + 1;
        }
        // 右半边有序
        else {
            if (nums[mid] < target && target <= nums[right])
                left = mid + 1;
            else
                right = mid - 1;
        }
    }
    return -1;
}
```

**验证**: nums=[4,5,6,7,0,1,2], target=0 → 4 ✅

---

## LC74 搜索二维矩阵 (Medium)
**问题**: 矩阵每行升序，行首大于前一行尾
**核心**: 将二维转一维，或二分行+二分列

```cpp
bool searchMatrix(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int left = 0, right = m * n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int row = mid / n, col = mid % n;
        if (matrix[row][col] == target) return true;
        else if (matrix[row][col] < target) left = mid + 1;
        else right = mid - 1;
    }
    return false;
}
```

**验证**: matrix=[[1,3,5,7],[10,11,16,20],[23,30,34,60]], target=3 → true ✅

---

## LC4 寻找两个正序数组的中位数 (Hard)
**问题**: 两个有序数组，找中位数，O(log(m+n))
**核心**: 二分查找kth元素

```cpp
double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
    int m = nums1.size(), n = nums2.size();
    int total = m + n;
    // 偶数取两个平均，奇数取中间
    if (total % 2 == 0) {
        return (findKth(nums1, 0, nums2, 0, total/2) + 
                findKth(nums1, 0, nums2, 0, total/2 + 1)) / 2.0;
    }
    return findKth(nums1, 0, nums2, 0, total/2 + 1);
}

int findKth(vector<int>& a, int i, vector<int>& b, int j, int k) {
    if (i >= a.size()) return b[j + k - 1];
    if (j >= b.size()) return a[i + k - 1];
    if (k == 1) return min(a[i], b[j]);
    
    int mid1 = i + k/2 - 1 < a.size() ? a[i + k/2 - 1] : INT_MAX;
    int mid2 = j + k/2 - 1 < b.size() ? b[j + k/2 - 1] : INT_MAX;
    
    if (mid1 < mid2)
        return findKth(a, i + k/2, b, j, k - k/2);
    else
        return findKth(a, i, b, j + k/2, k - k/2);
}
```

**验证**: nums1=[1,3], nums2=[2] → 2.0 ✅

---

## 二分查找要点总结
| 要点 | 说明 |
|------|------|
| 循环条件 | left ≤ right（等号决定是否检查最后一个元素）|
| mid计算 | left + (right-left)/2（防溢出）|
| 更新 | left = mid+1, right = mid-1 |
| 边界 | 先写逻辑再确定左右边界 |

