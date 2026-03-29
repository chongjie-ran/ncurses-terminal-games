// LC327: Count of Range Sum — 区间和的个数
// Hard: BIT + 离散化 + 前缀和
// 思路: 前缀和之差在 [lower, upper] 范围内
//   sum[j] - sum[i] ∈ [lower, upper]  for i < j
//   => sum[i] ∈ [sum[j]-upper, sum[j]-lower]
//   用 BIT 统计前缀和数组中落在某区间的个数
// 时间: O(n log n), 空间: O(n)

#include <vector>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <map>
#include <set>

using namespace std;

class BIT {
    vector<int> tree;
    int n;
public:
    BIT(int n): n(n), tree(n+1, 0) {}
    void add(int idx, int val) {
        for (++idx; idx <= n; idx += idx & -idx) tree[idx] += val;
    }
    int sumPrefix(int idx) const {
        int r = 0;
        for (++idx; idx > 0; idx -= idx & -idx) r += tree[idx];
        return r;
    }
    int rangeSum(int l, int r) const {
        if (r < l) return 0;
        return sumPrefix(r) - (l > 0 ? sumPrefix(l-1) : 0);
    }
};

int countRangeSum(vector<int>& nums, int lower, int upper) {
    // 前缀和: pre[i] = sum(nums[0..i-1]), pre[0] = 0
    vector<long long> pre(nums.size() + 1);
    for (int i = 0; i < (int)nums.size(); ++i) pre[i+1] = pre[i] + nums[i];

    // 离散化：所有前缀和 + 所有 pre[k]-lower 和 pre[k]-upper 边界
    vector<long long> vals = pre;
    for (int i = 0; i < (int)pre.size(); ++i) {
        vals.push_back(pre[i] - lower);
        vals.push_back(pre[i] - upper);
    }
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());

    BIT bit((int)vals.size());
    long long ans = 0;

    for (int i = 0; i < (int)pre.size(); ++i) {
        // 当前前缀 pre[i] 对应的离散化索引
        int idx = (int)(lower_bound(vals.begin(), vals.end(), pre[i]) - vals.begin());
        // 统计之前有多少 pre 在 [pre[i]-upper, pre[i]-lower] 范围内
        long long lo = pre[i] - upper;
        long long hi = pre[i] - lower;
        int l = (int)(lower_bound(vals.begin(), vals.end(), lo) - vals.begin());
        int r = (int)(lower_bound(vals.begin(), vals.end(), hi) - vals.begin());
        int cnt = bit.rangeSum(l, r);
        ans += cnt;
        bit.add(idx, 1);
    }
    return (int)ans;
}

int main() {
    vector<int> nums1 = {-2, 5, -1};
    int lo1 = -2, up1 = 2;
    cout << countRangeSum(nums1, lo1, up1) << endl; // 3

    vector<int> nums2 = {0};
    cout << countRangeSum(nums2, 0, 0) << endl; // 1

    vector<int> nums3 = {1,-1};
    cout << countRangeSum(nums3, -1, 1) << endl; // 3

    // Test: large range
    vector<int> nums4 = {5,-7,3,-5,10,-2,-1,6};
    cout << countRangeSum(nums4, -2, 5) << endl; // 应该有一些结果
    return 0;
}
