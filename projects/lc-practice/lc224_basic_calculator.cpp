// LC224 基本计算器 - 栈实现
// 实现字符串表达式的括号处理
#include <iostream>
#include <stack>
#include <string>
#include <cctype>
using namespace std;

class Solution {
public:
    int calculate(string s) {
        stack<long long> signs;
        long long sign = 1, result = 0, num = 0;
        signs.push(1);
        
        for (char c : s) {
            if (c >= '0' && c <= '9') {
                num = num * 10 + (c - '0');
            } else if (c == '+' || c == '-') {
                result += sign * num;
                sign = signs.top() * (c == '+' ? 1 : -1);
                num = 0;
            } else if (c == '(') {
                signs.push(sign);
                num = 0;
            } else if (c == ')') {
                result += sign * num;
                sign = signs.top();
                signs.pop();
                num = 0;
            }
        }
        result += sign * num;
        return (int)result;
    }
};

// 测试
int main() {
    Solution sol;
    vector<string> tests = {
        "1 + 1",           // 2
        " 2-1 + 2 ",       // 3
        "(1+(4+5+2)-3)+(6+8)", // 23
        "((2+1))",         // 3
        "1-(5)",           // -4
    };
    for (const auto& t : tests) {
        cout << "\"" << t << "\" = " << sol.calculate(t) << endl;
    }
    return 0;
}
