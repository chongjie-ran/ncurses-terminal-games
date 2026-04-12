#include <stdio.h>
#include <stdlib.h>

// LC34. Find First and Last Position of Element in Sorted Array
// Time: O(log n), Space: O(1)

int findFirst(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;
    int result = -1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) {
            result = mid;
            right = mid - 1;  // continue searching left
        } else if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return result;
}

int findLast(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;
    int result = -1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) {
            result = mid;
            left = mid + 1;  // continue searching right
        } else if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return result;
}

int* searchRange(int* nums, int numsSize, int target, int* returnSize) {
    *returnSize = 2;
    int* result = (int*)malloc(sizeof(int) * 2);
    result[0] = findFirst(nums, numsSize, target);
    result[1] = findLast(nums, numsSize, target);
    return result;
}

// Test
int main() {
    int nums1[] = {5, 7, 7, 8, 8, 10};
    int returnSize;
    int* result;
    
    printf("LC34 Find First and Last Position:\n");
    result = searchRange(nums1, 6, 8, &returnSize);
    printf("searchRange([5,7,7,8,8,10], 6, 8) = [%d, %d] (expected [3, 4])\n", result[0], result[1]);
    free(result);
    
    result = searchRange(nums1, 6, 6, &returnSize);
    printf("searchRange([5,7,7,8,8,10], 6, 6) = [%d, %d] (expected [-1, -1])\n", result[0], result[1]);
    free(result);
    
    int nums2[] = {5, 7, 7, 8, 8, 8, 10};
    result = searchRange(nums2, 7, 8, &returnSize);
    printf("searchRange([5,7,7,8,8,8,10], 7, 8) = [%d, %d] (expected [3, 5])\n", result[0], result[1]);
    free(result);
    
    int nums3[] = {};
    result = searchRange(nums3, 0, 0, &returnSize);
    printf("searchRange([], 0, 0) = [%d, %d] (expected [-1, -1])\n", result[0], result[1]);
    free(result);
    
    return 0;
}
