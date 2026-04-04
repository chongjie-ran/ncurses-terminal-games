// LC503. Next Greater Element II
// Medium - Monotonic Stack + 环状数组
// O(n) time, O(n) space

#include <iostream>
#include <vector>
#include <stack>
using namespace std;

// 环状数组技巧: 遍历2n长度,取模访问
// 栈内存放元素下标,维护递减栈(存的是"下一个更大"的候选)
vector<int> nextGreaterElements(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> st; // 存下标,栈内对应的nums值递减
    
    for (int i = 0; i < 2 * n; i++) {
        int cur = nums[i % n];
        while (!st.empty() && nums[st.top()] < cur) {
            result[st.top()] = cur;
            st.pop();
        }
        if (i < n) st.push(i); // 只在第一轮入栈,第二轮只做匹配
    }
    return result;
}

int main() {
    vector<int> n1 = {1,2,1};
    auto r1 = nextGreaterElements(n1);
    for (int x : r1) cout << x << " "; // 2  -1  2
    cout << endl;
    
    vector<int> n2 = {1,2,3,4,3};
    auto r2 = nextGreaterElements(n2);
    for (int x : r2) cout << x << " "; // 2  3  -1  -1  4
    cout << endl;
    
    return 0;
}

/*
核心模式:
1. 环状数组 → 遍历2n长度,i% n访问
2. 单调递减栈: 栈顶是最近的较大元素位置
3. 遇到更大值 → 弹出并设置next greater
4. 只在第一轮(i < n)入栈,避免重复

复杂度: O(n) time, O(n) space
为什么正确: 2n遍历保证所有元素都和环内所有元素比较过
*/
