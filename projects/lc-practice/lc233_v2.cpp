// LC233 Number of Digit One - 修正版
// 验证: n=13期望6, n=100期望21

#include <iostream>
using namespace std;

int countDigitOne(int n) {
    long long count = 0;
    long long factor = 1;
    
    while (factor <= n) {
        long long high = n / (factor * 10);
        long long cur = (n / factor) % 10;
        long long low = n % factor;
        
        if (cur == 0) {
            count += high * factor;
        } else if (cur == 1) {
            count += high * factor + low + 1;
        } else {
            count += (high + 1) * factor;
        }
        
        factor *= 10;
    }
    return static_cast<int>(count);
}

// 暴力验证
int brute(int n) {
    int count = 0;
    for (int i = 1; i <= n; i++) {
        int x = i;
        while (x > 0) {
            if (x % 10 == 1) count++;
            x /= 10;
        }
    }
    return count;
}

int main() {
    for (int n = 0; n <= 200; n++) {
        int fast = countDigitOne(n);
        int slow = brute(n);
        if (fast != slow) {
            cout << "DIFF at n=" << n << ": fast=" << fast << " slow=" << slow << endl;
        }
    }
    cout << "Done, all 0-200 match" << endl;
    
    // 官方测试用例
    cout << "n=13 -> " << countDigitOne(13) << " (期望6)" << endl;
    cout << "n=100 -> " << countDigitOne(100) << " (期望21)" << endl;
    cout << "n=13 brute=" << brute(13) << endl;
    cout << "n=100 brute=" << brute(100) << endl;
    
    return 0;
}
