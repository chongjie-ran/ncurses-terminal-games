// LC224 Basic Calculator - Cleaner Version
// Hard - Stack
#include <iostream>
#include <stack>
#include <string>
using namespace std;

int calculate(string s) {
    int n = s.size();
    long long num = 0;
    long long result = 0;
    int sign = 1;        // 当前项的符号
    stack<int> st;       // 每个括号的sign上下文
    st.push(1);

    for (int i = 0; i < n; i++) {
        char c = s[i];

        if (c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
        } else if (c == '+' || c == '-') {
            // 累积之前的数字
            result += sign * num;
            num = 0;
            sign = (c == '+' ? 1 : -1) * st.top();
        } else if (c == '(') {
            // 把当前 sign 压栈, 作为括号内的新上下文
            st.push(sign);
        } else if (c == ')') {
            // 累积括号内的最后一个数字
            result += sign * num;
            num = 0;
            st.pop();
            // sign 保持,但上下文变成新的 st.top()
            sign = st.top();
        }
        // 跳过空格
    }

    // 最后一个数字
    result += sign * num;
    return (int)result;
}

int main() {
    auto test = [](const string& s, int expected) {
        int ans = calculate(s);
        cout << "\"" << s << "\" = " << ans << " (expected " << expected << ")";
        if (ans == expected) cout << " ✅" << endl;
        else cout << " ❌" << endl;
    };

    test("1 + 1", 2);
    test(" 2-1 + 2 ", 3);
    test("(1+(4+5+2)-3)+(6+8)", 23);
    test("1-(5)", -4);
    test("-2+ 1", -1);
    test("(1-(3-4)+(5+6)-(7-8-(9-1)))", 18);

    return 0;
}
