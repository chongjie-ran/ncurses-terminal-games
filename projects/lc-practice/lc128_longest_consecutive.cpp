#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

int longestConsecutive(vector<int>& nums) {
    unordered_map<int, int> parent, size;
    function<int(int)> find = [&](int x) -> int {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    };
    auto unite = [&](int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return;
        if (size[px] < size[py]) swap(px, py);
        parent[py] = px;
        size[px] += size[py];
    };
    
    for (int n : nums) {
        parent[n] = n;
        size[n] = 1;
    }
    for (int n : nums) {
        if (parent.find(n+1) != parent.end()) unite(n, n+1);
        if (parent.find(n-1) != parent.end()) unite(n, n-1);
    }
    
    int ans = 0;
    for (auto& kv : size) ans = max(ans, kv.second);
    return ans;
}

int main() {
    // [100,4,200,1,3,2] -> 4 (1,2,3,4)
    vector<int> nums = {100,4,200,1,3,2};
    int ans = longestConsecutive(nums);
    cout << "LC128 最长连续序列: " << ans << (ans == 4 ? " ✓" : " ✗ FAIL") << endl;
    // [0,3,7,2,5,8,4,6,0,1] -> 9
    nums = {0,3,7,2,5,8,4,6,0,1};
    ans = longestConsecutive(nums);
    cout << "  [0,3,7,2,5,8,4,6,0,1]: " << ans << (ans == 9 ? " ✓" : " ✗ FAIL") << endl;
    return 0;
}
