// LC215: Kth Largest Element in Array
// 快速选择算法，平均O(n)，最坏O(n²)
// 第k大 = 第n-k小
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int findKthLargest(vector<int>& nums, int k) {
        int left = 0, right = nums.size()-1;
        k--;
        while(true) {
            int p = partition(nums, left, right);
            if(p == (int)nums.size()-k) return nums[p];
            else if(p < (int)nums.size()-k) left = p+1;
            else right = p-1;
        }
    }
private:
    int partition(vector<int>& nums, int l, int r) {
        int pivot = nums[r], i = l;
        for(int j=l; j<r; j++) if(nums[j]<=pivot) swap(nums[i++], nums[j]);
        swap(nums[i], nums[r]);
        return i;
    }
};
