// LC307 Range Sum Query - Mutable
// 树状数组 (Fenwick Tree) 实现
// update: O(log n), sumRange: O(log n)

#include <iostream>
#include <vector>
using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;
public:
    FenwickTree(int n_=0): n(n_), bit(n_+1, 0) {}
    
    void init(const vector<int>& nums) {
        n = nums.size();
        bit.assign(n + 1, 0);
        for (int i = 0; i < n; ++i)
            _add(i + 1, nums[i]);
    }
    
    // 内部: 1-based
    void _add(int i, int delta) {
        for (; i <= n; i += i & -i)
            bit[i] += delta;
    }
    
    // 对外: 0-based index, 加 delta
    void add(int idx, int delta) {
        _add(idx + 1, delta);
    }
    
    // 前缀和 1-based
    int _sum(int i) const {
        int res = 0;
        for (; i > 0; i -= i & -i)
            res += bit[i];
        return res;
    }
    
    // 前缀和 0-based
    int sum(int idx) const {
        return _sum(idx + 1);
    }
    
    // 区间和 [l, r] 0-based
    int rangeSum(int l, int r) const {
        if (l > r) return 0;
        return _sum(r + 1) - _sum(l);
    }
};

class NumArray {
    FenwickTree ft;
    vector<int> arr;
    int n;
public:
    NumArray(const vector<int>& nums) {
        n = nums.size();
        arr = nums;
        ft.init(nums);
    }
    
    void update(int index, int val) {
        int delta = val - arr[index];
        arr[index] = val;
        ft.add(index, delta);
    }
    
    int sumRange(int left, int right) const {
        return ft.rangeSum(left, right);
    }
};

int main() {
    // LeetCode 官方示例
    vector<int> nums1 = {1, 3, 5};
    NumArray na(nums1);
    cout << na.sumRange(0, 2) << endl;  // 9
    na.update(1, 2);
    cout << na.sumRange(0, 2) << endl;  // 8
    
    // LeetCode 官方测试2: [7,2,7,2,0]
    vector<int> nums2 = {7, 2, 7, 2, 0};
    NumArray na2(nums2);
    na2.update(4, 6);
    na2.update(0, 2);
    na2.update(3, 9);
    cout << na2.sumRange(3, 4) << endl;  // 15
    na2.update(4, 1);
    cout << na2.sumRange(0, 4) << endl;  // 21
    
    return 0;
}
