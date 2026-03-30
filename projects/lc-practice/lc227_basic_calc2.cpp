// LC227 基本计算器 II — 栈处理乘除加减（无括号）
#include <iostream>
#include <vector>
#include <stack>
#include <string>
using namespace std;

int calculate(string s) {
    long long num = 0;
    char op = '+';
    stack<long long> st;
    for (int i = 0; i <= (int)s.size(); i++) {
        char c = i < (int)s.size() ? s[i] : '+';
        if (c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
        } else if (c == ' ' && i < (int)s.size()) {
            continue;
        } else {
            if (op == '+') st.push(num);
            else if (op == '-') st.push(-num);
            else if (op == '*') {
                long long top = st.top(); st.pop();
                st.push(top * num);
            } else if (op == '/') {
                long long top = st.top(); st.pop();
                st.push(top / num);
            }
            op = c;
            num = 0;
        }
    }
    long long result = 0;
    while (!st.empty()) { result += st.top(); st.pop(); }
    return (int)result;
}

int main() {
    cout << calculate("3+2*2") << endl;        // 7
    cout << calculate(" 3/2 ") << endl;        // 1
    cout << calculate("3+5/2") << endl;       // 5
    return 0;
}
