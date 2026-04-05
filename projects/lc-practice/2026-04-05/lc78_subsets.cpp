// LC78 Subsets
// 分类: Backtracking / Bit Manipulation
// 难度: Medium
// 思路: 每个元素选或不选，递归生成所有子集
// 时间: O(n * 2^n) 空间: O(n * 2^n)

#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> cur;
        backtrack(nums, 0, cur, result);
        return result;
    }
    
    void backtrack(const vector<int>& nums, int start, vector<int>& cur, vector<vector<int>>& result) {
        result.push_back(cur);
        for (int i = start; i < nums.size(); i++) {
            cur.push_back(nums[i]);
            backtrack(nums, i + 1, cur, result);
            cur.pop_back();
        }
    }
};

// ===== Bit Manipulation Version =====
class Solution2 {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        int n = nums.size();
        int total = 1 << n;  // 2^n subsets
        vector<vector<int>> result;
        for (int mask = 0; mask < total; mask++) {
            vector<int> cur;
            for (int i = 0; i < n; i++) {
                if (mask & (1 << i)) {
                    cur.push_back(nums[i]);
                }
            }
            result.push_back(cur);
        }
        return result;
    }
};

// ===== Tests =====
int main() {
    Solution s;
    
    // Test 1
    vector<int> nums1 = {1, 2, 3};
    auto res1 = s.subsets(nums1);
    cout << "Test 1 (nums=[1,2,3]): " << res1.size() << " subsets\n";
    for (auto& sub : res1) {
        cout << "  [";
        for (int i = 0; i < sub.size(); i++) cout << sub[i] << (i < sub.size()-1 ? "," : "");
        cout << "]\n";
    }
    // Expected: [], [1], [2], [3], [1,2], [1,3], [2,3], [1,2,3] = 8
    
    // Test 2
    vector<int> nums2 = {0};
    auto res2 = s.subsets(nums2);
    cout << "Test 2 (nums=[0]): " << res2.size() << " subsets\n";
    // Expected: [], [0] = 2
    
    // Test 3
    vector<int> nums3 = {};
    auto res3 = s.subsets(nums3);
    cout << "Test 3 (nums=[]): " << res3.size() << " subsets\n";
    // Expected: [[]] = 1
    
    cout << "\nBit Manipulation Version:\n";
    Solution2 s2;
    auto res4 = s2.subsets(nums1);
    cout << "Test 4 (nums=[1,2,3]): " << res4.size() << " subsets\n";
    
    return 0;
}
