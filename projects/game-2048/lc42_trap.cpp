/**
 * LeetCode 42 - Trapping Rain Water (接雨水)
 * 
 * 给定 n 个非负整数表示高度图，计算能接多少雨水。
 * 
 * 示例:
 *   输入: [0,1,0,2,1,0,1,3,2,1,2,1]
 *   输出: 6
 * 
 * 核心思想: 单调递减栈
 * - 维护一个单调递减的高度栈
 * - 当遇到比栈顶高的柱形时，形成凹槽，计算水量
 */

#include <iostream>
#include <vector>
#include <stack>

using namespace std;

/**
 * 方法1: 单调递减栈
 * 
 * 栈中存索引，高度单调递减
 * 当遇到比栈顶高的柱形时：
 *   - 弹出栈顶（形成凹槽的底）
 *   - 计算凹槽水量：min(左边柱, 当前柱) - 弹出柱高度
 */
int trap_monotone_stack(const vector<int>& heights) {
    stack<int> st;  // 存索引
    int water = 0;
    int n = heights.size();
    
    for (int i = 0; i < n; ++i) {
        while (!st.empty() && heights[i] > heights[st.top()]) {
            int bottom_idx = st.top();
            st.pop();
            
            // 如果弹出后栈为空，说明左边没有柱子，不能积水
            if (st.empty()) continue;
            
            // 左边柱子的索引（栈顶）
            int left_idx = st.top();
            // 当前柱子的索引
            int right_idx = i;
            
            // 凹槽宽度
            int width = right_idx - left_idx - 1;
            // 凹槽高度 = min(左柱, 右柱) - 底柱
            int height = min(heights[left_idx], heights[right_idx]) - heights[bottom_idx];
            
            water += width * height;
        }
        st.push(i);
    }
    
    return water;
}

/**
 * 方法2: 双指针（最优，O(1) 空间）
 * 
 * 核心洞察：
 * - 每个位置能接的水量，由其左右最高柱中的较矮者决定
 * - left_max[i] = max(height[0..i])
 * - right_max[i] = max(height[i..n-1])
 * - water[i] = min(left_max[i], right_max[i]) - height[i]
 * 
 * 双指针优化：维护 left_max 和 right_max 的较小者
 */
int trap_two_pointer(const vector<int>& heights) {
    int water = 0;
    int left = 0, right = heights.size() - 1;
    int left_max = 0, right_max = 0;
    
    while (left < right) {
        if (heights[left] < heights[right]) {
            // 左边较低，处理左边
            if (heights[left] >= left_max) {
                left_max = heights[left];  // 更新左边最高
            } else {
                water += left_max - heights[left];  // 能接水
            }
            ++left;
        } else {
            // 右边较低，处理右边
            if (heights[right] >= right_max) {
                right_max = heights[right];
            } else {
                water += right_max - heights[right];
            }
            --right;
        }
    }
    
    return water;
}

/**
 * 手动模拟: heights = [0,1,0,2,1,0,1,3,2,1,2,1]
 * 
 *  i   height   stack(存idx)     操作                   water
 *  --  ------   -------------   ---------------------  -----
 *  0     0       [0]            入栈                    0
 *  1     1       [0,1]          heights[1]=1 > 0, 入栈  0
 *  2     0       [0,1,2]        入栈                    0
 *  3     2       [0,1,2,3]      heights[3]=2 > 0, 入栈 0
 *  4     1       [0,1,3]        pop 2, 底=0, 左=1, 右=3
 *                                    width=3-1-1=1, h=min(1,2)-0=1
 *                                    water+=1                 1
 *                                  入栈 4                   1
 *  5     0       [0,1,3,4]       入栈                    1
 *  6     1       [0,1,3,6]      pop 4, 底=1, 左=3, 右=1
 *                                    width=6-3-1=2, h=min(2,1)-1=0
 *                                    water+=0                 1
 *                                  pop 6? heights[6]=1 not > heights[3]=2
 *                                  入栈 6                     1
 *  7     3       [0,1,7]        pop 6, 底=1, 左=3, 右=7
 *                                    width=7-3-1=3, h=min(2,3)-1=1
 *                                    water+=3                  4
 *                                  pop 1? heights[7]=3 > heights[1]=1
 *                                  pop 0? heights[7]=3 > heights[0]=0
 *                                  入栈 7                      4
 *  ...以此类推
 * 
 * 最终结果: water = 6
 */

// 测试
int main() {
    vector<vector<int>> test_cases = {
        {0,1,0,2,1,0,1,3,2,1,2,1},  // expected: 6
        {4,2,0,3,2,5},              // expected: 9
        {1,0,1},                     // expected: 1
        {3,0,2,0,4},                 // expected: 7
        {},                           // expected: 0
        {1},                          // expected: 0
    };
    
    for (const auto& heights : test_cases) {
        int result1 = trap_monotone_stack(heights);
        int result2 = trap_two_pointer(heights);
        cout << "heights: [";
        for (int i = 0; i < heights.size(); ++i) {
            cout << heights[i];
            if (i < heights.size() - 1) cout << ",";
        }
        cout << "] -> water: " << result1 << " (stack), " << result2 << " (two-pointer)" << endl;
    }
    
    return 0;
}
