// LC201 Bitwise AND of Numbers Range
// 题目: [m, n] 范围内所有数字按位与的结果
// 核心: 找公共前缀，将 m 和 n 右移直到相等，记录移位次数，再左移回来
// 时间: O(log(n-m)) 空间: O(1)

#include <iostream>
using namespace std;

class Solution {
public:
    int rangeBitwiseAnd(int m, int n) {
        int shift = 0;
        while (m != n) {
            m >>= 1;
            n >>= 1;
            ++shift;
        }
        return m << shift;
    }
};

int main() {
    Solution s;
    cout << s.rangeBitwiseAnd(5, 7) << endl;   // 期望: 4
    cout << s.rangeBitwiseAnd(0, 0) << endl;   // 期望: 0
    cout << s.rangeBitwiseAnd(1, 3) << endl;   // 期望: 0
    cout << s.rangeBitwiseAnd(16, 24) << endl; // 期望: 16
    cout << s.rangeBitwiseAnd(12, 15) << endl; // 期望: 8
    return 0;
}
