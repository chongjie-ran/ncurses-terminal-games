// LC295 Find Median from Data Stream
// Two heaps: max-heap for lower half, min-heap for upper half
// Invariant: maxHeap.size() == minHeap.size() OR maxHeap.size() == minHeap.size() + 1
// 2026-03-30 23:50

#include <queue>
#include <cassert>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>
using namespace std;

class MedianFinder {
public:
    MedianFinder() {}

    void addNum(int num) {
        // Lower half: max-heap (store directly, no negation needed)
        if (maxHeap.empty() || num <= maxHeap.top()) {
            maxHeap.push(num);
        } else {
            minHeap.push(num);
        }

        // Balance: maxHeap.size() == minHeap.size() OR maxHeap = minHeap + 1
        if (maxHeap.size() > minHeap.size() + 1) {
            // maxHeap too big → move largest of lower half to upper half
            minHeap.push(maxHeap.top());
            maxHeap.pop();
        } else if (maxHeap.size() < minHeap.size()) {
            // minHeap too big → move smallest of upper half to lower half
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    double findMedian() {
        if (maxHeap.size() > minHeap.size()) {
            return (double)maxHeap.top();
        } else {
            return (double)(maxHeap.top() + minHeap.top()) / 2.0;
        }
    }

private:
    priority_queue<int> maxHeap;                                       // lower half, max-heap
    priority_queue<int, vector<int>, greater<int>> minHeap;             // upper half, min-heap
};

int main() {
    MedianFinder mf;
    mf.addNum(1);
    mf.addNum(2);
    assert(fabs(mf.findMedian() - 1.5) < 1e-9);
    mf.addNum(3);
    assert(fabs(mf.findMedian() - 2.0) < 1e-9);
    cout << "LC295: Test 1 passed (1,2→1.5, 3→2.0)!" << endl;

    MedianFinder mf2;
    mf2.addNum(2);
    assert(fabs(mf2.findMedian() - 2.0) < 1e-9);
    mf2.addNum(1);
    assert(fabs(mf2.findMedian() - 1.5) < 1e-9);
    cout << "LC295: Test 2 passed (2,1→1.5)!" << endl;

    // Test even n: [1,2,3,4] median = (2+3)/2 = 2.5
    MedianFinder mf3;
    mf3.addNum(1);
    mf3.addNum(2);
    mf3.addNum(3);
    mf3.addNum(4);
    assert(fabs(mf3.findMedian() - 2.5) < 1e-9);
    cout << "LC295: Test 3 passed (1,2,3,4→2.5)!" << endl;

    // Test negative: [-1,-2,-3] median = -2
    MedianFinder mf4;
    mf4.addNum(-1);
    mf4.addNum(-2);
    mf4.addNum(-3);
    assert(fabs(mf4.findMedian() - (-2.0)) < 1e-9);
    cout << "LC295: Test 4 passed (-1,-2,-3→-2.0)!" << endl;

    cout << "✅ All LC295 tests passed!" << endl;
    return 0;
}
