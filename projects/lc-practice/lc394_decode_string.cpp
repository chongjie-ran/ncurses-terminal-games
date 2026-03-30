// LC394 字符串解码 — 栈
// s = "3[a2[c]]" → "accaccacc"
#include <iostream>
#include <stack>
#include <string>
using namespace std;

string decodeString(string s) {
    stack<int> countSt;
    stack<string> strSt;
    string cur = "";
    int num = 0;
    for (char c : s) {
        if (c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
        } else if (c == '[') {
            countSt.push(num);
            strSt.push(cur);
            num = 0; cur = "";
        } else if (c == ']') {
            int k = countSt.top(); countSt.pop();
            string prev = strSt.top(); strSt.pop();
            string repeated = "";
            for (int i = 0; i < k; i++) repeated += cur;
            cur = prev + repeated;
        } else {
            cur += c;
        }
    }
    return cur;
}

int main() {
    cout << decodeString("3[a2[c]]") << endl;       // accaccacc
    cout << decodeString("2[abc]3[cd]ef") << endl;  // abcabccdcdcdef
    return 0;
}
