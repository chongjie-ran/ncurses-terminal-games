// LC347: Top K Frequent Elements
// 桶排序：统计频率后，按频率分组
// 时间O(n)，空间O(n)
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

class Solution {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        unordered_map<int,int> freq;
        for(int x: nums) freq[x]++;
        int n = nums.size();
        vector<vector<int>> bucket(n+1);
        for(auto& p: freq) bucket[p.second].push_back(p.first);
        vector<int> result;
        for(int i=n; i>=0 && result.size()<(size_t)k; i--) {
            for(int x: bucket[i]) {
                result.push_back(x);
                if(result.size()==(size_t)k) break;
            }
        }
        return result;
    }
};
