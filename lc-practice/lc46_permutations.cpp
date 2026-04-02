// LC46 Permutations - Backtracking
// O(n! * n) time, O(n) space
#include <iostream>
#include <vector>
using namespace std;

void permuteHelper(vector<int>& nums, int start, vector<vector<int>>& result) {
    if (start == (int)nums.size()) {
        result.push_back(nums);
        return;
    }
    for (int i = start; i < (int)nums.size(); i++) {
        swap(nums[start], nums[i]);
        permuteHelper(nums, start + 1, result);
        swap(nums[start], nums[i]); // backtrack
    }
}

vector<vector<int>> permute(vector<int>& nums) {
    vector<vector<int>> result;
    permuteHelper(nums, 0, result);
    return result;
}

int main() {
    // Test 1
    vector<int> t1 = {1,2,3};
    vector<vector<int>> r1 = permute(t1);
    cout << "Test1 ([1,2,3]): " << r1.size() << " permutations (expected 6)" << endl;
    for (auto& p : r1) {
        cout << "  ";
        for (int x : p) cout << x;
        cout << endl;
    }
    
    // Test 2
    vector<int> t2 = {0,1};
    vector<vector<int>> r2 = permute(t2);
    cout << "Test2 ([0,1]): " << r2.size() << " permutations (expected 2)" << endl;
    
    // Test 3
    vector<int> t3 = {1};
    vector<vector<int>> r3 = permute(t3);
    cout << "Test3 ([1]): " << r3.size() << " permutations (expected 1)" << endl;
    
    return 0;
}
