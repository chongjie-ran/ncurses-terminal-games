#include <iostream>
using namespace std;

int countDigitOne(int n) {
    long long count = 0;
    long long factor = 1;
    while (factor <= n) {
        long long high = n / (factor * 10);
        long long cur = (n / factor) % 10;
        long long low = n % factor;
        if (cur == 0) count += high * factor;
        else if (cur == 1) count += high * factor + low + 1;
        else count += (high + 1) * factor;
        factor *= 10;
    }
    return static_cast<int>(count);
}

int main() {
    // 暴力验证
    auto brute = [](int n) {
        int count = 0;
        for (int i = 1; i <= n; i++) {
            int x = i;
            while (x > 0) { if (x % 10 == 1) count++; x /= 10; }
        }
        return count;
    };
    
    // 验证
    for (int n : {0, 1, 9, 10, 13, 20, 100, 1000, 10000, 100000}) {
        int fast = countDigitOne(n);
        int slow = brute(n);
        cout << "n=" << n << " fast=" << fast << " slow=" << slow 
             << (fast==slow?" OK":" FAIL") << endl;
    }
    return 0;
}
