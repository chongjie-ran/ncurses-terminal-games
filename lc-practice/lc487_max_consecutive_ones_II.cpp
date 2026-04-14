// LC487: Maximum Consecutive Ones II - 调试版
#include <iostream>
#include <vector>
using namespace std;

int longestOnes(vector<int>& nums, int k) {
    int left = 0, zeroCount = 0, maxLen = 0;
    for (int right = 0; right < nums.size(); right++) {
        if (nums[right] == 0) zeroCount++;
        while (zeroCount > k) {
            if (nums[left] == 0) zeroCount--;
            left++;
        }
        maxLen = max(maxLen, right - left + 1);
        cout << "r=" << right << " zc=" << zeroCount << " L=" << left << " len=" << (right-left+1) << " max=" << maxLen << endl;
    }
    return maxLen;
}

int main() {
    cout << "=== Test1: k=1 ===" << endl;
    vector<int> nums1 = {1,1,0,0,1,1,1,0,1};
    cout << "Result: " << longestOnes(nums1, 1) << " (expected: 6)" << endl;
    
    cout << "\n=== Test2: k=2 ===" << endl;
    vector<int> nums2 = {1,1,0,0,1,1,1,0,1};
    cout << "Result: " << longestOnes(nums2, 2) << " (expected: 8)" << endl;
    
    return 0;
}
