// LC215: 数组中的第K个最大元素
// 难度: Medium | 分类: 堆/快速选择
// 核心: 求第K大 => 维护大小为K的最小堆
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// 方法1: 最小堆（库函数）
class Solution1 {
public:
    int findKthLargest(vector<int>& nums, int k) {
        // 维护大小为k的最小堆
        priority_queue<int, vector<int>, greater<int>> minHeap;
        
        for (int num : nums) {
            minHeap.push(num);
            if (minHeap.size() > k) {
                minHeap.pop();
            }
        }
        
        return minHeap.top();
    }
};

// 方法2: 快速选择（手写堆）
class Solution2 {
public:
    int findKthLargest(vector<int>& nums, int k) {
        int n = nums.size();
        // 构建最小堆（从n/2-1到0下沉）
        for (int i = n/2 - 1; i >= 0; i--) {
            heapify(nums, n, i);
        }
        
        // 弹出n-k次
        for (int i = 0; i < n - k; i++) {
            swap(nums[0], nums[n-1-i]);
            heapify(nums, n-1-i, 0);
        }
        
        return nums[0];
    }
    
    void heapify(vector<int>& nums, int n, int i) {
        int smallest = i;
        int left = 2*i + 1;
        int right = 2*i + 2;
        
        if (left < n && nums[left] < nums[smallest]) smallest = left;
        if (right < n && nums[right] < nums[smallest]) smallest = right;
        
        if (smallest != i) {
            swap(nums[i], nums[smallest]);
            heapify(nums, n, smallest);
        }
    }
};

// 方法3: 快速选择算法（平均O(n)）
class Solution3 {
public:
    int findKthLargest(vector<int>& nums, int k) {
        int n = nums.size();
        int target = n - k; // 转成求第target小的数
        
        return quickSelect(nums, 0, n-1, target);
    }
    
    int quickSelect(vector<int>& nums, int l, int r, int target) {
        int mid = partition(nums, l, r);
        
        if (mid == target) return nums[mid];
        if (mid < target) return quickSelect(nums, mid+1, r, target);
        return quickSelect(nums, l, mid-1, target);
    }
    
    int partition(vector<int>& nums, int l, int r) {
        int pivot = nums[r];
        int i = l;
        for (int j = l; j < r; j++) {
            if (nums[j] <= pivot) {
                swap(nums[i], nums[j]);
                i++;
            }
        }
        swap(nums[i], nums[r]);
        return i;
    }
};

// 测试
int main() {
    vector<int> nums = {3,2,1,5,6,4};
    
    cout << "LC215 Kth Largest:" << endl;
    cout << "nums: [3,2,1,5,6,4]" << endl;
    
    Solution1 sol1;
    for (int k = 1; k <= 6; k++) {
        vector<int> copy = nums;
        cout << "k=" << k << " -> " << sol1.findKthLargest(copy, k) << endl;
    }
    
    cout << endl << "验证: k=2 -> 5, k=3 -> 4" << endl;
    
    return 0;
}
