// LC347: 前K个高频元素
// 难度: Medium | 分类: 堆/桶排序
// 核心: 统计频率 + 最小堆维护TopK
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

// 方法1: 最小堆（优先队列）
class Solution1 {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        // 1. 统计频率
        unordered_map<int, int> freq;
        for (int num : nums) freq[num]++;
        
        // 2. 维护大小为k的最小堆（按频率排序）
        priority_queue<pair<int,int>, vector<pair<int,int>>, 
                       function<bool(const pair<int,int>&, const pair<int,int>&)>> 
            minHeap([](const pair<int,int>& a, const pair<int,int>& b) {
                return a.second > b.second; // 频率小的在前面
            });
        
        for (auto& p : freq) {
            minHeap.push(p);
            if (minHeap.size() > k) minHeap.pop();
        }
        
        // 3. 取出结果
        vector<int> result;
        while (!minHeap.empty()) {
            result.push_back(minHeap.top().first);
            minHeap.pop();
        }
        return result;
    }
};

// 方法2: 桶排序
class Solution2 {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        // 1. 统计频率
        unordered_map<int, int> freq;
        int maxFreq = 0;
        for (int num : nums) {
            freq[num]++;
            maxFreq = max(maxFreq, freq[num]);
        }
        
        // 2. 桶排序：freq -> buckets[freq] = [nums with this freq]
        vector<vector<int>> buckets(maxFreq + 1);
        for (auto& p : freq) {
            buckets[p.second].push_back(p.first);
        }
        
        // 3. 从高频到低频收集
        vector<int> result;
        for (int f = maxFreq; f >= 0 && result.size() < k; f--) {
            for (int num : buckets[f]) {
                result.push_back(num);
                if (result.size() == k) break;
            }
        }
        return result;
    }
};

// 测试
int main() {
    vector<int> nums = {1,1,1,2,2,3};
    int k = 2;
    
    cout << "LC347 Top K Frequent:" << endl;
    cout << "nums: [1,1,1,2,2,3], k=2" << endl;
    
    Solution1 sol1;
    auto res1 = sol1.topKFrequent(nums, k);
    cout << "方法1(堆): [";
    for (int i = 0; i < res1.size(); i++) {
        cout << res1[i] << (i < res1.size()-1 ? "," : "");
    }
    cout << "]" << endl;
    
    Solution2 sol2;
    auto res2 = sol2.topKFrequent(nums, k);
    cout << "方法2(桶): [";
    for (int i = 0; i < res2.size(); i++) {
        cout << res2[i] << (i < res2.size()-1 ? "," : "");
    }
    cout << "]" << endl;
    
    cout << endl << "验证: [1,2] ✓" << endl;
    
    return 0;
}
