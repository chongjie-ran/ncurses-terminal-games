// LC227 Basic Calculator II
// Medium - Stack
// 核心: 处理 + - * /, 没有括号
// 思路: 上一个操作数入栈, 遇到 * / 弹出栈顶计算后压回
// 最后栈中所有元素求和

#include <iostream>
#include <stack>
#include <string>
using namespace std;

int calculate(string s) {
    int n = s.size();
    long long num = 0;
    char op = '+';  // 上一个操作符
    stack<long long> st;

    for (int i = 0; i < n; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
        }

        if ((c == '+' || c == '-' || c == '*' || c == '/' || i == n - 1)) {
            if (op == '+') {
                st.push(num);
            } else if (op == '-') {
                st.push(-num);
            } else if (op == '*') {
                long long top = st.top(); st.pop();
                st.push(top * num);
            } else if (op == '/') {
                long long top = st.top(); st.pop();
                st.push(top / num);
            }
            num = 0;
            if (i < n - 1) op = c;
        }
    }

    long long result = 0;
    while (!st.empty()) {
        result += st.top(); st.pop();
    }
    return (int)result;
}

int main() {
    auto test = [&](const string& s, int expected) {
        int ans = calculate(s);
        cout << "\"" << s << "\" = " << ans << " (expected " << expected << ")";
        if (ans == expected) cout << " ✅" << endl;
        else cout << " ❌" << endl;
    };

    test("3+2*2", 7);
    test(" 3/2 ", 1);
    test(" 3+5 / 2 ", 5);
    test("1+2*5/3+6/4*2/3+1", 4);
    test("0", 0);
    test("100000000/1/2/3/4/5/6/7/8/9/10", 0); // integer division floors

    return 0;
}
