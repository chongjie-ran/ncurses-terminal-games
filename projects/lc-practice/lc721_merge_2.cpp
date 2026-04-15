// LC721 Merge Intervals II - 合并区间II
// Hard | 排序 + 区间合并
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

// 方法1: 先排序再合并 (与LC56相同思路)
vector<vector<int>> merge1(vector<vector<int>>& intervals) {
    if (intervals.empty()) return {};
    sort(intervals.begin(), intervals.end());
    vector<vector<int>> result = {intervals[0]};
    for (int i = 1; i < intervals.size(); i++) {
        if (intervals[i][0] <= result.back()[1]) {
            result.back()[1] = max(result.back()[1], intervals[i][1]);
        } else {
            result.push_back(intervals[i]);
        }
    }
    return result;
}

// 方法2: 用map计数 (统计每个点被覆盖的次数) - 需要C++17
vector<vector<int>> merge2(vector<vector<int>>& intervals) {
    if (intervals.empty()) return {};
    
    // 使用map自动排序
    map<int, int> diff;
    for (auto& inv : intervals) {
        diff[inv[0]]++;      // 起点+1
        diff[inv[1]--]--;    // 终点-1 (注意这里是--因为区间是闭区间)
    }
    
    vector<vector<int>> result;
    int count = 0, start = -1;
    
    for (auto& p : diff) {
        int pos = p.first;
        int delta = p.second;
        if (count == 0) start = pos;  // 开始新区间
        count += delta;
        if (count == 0) {  // 区间结束
            result.push_back({start, pos});
        }
    }
    return result;
}

int main() {
    // 测试1
    vector<vector<int>> i1 = {{1,3},{2,6},{8,10},{15,18}};
    auto r1 = merge1(i1);
    cout << "Method1: ";
    for (auto& inv : r1) cout << "[" << inv[0] << "," << inv[1] << "] ";
    cout << endl;
    // 期望: [1,6] [8,10] [15,18]
    
    // 测试2: 有重叠的区间
    vector<vector<int>> i2 = {{1,4},{2,3}};
    auto r2 = merge1(i2);
    cout << "Method1: ";
    for (auto& inv : r2) cout << "[" << inv[0] << "," << inv[1] << "] ";
    cout << endl;
    // 期望: [1,4]
    
    // 测试3: 空输入
    vector<vector<int>> i3;
    auto r3 = merge2(i3);
    cout << "Method2 empty: " << r3.size() << endl;
    
    // 测试4: 方法2
    auto r4 = merge2(i1);
    cout << "Method2: ";
    for (auto& inv : r4) cout << "[" << inv[0] << "," << inv[1] << "] ";
    cout << endl;
    
    return 0;
}
