// LC17 Letter Combinations of a Phone Number - Backtracking
#include <iostream>
#include <vector>
#include <string>
using namespace std;

const string KEYPAD[] = {"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};

void backtrack(const string& digits, int idx, string& cur, vector<string>& result) {
    if (idx == (int)digits.size()) {
        if (!cur.empty()) result.push_back(cur);
        return;
    }
    int digit = digits[idx] - '0';
    for (char c : KEYPAD[digit]) {
        cur.push_back(c);
        backtrack(digits, idx + 1, cur, result);
        cur.pop_back();
    }
}

vector<string> letterCombinations(string digits) {
    vector<string> result;
    string cur;
    backtrack(digits, 0, cur, result);
    return result;
}

int main() {
    vector<string> r1 = letterCombinations("23");
    cout << "Test1 ('23'): " << r1.size() << " (expected 9)" << endl;
    for (string s : r1) cout << "  " << s << endl;
    
    vector<string> r2 = letterCombinations("");
    cout << "Test2 (''): " << r2.size() << " (expected 0)" << endl;
    
    vector<string> r3 = letterCombinations("2");
    cout << "Test3 ('2'): " << r3.size() << " (expected 3)" << endl;
    
    return 0;
}
