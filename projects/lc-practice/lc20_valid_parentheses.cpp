#include <iostream>
#include <stack>
#include <string>
using namespace std;
// LC20 有效括号 - 栈匹配
bool isValid(string s) {
    stack<char> st;
    for (char c : s) {
        if (c=='('||c=='['||c=='{') st.push(c);
        else {
            if (st.empty()) return false;
            char t = st.top(); st.pop();
            if ((c==')'&&t!='(')||(c==']'&&t!='[')||(c=='}'&&t!='{')) return false;
        }
    }
    return st.empty();
}
int main() {
    cout << isValid("()") << endl;      // 1
    cout << isValid("()[]{}") << endl;  // 1
    cout << isValid("(]") << endl;      // 0
    cout << isValid("([)]") << endl;     // 0
    cout << isValid("{[]}") << endl;     // 1
    cout << isValid("") << endl;        // 1
    cout << isValid("]") << endl;        // 0
}
