#include <iostream>
#include <vector>
using namespace std;

// LC42 Trapping Rain Water
// Hard: 接雨水 - 两指针法
// 思路: 左右双指针，从两端向中间收缩
// 时间: O(N), 空间: O(1)

class Solution {
public:
    int trap(vector<int>& h) {
        int n = h.size();
        if (n == 0) return 0;
        long long ans = 0;
        int L = 0, R = n - 1;
        int leftMax = 0, rightMax = 0;

        while (L < R) {
            if (h[L] <= h[R]) {
                if (h[L] >= leftMax) {
                    leftMax = h[L];
                } else {
                    ans += leftMax - h[L];
                }
                L++;
            } else {
                if (h[R] >= rightMax) {
                    rightMax = h[R];
                } else {
                    ans += rightMax - h[R];
                }
                R--;
            }
        }
        return (int)ans;
    }
};

int main() {
    Solution s;
    vector<int> h1 = {0,1,0,2,1,0,1,3,2,1,2,1};
    cout << "Test1 (6): " << s.trap(h1) << endl;

    vector<int> h2 = {4,2,0,3,2,5};
    cout << "Test2 (9): " << s.trap(h2) << endl;

    vector<int> h3 = {1,0,1};
    cout << "Test3 (1): " << s.trap(h3) << endl;

    return 0;
}
