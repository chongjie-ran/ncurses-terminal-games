// LC233 Number of Digit One
// 难度: Hard
// 核心: 数位DP/数学分析
// 思路: 统计1到n中所有数字里出现1的总次数
// 公式: 对于每一位i (从低位到高位)
//   - high = n / (factor * 10)   // 高位部分
//   - cur = (n / factor) % 10    // 当前位
//   - low = n % factor           // 低位部分
//   - 贡献 = high * factor + (cur==1 ? low+1 : cur>1 ? factor : 0)

#include <iostream>
using namespace std;

int countDigitOne(int n) {
    long long count = 0;
    long long factor = 1;  // 10^i，当前位数权重
    
    while (factor <= n) {
        long long high = n / (factor * 10);
        long long cur = (n / factor) % 10;
        long long low = n % factor;
        
        if (cur == 0) {
            count += high * factor;
        } else if (cur == 1) {
            count += high * factor + low + 1;
        } else {  // cur >= 2
            count += (high + 1) * factor;
        }
        
        factor *= 10;
    }
    return static_cast<int>(count);
}

int main() {
    // 测试用例
    cout << "0 -> " << countDigitOne(0) << " (期望0)" << endl;
    cout << "1 -> " << countDigitOne(1) << " (期望1)" << endl;
    cout << "9 -> " << countDigitOne(9) << " (期望1)" << endl;
    cout << "10 -> " << countDigitOne(10) << " (期望2)" << endl;
    cout << "13 -> " << countDigitOne(13) << " (期望6)" << endl;
    cout << "20 -> " << countDigitOne(20) << " (期望12)" << endl;
    cout << "100 -> " << countDigitOne(100) << " (期望21)" << endl;
    cout << "1000 -> " << countDigitOne(1000) << " (期望192)" << endl;
    cout << "10000000 -> " << countDigitOne(10000000) << endl;
    
    return 0;
}
