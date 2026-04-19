// LC46 Permutations - 回溯
// 给定一个无重复元素的数组，返回所有可能的排列
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        results.clear();
        path.clear();
        used.assign(nums.size(), false);
        backtrack(nums);
        return results;
    }

private:
    vector<vector<int>> results;
    vector<int> path;
    vector<bool> used;

    void backtrack(const vector<int>& nums) {
        if (path.size() == nums.size()) {
            results.push_back(path);
            return;
        }
        for (int i = 0; i < nums.size(); i++) {
            if (used[i]) continue;
            used[i] = true;
            path.push_back(nums[i]);
            backtrack(nums);
            path.pop_back();
            used[i] = false;
        }
    }
};

int main() {
    Solution sol;

    // Test 1: [1,2,3] → 6 permutations
    vector<int> nums1 = {1, 2, 3};
    auto res1 = sol.permute(nums1);
    cout << "Test [1,2,3]: " << res1.size() << " permutations (expected 6)" << endl;
    for (auto& p : res1) {
        cout << "  ";
        for (int x : p) cout << x;
        cout << endl;
    }

    // Test 2: [0,1] → 2 permutations
    vector<int> nums2 = {0, 1};
    auto res2 = sol.permute(nums2);
    cout << "Test [0,1]: " << res2.size() << " permutations (expected 2)" << endl;

    // Test 3: [1] → 1 permutation
    vector<int> nums3 = {1};
    auto res3 = sol.permute(nums3);
    cout << "Test [1]: " << res3.size() << " permutations (expected 1)" << endl;

    return 0;
}
