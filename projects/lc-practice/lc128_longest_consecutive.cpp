// LC128 Longest Consecutive Sequence - 最长连续序列
// Hard | HashSet | O(n)时间复杂度
#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
using namespace std;

// 方法1: HashSet + 区间扩展 (O(n)平均)
int longestConsecutive1(vector<int>& nums) {
    if (nums.empty()) return 0;
    
    unordered_set<int> s(nums.begin(), nums.end());
    int longest = 0;
    
    for (int num : s) {
        // 只从序列起点开始扩展
        if (s.find(num - 1) == s.end()) {
            int cur = num;
            int len = 1;
            while (s.find(++cur) != s.end()) len++;
            longest = max(longest, len);
        }
    }
    return longest;
}

// 方法2: 排序后遍历 (O(n log n))
int longestConsecutive2(vector<int>& nums) {
    if (nums.empty()) return 0;
    sort(nums.begin(), nums.end());
    
    int longest = 1, cur = 1;
    for (int i = 1; i < nums.size(); i++) {
        if (nums[i] == nums[i-1] + 1) {
            cur++;
        } else if (nums[i] != nums[i-1]) {
            cur = 1;
        }
        longest = max(longest, cur);
    }
    return longest;
}

int main() {
    // 测试1
    vector<int> t1 = {100,4,200,1,3,2};
    cout << "Test1: " << longestConsecutive1(t1) << endl;  // 期望: 4 (1,2,3,4)
    
    // 测试2
    vector<int> t2 = {0,3,7,2,5,8,4,6,0,1};
    cout << "Test2: " << longestConsecutive1(t2) << endl;  // 期望: 9
    
    // 测试3: 空数组
    vector<int> t3 = {};
    cout << "Test3: " << longestConsecutive1(t3) << endl;  // 期望: 0
    
    // 测试4: 单个元素
    vector<int> t4 = {1};
    cout << "Test4: " << longestConsecutive1(t4) << endl;  // 期望: 1
    
    // 测试5: 方法2验证
    cout << "Method2 Test1: " << longestConsecutive2(t1) << endl;
    
    return 0;
}
