#include <iostream>
#include <vector>
#include <stack>
using namespace std;

// LC496 下一个更大元素I - 单调栈模板
vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
    vector<int> ans;
    stack<int> s;
    for (int i = 0; i < nums2.size(); i++) {
        while (!s.empty() && nums2[s.top()] < nums2[i]) {
            s.pop();
        }
        s.push(i);
    }
    return ans;
}

// LC739 每日温度 - 单调递减栈
vector<int> dailyTemperatures(vector<int>& temperatures) {
    vector<int> ans(temperatures.size(), 0);
    stack<int> s; // 存索引
    for (int i = 0; i < temperatures.size(); i++) {
        while (!s.empty() && temperatures[i] > temperatures[s.top()]) {
            int prev = s.top(); s.pop();
            ans[prev] = i - prev;
        }
        s.push(i);
    }
    return ans;
}

// LC84 柱状图中最大的矩形 - 单调递增栈
int largestRectangleArea(vector<int>& heights) {
    stack<int> s;
    int maxArea = 0;
    for (int i = 0; i <= heights.size(); i++) {
        int cur = (i == heights.size()) ? 0 : heights[i];
        while (!s.empty() && cur < heights[s.top()]) {
            int h = heights[s.top()]; s.pop();
            int w = s.empty() ? i : i - s.top() - 1;
            maxArea = max(maxArea, h * w);
        }
        s.push(i);
    }
    return maxArea;
}

// LC42 接雨水 - 单调递减栈
int trap(vector<int>& height) {
    stack<int> s;
    int water = 0;
    for (int i = 0; i < height.size(); i++) {
        while (!s.empty() && height[i] > height[s.top()]) {
            int bottom = s.top(); s.pop();
            if (s.empty()) continue;
            int w = i - s.top() - 1;
            int h = min(height[i], height[s.top()]) - height[bottom];
            water += w * h;
        }
        s.push(i);
    }
    return water;
}

// LC85 最大矩形 - 逐行应用LC84
int maximalRectangle(vector<vector<char>>& matrix) {
    if (matrix.empty()) return 0;
    vector<int> heights(matrix[0].size(), 0);
    int maxArea = 0;
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix[0].size(); j++) {
            heights[j] = (matrix[i][j] == '1') ? heights[j] + 1 : 0;
        }
        // 应用LC84
        stack<int> s;
        for (int j = 0; j <= heights.size(); j++) {
            int cur = (j == heights.size()) ? 0 : heights[j];
            while (!s.empty() && cur < heights[s.top()]) {
                int h = heights[s.top()]; s.pop();
                int w = s.empty() ? j : j - s.top() - 1;
                maxArea = max(maxArea, h * w);
            }
            s.push(j);
        }
    }
    return maxArea;
}

int main() {
    // LC739 测试
    vector<int> temps = {73,74,75,71,69,72,76,73};
    auto res = dailyTemperatures(temps);
    cout << "LC739 每日温度: ";
    for (int x : res) cout << x << " "; // 1 1 4 2 1 1 0 0
    cout << endl;
    
    // LC84 测试
    vector<int> heights = {2,1,5,6,2,3};
    cout << "LC84 最大矩形: " << largestRectangleArea(heights) << " (期望10)" << endl;
    
    // LC42 测试
    vector<int> trapH = {0,1,0,2,1,0,1,3,2,1,2,1};
    cout << "LC42 接雨水: " << trap(trapH) << " (期望6)" << endl;
    
    // LC85 测试
    vector<vector<char>> matrix = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    cout << "LC85 最大矩形: " << maximalRectangle(matrix) << " (期望6)" << endl;
    
    return 0;
}
