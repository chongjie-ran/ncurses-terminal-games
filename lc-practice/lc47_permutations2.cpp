// LC47 Permutations II - Backtracking with deduplication
// Key: sort + skip duplicates, track used[]
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void permuteUniqueHelper(vector<int>& nums, vector<int>& path, vector<bool>& used, vector<vector<int>>& result) {
    if (path.size() == nums.size()) {
        result.push_back(path);
        return;
    }
    for (int i = 0; i < (int)nums.size(); i++) {
        if (used[i]) continue;
        // Skip duplicates: if same value not used yet at this level, skip
        if (i > 0 && nums[i] == nums[i-1] && !used[i-1]) continue;
        used[i] = true;
        path.push_back(nums[i]);
        permuteUniqueHelper(nums, path, used, result);
        path.pop_back();
        used[i] = false;
    }
}

vector<vector<int>> permuteUnique(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    vector<int> path;
    vector<bool> used(nums.size(), false);
    permuteUniqueHelper(nums, path, used, result);
    return result;
}

int main() {
    // Test 1
    vector<int> t1 = {1,1,2};
    vector<vector<int>> r1 = permuteUnique(t1);
    cout << "Test1: " << r1.size() << " permutations (expected 3)" << endl;
    for (auto& p : r1) {
        cout << "  ";
        for (int x : p) cout << x;
        cout << endl;
    }
    
    // Test 2
    vector<int> t2 = {1,2,3};
    vector<vector<int>> r2 = permuteUnique(t2);
    cout << "Test2: " << r2.size() << " permutations (expected 6)" << endl;
    
    return 0;
}
