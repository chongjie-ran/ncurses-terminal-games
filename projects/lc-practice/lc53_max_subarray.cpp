// LC53 Maximum Subarray - Kadane's Algorithm
// 贪心：局部最大 → 全局最大
// 核心：sum < 0 时重置（负数和只会拉低后续总和）
#include <iostream>
#include <vector>
using namespace std;

int maxSubArray(vector<int>& nums) {
    int maxSum = nums[0];
    int curSum = nums[0];
    
    for (int i = 1; i < nums.size(); i++) {
        curSum = max(nums[i], curSum + nums[i]);  // 重置或延续
        maxSum = max(maxSum, curSum);
    }
    return maxSum;
}

int main() {
    vector<int> t1 = {-2,1,-3,4,-1,2,1,-5,4};
    vector<int> t2 = {1};
    vector<int> t3 = {5,4,-1,7,8};
    vector<int> t4 = {-1};
    vector<int> t5 = {-2,-1};
    
    cout << maxSubArray(t1) << endl;  // 6 ([4,-1,2,1])
    cout << maxSubArray(t2) << endl;  // 1
    cout << maxSubArray(t3) << endl;  // 23
    cout << maxSubArray(t4) << endl;  // -1
    cout << maxSubArray(t5) << endl;  // -1
}
