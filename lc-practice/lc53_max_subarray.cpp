// LC53 Maximum Subarray (Kadane's Algorithm)
// 核心：dp[i] = max(dp[i-1] + nums[i], nums[i])
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

// 方法1：Kadane's Algorithm (最优)
class Solution1 {
public:
    int maxSubArray(vector<int>& nums) {
        int maxSum = nums[0];
        int current = nums[0];
        for (int i = 1; i < nums.size(); i++) {
            current = max(current + nums[i], nums[i]);
            maxSum = max(maxSum, current);
        }
        return maxSum;
    }
};

// 方法2：分治法
class Solution2 {
public:
    int maxSubArray(vector<int>& nums) {
        return divideConquer(nums, 0, (int)nums.size() - 1);
    }
    
private:
    int divideConquer(const vector<int>& nums, int left, int right) {
        if (left == right) return nums[left];
        int mid = left + (right - left) / 2;
        
        int leftMax = divideConquer(nums, left, mid);
        int rightMax = divideConquer(nums, mid + 1, right);
        
        int leftSum = 0, leftBest = INT_MIN;
        for (int i = mid; i >= left; i--) {
            leftSum += nums[i];
            leftBest = max(leftBest, leftSum);
        }
        
        int rightSum = 0, rightBest = INT_MIN;
        for (int i = mid + 1; i <= right; i++) {
            rightSum += nums[i];
            rightBest = max(rightBest, rightSum);
        }
        
        int crossMax = leftBest + rightBest;
        return max({leftMax, rightMax, crossMax});
    }
};

int main() {
    vector<int> nums = {-2,1,-3,4,-1,2,1,-5,4};
    
    cout << "LC53 Maximum Subarray:" << endl;
    cout << "nums: [-2,1,-3,4,-1,2,1,-5,4]" << endl;
    
    Solution1 sol1;
    int r1 = sol1.maxSubArray(nums);
    cout << "Kadane: " << r1 << " (expected 6: [4,-1,2,1])" << endl;
    
    Solution2 sol2;
    int r2 = sol2.maxSubArray(nums);
    cout << "分治: " << r2 << " ✓" << endl;
    
    vector<int> single = {5};
    cout << "\n边界: [5] -> " << sol1.maxSubArray(single) << " (expected 5)" << endl;
    
    vector<int> allNeg = {-3,-1,-2};
    cout << "全负: [-3,-1,-2] -> " << sol1.maxSubArray(allNeg) << " (expected -1)" << endl;
    
    return 0;
}
