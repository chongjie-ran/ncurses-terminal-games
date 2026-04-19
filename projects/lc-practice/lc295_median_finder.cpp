#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>
#include <functional>
using namespace std;

// LC295 数据流中位数
// Hard - 堆的应用

class MedianFinder {
private:
    // maxHeap: 左边较小的一半 (用大根堆)
    // minHeap: 右边较大的一半 (用小根堆)
    priority_queue<int> maxHeap;
    priority_queue<int, vector<int>, greater<int>> minHeap;
    
public:
    MedianFinder() {}
    
    void addNum(int num) {
        // 先加入maxHeap，再平衡
        maxHeap.push(num);
        
        // 确保maxHeap的top <= minHeap的top
        if (!minHeap.empty() && maxHeap.top() > minHeap.top()) {
            int val = maxHeap.top(); maxHeap.pop();
            minHeap.push(val);
        }
        
        // 平衡: maxHeap.size() == minHeap.size() || maxHeap.size() == minHeap.size() + 1
        if (maxHeap.size() > minHeap.size() + 1) {
            int val = maxHeap.top(); maxHeap.pop();
            minHeap.push(val);
        } else if (maxHeap.size() < minHeap.size()) {
            int val = minHeap.top(); minHeap.pop();
            maxHeap.push(val);
        }
    }
    
    double findMedian() {
        if (maxHeap.size() > minHeap.size()) {
            return (double)maxHeap.top();
        } else if (maxHeap.size() < minHeap.size()) {
            return (double)minHeap.top();
        } else {
            return (double)(maxHeap.top() + minHeap.top()) / 2.0;
        }
    }
};

// 测试
int main() {
    MedianFinder mf;
    vector<pair<string, vector<int>>> tests = {
        {"Example1", {2, 3, 4}},
        {"Example2", {1, 2, 3}},
        {"Odd", {5, 4, 3, 2, 1}},
        {"Even", {6, 5, 4, 3, 2, 1}},
    };
    
    for (auto& [name, nums] : tests) {
        MedianFinder mf2;
        cout << name << ": ";
        for (int n : nums) {
            mf2.addNum(n);
            cout << n << " ";
        }
        double median = mf2.findMedian();
        cout << "-> Median: " << fixed << setprecision(1) << median << endl;
    }
    
    return 0;
}
