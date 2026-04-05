/*
LC41 - First Missing Positive
分类: Array + In-place Hash
难度: Hard
日期: 2026-04-05

思路:
  O(n)时间+O(1)空间:
  将每个数字n放到索引n-1的位置（对于1<=n<=n的数）
  第一遍: 遍历，如果1<=nums[i]<=n且nums[nums[i]-1]!=nums[i]，交换
  第二遍: 第一个nums[i]!=i+1的i+1就是答案
  
关键洞察:
  - 只需要处理1到n范围内的正数
  - 利用数组索引做hash（in-place）
  - 跳过已经在正确位置的数避免死循环

时间: O(n), 空间: O(1)
*/

#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

int firstMissingPositive(vector<int> nums) {
    int n = nums.size();
    for (int i = 0; i < n; ) {
        int x = nums[i];
        if (x >= 1 && x <= n && nums[x-1] != x) {
            swap(nums[i], nums[x-1]);
        } else {
            i++;
        }
    }
    for (int i = 0; i < n; i++) {
        if (nums[i] != i + 1) return i + 1;
    }
    return n + 1;
}

int main() {
    assert(firstMissingPositive({1,2,0}) == 3);
    cout << "Test1=3 ✅ [1,2,0]";
    assert(firstMissingPositive({3,4,-1,1}) == 2);
    cout << ", Test2=2 ✅ [3,4,-1,1]";
    assert(firstMissingPositive({7,8,9,11,12}) == 1);
    cout << ", Test3=1 ✅ [7,8,9,11,12]";
    assert(firstMissingPositive({1,1}) == 2);
    cout << ", Test4=2 ✅ [1,1]";
    assert(firstMissingPositive({1}) == 2);
    cout << ", Test5=2 ✅ [1]";
    assert(firstMissingPositive({2}) == 1);
    cout << ", Test6=1 ✅ [2]";
    cout << "\nAll tests PASS\n";
    return 0;
}
