#include <stdio.h>
#include <limits.h>

// LC153. Find Minimum in Rotated Sorted Array
// Time: O(log n), Space: O(1)

int findMin(int* nums, int numsSize) {
    int left = 0, right = numsSize - 1;
    
    while (left < right) {
        int mid = left + (right - left) / 2;
        
        if (nums[mid] > nums[right]) {
            // Min is in right half
            left = mid + 1;
        } else {
            // Min is in left half (including mid)
            right = mid;
        }
    }
    
    return nums[left];
}

// Test
int main() {
    int nums1[] = {3, 4, 5, 1, 2};
    printf("LC153 Find Minimum in Rotated Sorted Array:\n");
    printf("findMin([3,4,5,1,2], 5) = %d (expected 1)\n", findMin(nums1, 5));
    
    int nums2[] = {4, 5, 6, 7, 0, 1, 2};
    printf("findMin([4,5,6,7,0,1,2], 7) = %d (expected 0)\n", findMin(nums2, 7));
    
    int nums3[] = {11, 13, 15, 17};
    printf("findMin([11,13,15,17], 4) = %d (expected 11)\n", findMin(nums3, 4));
    
    int nums4[] = {2, 1};
    printf("findMin([2,1], 2) = %d (expected 1)\n", findMin(nums4, 2));
    
    int nums5[] = {1};
    printf("findMin([1], 1) = %d (expected 1)\n", findMin(nums5, 1));
    
    return 0;
}
