#include <stdio.h>
#include <stdlib.h>

// LC33. Search in Rotated Sorted Array
// Array is rotated at some pivot, find target
// Time: O(log n), Space: O(1)

int search(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (nums[mid] == target) return mid;
        
        // Determine which half is sorted
        if (nums[left] <= nums[mid]) {
            // Left half is sorted
            if (nums[left] <= target && target < nums[mid]) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        } else {
            // Right half is sorted
            if (nums[mid] < target && target <= nums[right]) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    }
    
    return -1;
}

// Test
int main() {
    int nums1[] = {4, 5, 6, 7, 0, 1, 2};
    printf("LC33 Search in Rotated Sorted Array:\n");
    printf("search([4,5,6,7,0,1,2], 7, 0) = %d (expected 4)\n", search(nums1, 7, 0));
    printf("search([4,5,6,7,0,1,2], 7, 3) = %d (expected -1)\n", search(nums1, 7, 3));
    printf("search([4,5,6,7,0,1,2], 7, 4) = %d (expected 0)\n", search(nums1, 7, 4));
    printf("search([4,5,6,7,0,1,2], 7, 2) = %d (expected 6)\n", search(nums1, 7, 2));
    
    int nums2[] = {1};
    printf("search([1], 1, 0) = %d (expected -1)\n", search(nums2, 1, 0));
    
    int nums3[] = {1, 3};
    printf("search([1,3], 2, 3) = %d (expected 1)\n", search(nums3, 2, 3));
    
    return 0;
}
