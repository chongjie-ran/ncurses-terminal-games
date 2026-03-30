// LC224 基本计算器 — 栈处理括号和加减
#include <iostream>
#include <vector>
#include <stack>
#include <string>
using namespace std;

int calculate(string s) {
    int n = s.size();
    long long sign = 1, num = 0, result = 0;
    stack<long long> st;
    for (int i = 0; i < n; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
        } else if (c == '+') {
            result += sign * num;
            sign = 1; num = 0;
        } else if (c == '-') {
            result += sign * num;
            sign = -1; num = 0;
        } else if (c == '(') {
            st.push(result);
            st.push(sign);
            result = 0; sign = 1;
        } else if (c == ')') {
            result += sign * num;
            long long preSign = st.top(); st.pop();
            long long preResult = st.top(); st.pop();
            result = preResult + preSign * result;
            num = 0;
        }
    }
    result += sign * num;
    return (int)result;
}

int main() {
    cout << calculate("1 + 1") << endl;       // 2
    cout << calculate(" 2-1 + 2 ") << endl;  // 3
    cout << calculate("(1+(4+5+2)-3)+(6+8)") << endl; // 23
    return 0;
}
