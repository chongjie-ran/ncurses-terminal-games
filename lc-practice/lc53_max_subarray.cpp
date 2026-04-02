// LC53 Maximum Subarray - Kadane's Algorithm
// O(n) time, O(1) space
#include <iostream>
#include <vector>
using namespace std;

int maxSubArray(vector<int>& nums) {
    int best = nums[0], cur = nums[0];
    for (int i = 1; i < (int)nums.size(); i++) {
        cur = max(nums[i], cur + nums[i]);
        best = max(best, cur);
    }
    return best;
}

int main() {
    vector<int> t1 = {-2,1,-3,4,-1,2,1,-5,4};
    cout << "Test1: " << maxSubArray(t1) << " (expected 6)" << endl;
    
    vector<int> t2 = {1};
    cout << "Test2: " << maxSubArray(t2) << " (expected 1)" << endl;
    
    vector<int> t3 = {5,4,-1,7,8};
    cout << "Test3: " << maxSubArray(t3) << " (expected 23)" << endl;
    
    vector<int> t4 = {-1};
    cout << "Test4: " << maxSubArray(t4) << " (expected -1)" << endl;
    
    vector<int> t5 = {-2,-1};
    cout << "Test5: " << maxSubArray(t5) << " (expected -1)" << endl;
    
    return 0;
}
