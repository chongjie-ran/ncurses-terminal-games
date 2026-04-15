// LC22 Generate Parentheses - 2026-04-15 07:08
// 回溯：左右括号数量平衡
#include <iostream>
#include <vector>
#include <string>
#include <functional>
using namespace std;

class Solution {
public:
    vector<string> generateParenthesis(int n) {
        vector<string> result;
        string path;
        function<void(int,int)> backtrack = [&](int left, int right) {
            if (path.size() == 2 * n) {
                result.push_back(path);
                return;
            }
            if (left < n) {
                path.push_back('(');
                backtrack(left + 1, right);
                path.pop_back();
            }
            if (right < left) {
                path.push_back(')');
                backtrack(left, right + 1);
                path.pop_back();
            }
        };
        backtrack(0, 0);
        return result;
    }
};

int main() {
    Solution s;
    auto r = s.generateParenthesis(3);
    cout << "n=3: " << r.size() << " combinations" << endl;
    for (auto& str : r) cout << str << endl;
    
    auto r2 = s.generateParenthesis(1);
    cout << "\nn=1: " << r2.size() << " combinations" << endl;
    for (auto& str : r2) cout << str << endl;
    
    auto r4 = s.generateParenthesis(4);
    cout << "\nn=4: " << r4.size() << " combinations (expected 14)" << endl;
    return 0;
}
