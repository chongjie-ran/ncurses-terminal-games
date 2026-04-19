// LC410 Split Array Largest Sum
// 难度: Hard
// 核心: 二分搜索 + 贪心验证
// 思路: 
//   - 答案范围: [max(nums), sum(nums)]
//   - 二分搜索最大和的最小值
//   - 贪心: 尽可能填满每个子数组，超限时开新子数组
//   - 验证: 能否在限制下分成<=m个子数组

#include <iostream>
#include <vector>
using namespace std;

int splitArray(vector<int>& nums, int m) {
    long long lo = 0, hi = 0;
    for (int num : nums) {
        lo = max(lo, (long long)num);
        hi += num;
    }
    
    while (lo < hi) {
        long long mid = (lo + hi) / 2;
        int cnt = 1;
        long long sum = 0;
        
        for (int num : nums) {
            if (sum + num > mid) {
                cnt++;
                sum = num;
            } else {
                sum += num;
            }
        }
        
        if (cnt <= m) {
            hi = mid;  // 可以分成<=m段，尝试更小的最大值
        } else {
            lo = mid + 1;  // 需要更多段，必须增大最大值
        }
    }
    return (int)lo;
}

int main() {
    // 测试1: [7,2,5,10,8], m=2 -> 18
    // 分法: [7,2,5,10]和[8]，最大和=18
    vector<int> nums1 = {7, 2, 5, 10, 8};
    cout << "7,2,5,10,8 m=2 -> " << splitArray(nums1, 2) << " (期望18)" << endl;
    
    // 测试2: [1,2,3,4,5], m=2 -> 9
    // 分法: [1,2,3,4]和[5]，最大和=9 或 [1,2,3]和[4,5]最大和=9
    vector<int> nums2 = {1, 2, 3, 4, 5};
    cout << "1,2,3,4,5 m=2 -> " << splitArray(nums2, 2) << " (期望9)" << endl;
    
    // 测试3: [1,4,4], m=3 -> 4
    vector<int> nums3 = {1, 4, 4};
    cout << "1,4,4 m=3 -> " << splitArray(nums3, 3) << " (期望4)" << endl;
    
    // 测试4: [1,2,3,4,5], m=1 -> 15
    vector<int> nums4 = {1, 2, 3, 4, 5};
    cout << "1,2,3,4,5 m=1 -> " << splitArray(nums4, 1) << " (期望15)" << endl;
    
    // 测试5: [1,2,3,4,5], m=5 -> 4
    vector<int> nums5 = {1, 2, 3, 4, 5};
    cout << "1,2,3,4,5 m=5 -> " << splitArray(nums5, 5) << " (期望4)" << endl;
    
    return 0;
}
