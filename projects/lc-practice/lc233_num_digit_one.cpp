/*
LC233 数字1的个数
Hard - 数位DP

给定一个整数 n，计算所有从 0 到 n 之间数字中出现'1'的个数。

思路：按位统计
对每一位，分析该位为1时，前缀和后缀的取值范围
- high = n / (factor * 10)      // 前缀
- cur = (n / factor) % 10       // 当前位
- low = n % factor               // 后缀

当前位为1时： possibilities = high * factor
当前位>1时： possibilities = (high+1) * factor
当前位=0时： possibilities = high * factor
*/

#include <iostream>
using namespace std;

long long countDigitOne(int n) {
    long long count = 0;
    long long factor = 1; // 当前位数因子（1,10,100,...）

    while (factor <= n) {
        long long high = n / (factor * 10);
        long long cur = (n / factor) % 10;
        long long low = n % factor;

        if (cur == 0) {
            // 当前位为0：1出现在该位的情况 = high * factor
            count += high * factor;
        } else if (cur == 1) {
            // 当前位为1：= high * factor + low + 1
            count += high * factor + low + 1;
        } else {
            // cur > 1：= (high+1) * factor
            count += (high + 1) * factor;
        }

        factor *= 10;
    }
    return count;
}

int main() {
    int tests[] = {0, 1, 13, 100, 123, 99999999};
    for (int n : tests) {
        cout << "n=" << n << " => " << countDigitOne(n) << endl;
    }
    // 验证已知答案: 
    // n=13: [1,10,11,12,13] → 1,10,11,12,13 中1的个数 = 6
    // n=100: 应该包含所有100以内数字的1 → 21
    return 0;
}
