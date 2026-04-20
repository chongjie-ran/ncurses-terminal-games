// LC213 House Robber II - 环形房屋盗窃
// 关键: 拆分为两个线性问题: [0,n-2] 和 [1,n-1]
#include <iostream>
#include <vector>
using namespace std;

class Solution {
    int robLinear(vector<int>& nums, int l, int r) {
        int prev2 = 0, prev1 = 0;
        for (int i = l; i <= r; i++) {
            int cur = max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = cur;
        }
        return prev1;
    }
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        if (n == 0) return 0;
        if (n == 1) return nums[0];
        // 两个选择: 不要最后一个 或 不要第一个
        return max(robLinear(nums, 0, n-2), robLinear(nums, 1, n-1));
    }
};

int main() {
    Solution s;
    vector<int> n1 = {2,3,2};
    cout << s.rob(n1) << " (expect 3)" << endl;
    vector<int> n2 = {1,2,3,1};
    cout << s.rob(n2) << " (expect 4)" << endl;
    vector<int> n3 = {1};
    cout << s.rob(n3) << " (expect 1)" << endl;
    return 0;
}
