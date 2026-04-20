// LC907 Sum of Subarray Minimums
// 核心：每个元素作为最小值的子数组数量 = (i-PLE) * (NLE-i)
// 使用单调递增栈同时计算PLE和NLE
#include <vector>
#include <stack>
#include <iostream>
using namespace std;

class Solution {
public:
    int sumSubarrayMins(vector<int>& arr) {
        const long long MOD = 1e9 + 7;
        int n = arr.size();
        vector<int> left(n), right(n);
        stack<int> st;

        // PLE: 上一个更小元素的索引
        for (int i = 0; i < n; i++) {
            while (!st.empty() && arr[st.top()] > arr[i]) st.pop();
            left[i] = st.empty() ? i + 1 : i - st.top();
            st.push(i);
        }

        // 清空栈
        while (!st.empty()) st.pop();

        // NLE: 下一个更小或相等元素的索引（关键：用 >= 保证不重复计算）
        for (int i = n - 1; i >= 0; i--) {
            while (!st.empty() && arr[st.top()] >= arr[i]) st.pop();
            right[i] = st.empty() ? n - i : st.top() - i;
            st.push(i);
        }

        long long ans = 0;
        for (int i = 0; i < n; i++) {
            ans = (ans + 1LL * arr[i] * left[i] * right[i]) % MOD;
        }
        return (int)ans;
    }
};

// 测试框架
int main() {
    Solution sol;
    vector<vector<int>> cases = {
        {1, 2, 3},        // 10
        {3, 1, 2, 4},    // 17
        {11,81,94,43,3}, // 444
        {2,9,7,5,1,3,7,1,9}, // 99
        {1,2,3,4,5},     // 35
        {5,4,3,2,1},     // 35
        {3,3,3,3},       // 30
    };
    vector<int> expects = {10, 17, 444, 99, 35, 35, 30};

    for (int i = 0; i < cases.size(); i++) {
        long long result = sol.sumSubarrayMins(cases[i]);
        cout << "Case " << i << ": " << result << " (expect " << expects[i] << ") "
             << (result == expects[i] ? "✅" : "❌") << "\n";
    }
    return 0;
}
