// LC215 - 数组中第K大的元素
// 策略：最小堆维护k个最大元素，堆顶即为第k大
// 时间：O(n log k)，空间：O(k)
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    int findKthLargest(vector<int>& nums, int k) {
        // 最小堆：堆顶最小，所以只保留k个最大的
        priority_queue<int, vector<int>, greater<int>> min_heap;
        
        for (int num : nums) {
            min_heap.push(num);
            if (min_heap.size() > k) {
                min_heap.pop(); // 移除最小的，保持k个最大
            }
        }
        
        return min_heap.top(); // 堆顶是第k大的
    }
};

// 验证
int main() {
    Solution s;
    
    // Test 1
    vector<int> nums1 = {3,2,1,5,6,4};
    int k1 = 2;
    int ans1 = s.findKthLargest(nums1, k1);
    cout << "Test1: " << ans1 << " (expect 5) -> " << (ans1==5?"✅":"❌") << endl;
    
    // Test 2
    vector<int> nums2 = {3,2,3,1,2,4,5,5,6};
    int k2 = 4;
    int ans2 = s.findKthLargest(nums2, k2);
    cout << "Test2: " << ans2 << " (expect 4) -> " << (ans2==4?"✅":"❌") << endl;
    
    // Test 3: k=1 (最大)
    vector<int> nums3 = {1};
    int ans3 = s.findKthLargest(nums3, 1);
    cout << "Test3: " << ans3 << " (expect 1) -> " << (ans3==1?"✅":"❌") << endl;
    
    // Test 4: 全相同
    vector<int> nums4 = {5,5,5,5};
    int ans4 = s.findKthLargest(nums4, 2);
    cout << "Test4: " << ans4 << " (expect 5) -> " << (ans4==5?"✅":"❌") << endl;
    
    return 0;
}
