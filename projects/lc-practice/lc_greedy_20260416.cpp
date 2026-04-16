// LC贪心算法专题 - 2026-04-16 Evening
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

// LC55 跳跃游戏 - Medium
bool canJump(vector<int>& nums) {
    int farthest = 0;
    for (int i = 0; i < nums.size(); i++) {
        if (i > farthest) return false;
        farthest = max(farthest, i + nums[i]);
        if (farthest >= nums.size() - 1) return true;
    }
    return true;
}

// LC45 跳跃游戏II - Hard
int jump(vector<int>& nums) {
    int jumps = 0, curEnd = 0, farthest = 0;
    for (int i = 0; i < nums.size() - 1; i++) {
        farthest = max(farthest, i + nums[i]);
        if (i == curEnd) { jumps++; curEnd = farthest; }
    }
    return jumps;
}

// LC122 买卖股票最佳时机II
int maxProfit(vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < prices.size(); i++)
        if (prices[i] > prices[i-1]) profit += prices[i] - prices[i-1];
    return profit;
}

// LC134 加油站
int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
    int total = 0, tank = 0, start = 0;
    for (int i = 0; i < gas.size(); i++) {
        int diff = gas[i] - cost[i];
        tank += diff; total += diff;
        if (tank < 0) { start = i + 1; tank = 0; }
    }
    return total >= 0 ? start : -1;
}

// LC452 用最少的箭射气球
int findMinArrowShots(vector<vector<int>>& points) {
    if (points.empty()) return 0;
    sort(points.begin(), points.end(), [](auto& a, auto& b){ return a[1] < b[1]; });
    int arrows = 1, end = points[0][1];
    for (int i = 1; i < points.size(); i++)
        if (points[i][0] > end) { arrows++; end = points[i][1]; }
    return arrows;
}

// LC435 无重叠区间
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    if (intervals.empty()) return 0;
    sort(intervals.begin(), intervals.end(), [](auto& a, auto& b){ return a[1] < b[1]; });
    int count = 1, end = intervals[0][1];
    for (int i = 1; i < intervals.size(); i++)
        if (intervals[i][0] >= end) { count++; end = intervals[i][1]; }
    return intervals.size() - count;
}

// LC605 种花问题
bool canPlaceFlowers(vector<int>& flowerbed, int n) {
    int count = 0;
    for (int i = 0; i < flowerbed.size(); i++) {
        if (flowerbed[i] == 0) {
            int prev = (i == 0) ? 0 : flowerbed[i-1];
            int next = (i == flowerbed.size()-1) ? 0 : flowerbed[i+1];
            if (prev == 0 && next == 0) { flowerbed[i] = 1; count++; }
        }
    }
    return count >= n;
}

// LC392 判断子序列
bool isSubsequence(string s, string t) {
    int i = 0, j = 0;
    while (i < s.size() && j < t.size()) {
        if (s[i] == t[j]) i++;
        j++;
    }
    return i == s.size();
}

int main() {
    cout << "=== 贪心算法验证 ===" << endl;
    vector<int> nums1 = {2,3,1,1,4};
    cout << "LC55: " << canJump(nums1) << " (1)" << endl;
    
    vector<int> nums2 = {2,3,1,1,4};
    cout << "LC45: " << jump(nums2) << " (2)" << endl;
    
    vector<int> prices = {7,1,5,3,6,4};
    cout << "LC122: " << maxProfit(prices) << " (7)" << endl;
    
    vector<int> gas = {1,2,3,4,5}, cost = {3,4,5,1,2};
    cout << "LC134: " << canCompleteCircuit(gas, cost) << " (3)" << endl;
    
    vector<vector<int>> points = {{10,16},{2,8},{1,6},{7,12}};
    cout << "LC452: " << findMinArrowShots(points) << " (2)" << endl;
    
    vector<vector<int>> intervals = {{1,2},{2,3},{3,4},{1,3}};
    cout << "LC435: " << eraseOverlapIntervals(intervals) << " (1)" << endl;
    
    vector<int> flowerbed = {1,0,0,0,1};
    cout << "LC605: " << canPlaceFlowers(flowerbed, 1) << " (1)" << endl;
    
    cout << "LC392: " << isSubsequence("abc", "ahbgdc") << " (1)" << endl;
    return 0;
}
