#include <stdio.h>
#include <stdlib.h>

// LC704. Binary Search
// Given sorted integer array nums and target, return index or -1
// Time: O(log n), Space: O(1)

int search(int* nums, int numsSize, int target) {
    int left = 0;
    int right = numsSize - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;  // avoid overflow
        
        if (nums[mid] == target) {
            return mid;
        } else if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1;
}

// Test
int main() {
    int nums1[] = {-1, 0, 3, 5, 9, 12};
    printf("LC704 Binary Search:\n");
    printf("search([-1,0,3,5,9,12], 6, 9) = %d (expected 4)\n", search(nums1, 6, 9));
    printf("search([-1,0,3,5,9,12], 6, 2) = %d (expected -1)\n", search(nums1, 6, 2));
    
    int nums2[] = {5};
    printf("search([5], 1, 5) = %d (expected 0)\n", search(nums2, 1, 5));
    printf("search([5], 1, -5) = %d (expected -1)\n", search(nums2, 1, -5));
    
    return 0;
}
