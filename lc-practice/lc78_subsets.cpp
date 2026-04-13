// LC78 Subsets - 回溯算法
// 给定数组，返回所有子集
// 复杂度: O(n * 2^n)

#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> path;
        backtrack(0, nums, path, result);
        return result;
    }
    
    void backtrack(int start, const vector<int>& nums, 
                   vector<int>& path, vector<vector<int>>& result) {
        result.push_back(path); // 每个节点都是子集
        for (int i = start; i < nums.size(); i++) {
            path.push_back(nums[i]);
            backtrack(i + 1, nums, path, result);
            path.pop_back();
        }
    }
};

int main() {
    Solution s;
    vector<int> nums = {1, 2, 3};
    auto result = s.subsets(nums);
    
    cout << "Input: [1,2,3]" << endl;
    cout << "Output: " << result.size() << " subsets" << endl;
    for (auto& subset : result) {
        cout << "[";
        for (int i = 0; i < subset.size(); i++) {
            cout << subset[i] << (i < subset.size()-1 ? "," : "");
        }
        cout << "]" << endl;
    }
    // Expected: [], [1], [1,2], [1,2,3], [1,3], [2], [2,3], [3]
    return 0;
}
