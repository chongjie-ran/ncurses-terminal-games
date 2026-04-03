// LC22: Generate Parentheses
// Pattern: Backtracking / Recursion / Stack
// Difficulty: Medium
// Date: 2026-04-01

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Backtracking approach
// Key insight: at each step, can add '(' if open < n, add ')' if close < open
void backtrack(vector<string>& result, string& current, int open, int close, int n) {
    if (current.size() == n * 2) {
        result.push_back(current);
        return;
    }
    
    // Can add '(' if we haven't used all n
    if (open < n) {
        current.push_back('(');
        backtrack(result, current, open + 1, close, n);
        current.pop_back();
    }
    
    // Can add ')' if we have more open than close
    if (close < open) {
        current.push_back(')');
        backtrack(result, current, open, close + 1, n);
        current.pop_back();
    }
}

vector<string> generateParenthesis(int n) {
    vector<string> result;
    string current;
    backtrack(result, current, 0, 0, n);
    return result;
}

int main() {
    // Test n=3 (standard)
    auto r3 = generateParenthesis(3);
    cout << "n=3: " << r3.size() << " combinations" << endl;
    for (auto& s : r3) cout << "  " << s << endl;
    
    // Test n=1
    auto r1 = generateParenthesis(1);
    cout << "n=1: ";
    for (auto& s : r1) cout << s << " ";
    cout << endl;
    
    // Catlan number check: C_n = (2n)!/(n!(n+1)!)
    // n=3: C_3 = 5 ✓
    cout << "\nCatalan numbers: n=1->1, n=2->2, n=3->5, n=4->14" << endl;
    
    return 0;
}
