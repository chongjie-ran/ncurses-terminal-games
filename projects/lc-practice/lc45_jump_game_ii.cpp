// LC45 Jump Game II - 最少跳跃次数
// 贪心：每次在当前可达范围内，选择能跳最远的
#include <iostream>
#include <vector>
using namespace std;

int jump(vector<int>& nums) {
    if (nums.size() <= 1) return 0;
    
    int jumps = 0;
    int curEnd = 0;      // 当前跳跃的覆盖范围末尾
    int farthest = 0;    // 下一步能跳到的最远位置
    
    for (int i = 0; i < nums.size() - 1; i++) {
        farthest = max(farthest, i + nums[i]);
        if (i == curEnd) {        // 到达当前覆盖边界
            jumps++;
            curEnd = farthest;
        }
    }
    return jumps;
}

int main() {
    vector<int> t1 = {2,3,1,1,4};
    vector<int> t2 = {2,3,0,1,4};
    vector<int> t3 = {1,2,1,1,1};
    
    cout << jump(t1) << endl;  // 2 (0→1→4)
    cout << jump(t2) << endl;  // 2 (0→1→4)
    cout << jump(t3) << endl;  // 3 (0→2→3→4)
}
