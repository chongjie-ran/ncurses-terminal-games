// LC78 Subsets - 求集合的所有子集
// 复杂度: O(2^n * n)
// 核心模式: 回溯 - 选或不选每个元素

#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> path;
        backtrack(nums, 0, path, result);
        return result;
    }
    
    void backtrack(const vector<int>& nums, int start, 
                   vector<int>& path, vector<vector<int>>& result) {
        result.push_back(path);  // 每个节点都是子集
        
        for (int i = start; i < nums.size(); i++) {
            path.push_back(nums[i]);    // 选择
            backtrack(nums, i + 1, path, result);
            path.pop_back();             // 撤销
        }
    }
};

// 测试
int main() {
    Solution s;
    
    // Test1: [1,2,3] -> 8 subsets
    vector<int> nums1 = {1,2,3};
    auto r1 = s.subsets(nums1);
    cout << "Test1: " << r1.size() << "/8" << endl;
    
    // Test2: [0] -> 2 subsets
    vector<int> nums2 = {0};
    auto r2 = s.subsets(nums2);
    cout << "Test2: " << r2.size() << "/2" << endl;
    
    // Test3: [] -> 1 subset
    vector<int> nums3;
    auto r3 = s.subsets(nums3);
    cout << "Test3: " << r3.size() << "/1" << endl;
    
    // 验证: [1,2,3]应有8个
    cout << "Expected 8, Got " << r1.size() << " -> " << (r1.size()==8?"PASS":"FAIL") << endl;
    
    return 0;
}
