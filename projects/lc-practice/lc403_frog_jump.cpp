#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
using namespace std;

// LC403 Frog Jump
// Hard: 判断青蛙能否过河（最后一块石头）
// 思路：DFS + 记忆化。状态 (i, k) = 在第i块石头上，上次跳了k步
// 从右往左DP，记录可达状态

class Solution {
public:
    bool canCross(vector<int>& stones) {
        int n = stones.size();
        if (n < 2) return true;
        if (stones[1] != 1) return false;
        if (n == 2) return true;

        // 记忆化: stone_index -> set of reachable step sizes
        unordered_map<int, unordered_set<int>> memo;
        for (int i = 0; i < n; i++) {
            memo[i] = unordered_set<int>();
        }
        memo[1].insert(1);

        for (int i = 1; i < n; i++) {
            for (int k : memo[i]) {
                // 尝试 k-1, k, k+1
                for (int dk = -1; dk <= 1; dk++) {
                    int nk = k + dk;
                    if (nk <= 0) continue;
                    int next = stones[i] + nk;
                    // 二分查找 next
                    int lo = i+1, hi = n-1;
                    while (lo <= hi) {
                        int mid = (lo+hi)/2;
                        if (stones[mid] == next) {
                            memo[mid].insert(nk);
                            break;
                        } else if (stones[mid] < next) {
                            lo = mid+1;
                        } else {
                            hi = mid-1;
                        }
                    }
                }
            }
        }

        return !memo[n-1].empty();
    }
};

int main() {
    Solution s;
    vector<int> stones1 = {0,1,3,5,6,8,12,17};
    cout << "Test1 (true): " << s.canCross(stones1) << endl;

    vector<int> stones2 = {0,1,2,3,4,8,9,11};
    cout << "Test2 (false): " << s.canCross(stones2) << endl;

    vector<int> stones3 = {0,2};
    cout << "Test3 (false): " << s.canCross(stones3) << endl;

    return 0;
}
