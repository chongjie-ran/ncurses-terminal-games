// LC295 Find Median from Data Stream
// Hard - Two Heaps
// 核心: maxHeap存较小的一半, minHeap存较大的一半
// 中位数 = (maxHeap.top() + minHeap.top()) / 2 或 maxHeap.top()

#include <iostream>
#include <queue>
using namespace std;

class MedianFinder {
public:
    priority_queue<int> maxHeap;  // 存较小的一半, 顶部是最大值
    priority_queue<int, vector<int>, greater<int>> minHeap; // 存较大的一半, 顶部是最小值

    MedianFinder() {}

    void addNum(int num) {
        // 1. 先加入 maxHeap
        maxHeap.push(num);

        // 2. 平衡: maxHeap的top不能大于minHeap的top
        if (!minHeap.empty() && maxHeap.top() > minHeap.top()) {
            int val = minHeap.top(); minHeap.pop();
            minHeap.push(maxHeap.top()); maxHeap.pop();
            maxHeap.push(val);
        }

        // 3. 保证 size 差不超过1: maxHeap.size() >= minHeap.size()
        if (maxHeap.size() > minHeap.size() + 1) {
            minHeap.push(maxHeap.top());
            maxHeap.pop();
        } else if (maxHeap.size() < minHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    double findMedian() {
        if (maxHeap.size() > minHeap.size()) {
            return maxHeap.top();
        } else {
            return (maxHeap.top() + minHeap.top()) / 2.0;
        }
    }
};

int main() {
    MedianFinder* mf = new MedianFinder();

    // Test 1
    mf->addNum(1);
    mf->addNum(2);
    cout << "add 1, add 2: median = " << mf->findMedian() << " (expected 1.5)" << endl;

    mf->addNum(3);
    cout << "add 3: median = " << mf->findMedian() << " (expected 2)" << endl;

    // Test 2
    MedianFinder* mf2 = new MedianFinder();
    mf2->addNum(2);
    cout << "add 2: median = " << mf2->findMedian() << " (expected 2)" << endl;
    mf2->addNum(3);
    cout << "add 3: median = " << mf2->findMedian() << " (expected 2.5)" << endl;
    mf2->addNum(4);
    cout << "add 4: median = " << mf2->findMedian() << " (expected 3)" << endl;
    mf2->addNum(4);
    cout << "add 4: median = " << mf2->findMedian() << " (expected 3.5)" << endl;
    mf2->addNum(4);
    cout << "add 4: median = " << mf2->findMedian() << " (expected 4)" << endl;

    return 0;
}
