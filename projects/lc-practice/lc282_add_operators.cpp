// LC282 Expression Add Operators
// 难度: Hard
// 核心: DFS回溯 + 乘法处理
// 思路: 
//   - 枚举每个数字的分割点和运算符
//   - 乘法处理: '*'优先级高于'+'，需记录当前表达式值和最后一个数
//   - 当前值 = 之前累计 - lastNum + lastNum * curNum

#include <iostream>
#include <vector>
#include <string>
using namespace std;

void dfs(const string& num, int target, long long curVal, long long lastNum,
         const string& expr, vector<string>& result, int pos) {
    if (pos == num.size()) {
        if (curVal == target) result.push_back(expr);
        return;
    }
    
    for (int i = pos; i < num.size(); i++) {
        string curStr = num.substr(pos, i - pos + 1);
        long long curNum = stoll(curStr);
        
        if (pos == 0) {
            dfs(num, target, curNum, curNum, curStr, result, i + 1);
        } else {
            // +
            dfs(num, target, curVal + curNum, curNum,
                expr + "+" + curStr, result, i + 1);
            // -
            dfs(num, target, curVal - curNum, -curNum,
                expr + "-" + curStr, result, i + 1);
            // *
            long long newVal = curVal - lastNum + lastNum * curNum;
            dfs(num, target, newVal, lastNum * curNum,
                expr + "*" + curStr, result, i + 1);
        }
        
        // 避免前导0（如"05"）
        if (num[pos] == '0') break;
    }
}

vector<string> addOperators(string num, int target) {
    vector<string> result;
    if (num.empty()) return result;
    dfs(num, target, 0, 0, "", result, 0);
    return result;
}

int main() {
    // 测试1: "123", target=6 -> ["1*2*3","1+2+3"]
    auto r1 = addOperators("123", 6);
    cout << "123=6: ";
    for (auto& s : r1) cout << s << " "; cout << endl;
    
    // 测试2: "232", target=8 -> ["2*3+2","2+3*2"]
    auto r2 = addOperators("232", 8);
    cout << "232=8: ";
    for (auto& s : r2) cout << s << " "; cout << endl;
    
    // 测试3: "105", target=5 -> ["1*0+5","10-5"]
    auto r3 = addOperators("105", 5);
    cout << "105=5: ";
    for (auto& s : r3) cout << s << " "; cout << endl;
    
    // 测试4: "00", target=0 -> ["0+0","0-0","0*0"]
    auto r4 = addOperators("00", 0);
    cout << "00=0: ";
    for (auto& s : r4) cout << s << " "; cout << endl;
    
    // 测试5: "3456237490", target=9191
    auto r5 = addOperators("3456237490", 9191);
    cout << "3456237490=9191: ";
    for (auto& s : r5) cout << s << " "; cout << endl;
    
    return 0;
}
