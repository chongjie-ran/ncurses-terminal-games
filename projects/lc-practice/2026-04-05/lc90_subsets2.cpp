// LC90 Subsets II
// 分类: Backtracking
// 难度: Medium
// 思路: 先排序，相邻相同元素只在第一个的分支继续递归，避免重复子集
// 时间: O(n * 2^n) 空间: O(n * 2^n)

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Solution {
public:
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        sort(nums.begin(), nums.end());
        vector<vector<int>> result;
        vector<int> cur;
        backtrack(nums, 0, cur, result);
        return result;
    }
    
    void backtrack(const vector<int>& nums, int start, vector<int>& cur, vector<vector<int>>& result) {
        result.push_back(cur);
        for (int i = start; i < nums.size(); i++) {
            // Skip duplicates: only take nums[i] if it's the first in this level
            if (i > start && nums[i] == nums[i-1]) continue;
            cur.push_back(nums[i]);
            backtrack(nums, i + 1, cur, result);
            cur.pop_back();
        }
    }
};

int main() {
    Solution s;
    
    // Test 1: [1,2,2] -> [], [1], [1,2], [1,2,2], [2], [2,2]
    vector<int> nums1 = {1, 2, 2};
    auto res1 = s.subsetsWithDup(nums1);
    cout << "Test 1 ([1,2,2]): " << res1.size() << " subsets\n";
    for (auto& sub : res1) {
        cout << "  [";
        for (int i = 0; i < sub.size(); i++) cout << sub[i] << (i < sub.size()-1 ? "," : "");
        cout << "]\n";
    }
    // Expected: [], [1], [1,2], [1,2,2], [2], [2,2] = 6
    
    // Test 2: [0]
    vector<int> nums2 = {0};
    auto res2 = s.subsetsWithDup(nums2);
    cout << "Test 2 ([0]): " << res2.size() << " subsets\n";
    // Expected: [], [0] = 2
    
    // Test 3: [1,1,1]
    vector<int> nums3 = {1, 1, 1};
    auto res3 = s.subsetsWithDup(nums3);
    cout << "Test 3 ([1,1,1]): " << res3.size() << " subsets\n";
    // Expected: [], [1], [1,1], [1,1,1] = 4 (3 duplicates removed)
    
    return 0;
}
