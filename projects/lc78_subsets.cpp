#include <iostream>
#include <vector>
using namespace std;

// LC78 Subsets - 回溯算法
class Solution {
public:
    vector<vector<int>> result;
    
    void backtrack(const vector<int>& nums, int start, vector<int>& path) {
        result.push_back(path);
        for (int i = start; i < (int)nums.size(); i++) {
            path.push_back(nums[i]);
            backtrack(nums, i + 1, path);
            path.pop_back();
        }
    }
    
    vector<vector<int>> subsets(vector<int>& nums) {
        result.clear();
        vector<int> path;
        backtrack(nums, 0, path);
        return result;
    }
};

int main() {
    // Test 1: [1,2,3]
    {
        Solution s;
        vector<int> nums = {1, 2, 3};
        auto res = s.subsets(nums);
        cout << "Input: [1,2,3]" << endl;
        cout << "Output: " << res.size() << " subsets" << endl;
        for (auto& subset : res) {
            cout << "  [";
            for (int i = 0; i < (int)subset.size(); i++)
                cout << subset[i] << (i < (int)subset.size()-1 ? "," : "");
            cout << "]" << endl;
        }
        cout << "Expected: 8, Got: " << res.size() << " => " << (res.size()==8?"✅":"❌") << endl;
    }
    
    // Test 2: []
    {
        Solution s;
        vector<int> nums = {};
        auto res = s.subsets(nums);
        cout << "\nInput: [] => " << res.size() << " (expected 1) => " << (res.size()==1?"✅":"❌") << endl;
    }
    
    // Test 3: [1]
    {
        Solution s;
        vector<int> nums = {1};
        auto res = s.subsets(nums);
        cout << "Input: [1] => " << res.size() << " (expected 2) => " << (res.size()==2?"✅":"❌") << endl;
    }
    
    // Test 4: [1,2,3,4]
    {
        Solution s;
        vector<int> nums = {1, 2, 3, 4};
        auto res = s.subsets(nums);
        cout << "Input: [1,2,3,4] => " << res.size() << " (expected 16) => " << (res.size()==16?"✅":"❌") << endl;
    }
    
    cout << "\n=== Backtrack Pattern ===" << endl;
    cout << "1. result.push_back(path) at each node" << endl;
    cout << "2. for loop from start to n-1" << endl;
    cout << "3. path.push_back -> recurse(i+1) -> path.pop_back" << endl;
    cout << "4. Time: O(n*2^n), Space: O(n)" << endl;
    
    return 0;
}
