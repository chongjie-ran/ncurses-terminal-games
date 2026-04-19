// LC295 Find Median from Data Stream
// 难度: Hard
// 核心: 对顶堆 - 一个最大堆(左)+一个最小堆(右)
// 思路: 
//   - maxHeap存较小一半, minHeap存较大一半
//   - 维持maxHeap.size() >= minHeap.size()
//   - 中位数 = maxHeap.top() 或 (maxHeap.top()+minHeap.top())/2

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

class MedianFinder {
private:
    // maxHeap: 存较小一半，最大堆
    priority_queue<double> maxHeap;
    // minHeap: 存较大一半，最小堆
    priority_queue<double, vector<double>, greater<double>> minHeap;
    
    // 平衡两个堆的大小
    void rebalance() {
        // maxHeap大小 >= minHeap大小
        if (maxHeap.size() > minHeap.size() + 1) {
            minHeap.push(maxHeap.top());
            maxHeap.pop();
        } else if (maxHeap.size() < minHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }
    
public:
    MedianFinder() {}
    
    void addNum(int num) {
        double dnum = static_cast<double>(num);
        // 新数加入maxHeap(左半边)
        if (maxHeap.empty() || dnum <= maxHeap.top()) {
            maxHeap.push(dnum);
        } else {
            minHeap.push(dnum);
        }
        rebalance();
    }
    
    double findMedian() {
        if (maxHeap.size() > minHeap.size()) {
            return maxHeap.top();
        } else if (maxHeap.size() < minHeap.size()) {
            return minHeap.top();
        } else {
            return (maxHeap.top() + minHeap.top()) / 2.0;
        }
    }
};

int main() {
    MedianFinder mf;
    
    // 测试1: [1,2] -> 中位数1.5
    mf.addNum(1);
    mf.addNum(2);
    cout << "1,2 -> " << mf.findMedian() << " (期望1.5)" << endl;
    
    // 测试2: [1,2,3] -> 中位数2.0
    mf.addNum(3);
    cout << "1,2,3 -> " << mf.findMedian() << " (期望2.0)" << endl;
    
    // 测试3: [1,2,3,4] -> 中位数2.5
    mf.addNum(4);
    cout << "1,2,3,4 -> " << mf.findMedian() << " (期望2.5)" << endl;
    
    // 测试4: 单个数
    MedianFinder mf2;
    mf2.addNum(5);
    cout << "5 -> " << mf2.findMedian() << " (期望5.0)" << endl;
    
    // 测试5: [12,10,11,5] -> 依次加
    MedianFinder mf3;
    mf3.addNum(12);
    cout << "12 -> " << mf3.findMedian() << " (期望12.0)" << endl;
    mf3.addNum(10);
    cout << "12,10 -> " << mf3.findMedian() << " (期望11.0)" << endl;
    mf3.addNum(11);
    cout << "12,10,11 -> " << mf3.findMedian() << " (期望11.0)" << endl;
    mf3.addNum(5);
    cout << "12,10,11,5 -> " << mf3.findMedian() << " (期望10.5)" << endl;
    
    return 0;
}
