// 悟通LC练习 - 线段树(Segment Tree)模板 + 典型题目
// 2026-04-19 06:10 CST
// macOS compatible: no bits/stdc++.h

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <climits>
using namespace std;

// ============ 线段树模板 ============
class SegTree {
public:
    int n;
    vector<int> tree;
    
    SegTree(vector<int>& arr) {
        n = arr.size();
        tree.resize(4 * n);
        build(1, 0, n - 1, arr);
    }
    
    void build(int node, int l, int r, vector<int>& arr) {
        if (l == r) { tree[node] = arr[l]; return; }
        int mid = (l + r) / 2;
        build(node * 2, l, mid, arr);
        build(node * 2 + 1, mid + 1, r, arr);
        tree[node] = tree[node * 2] + tree[node * 2 + 1];
    }
    
    void update(int node, int l, int r, int idx, int val) {
        if (l == r) { tree[node] = val; return; }
        int mid = (l + r) / 2;
        if (idx <= mid) update(node * 2, l, mid, idx, val);
        else update(node * 2 + 1, mid + 1, r, idx, val);
        tree[node] = tree[node * 2] + tree[node * 2 + 1];
    }
    
    int query(int node, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return tree[node];
        int mid = (l + r) / 2;
        return query(node * 2, l, mid, ql, qr) + query(node * 2 + 1, mid + 1, r, ql, qr);
    }
    
    void pointUpdate(int idx, int val) { update(1, 0, n - 1, idx, val); }
    int rangeQuery(int l, int r) { return query(1, 0, n - 1, l, r); }
};

// ============ Lazy Propagation SegTree (区间更新) ============
class LazySegTree {
public:
    int n;
    vector<int> tree, lazy;
    
    LazySegTree(vector<int>& arr) {
        n = arr.size();
        tree.resize(4 * n);
        lazy.resize(4 * n, 0);
        build(1, 0, n - 1, arr);
    }
    
    void build(int node, int l, int r, vector<int>& arr) {
        if (l == r) { tree[node] = arr[l]; return; }
        int mid = (l + r) / 2;
        build(node * 2, l, mid, arr);
        build(node * 2 + 1, mid + 1, r, arr);
        tree[node] = tree[node * 2] + tree[node * 2 + 1];
    }
    
    void push(int node) {
        if (lazy[node] != 0) {
            tree[node * 2] += lazy[node];
            tree[node * 2 + 1] += lazy[node];
            lazy[node * 2] += lazy[node];
            lazy[node * 2 + 1] += lazy[node];
            lazy[node] = 0;
        }
    }
    
    void updateRange(int node, int l, int r, int ql, int qr, int val) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) {
            tree[node] += val * (r - l + 1);
            lazy[node] += val;
            return;
        }
        push(node);
        int mid = (l + r) / 2;
        updateRange(node * 2, l, mid, ql, qr, val);
        updateRange(node * 2 + 1, mid + 1, r, ql, qr, val);
        tree[node] = tree[node * 2] + tree[node * 2 + 1];
    }
    
    int queryRange(int node, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return tree[node];
        push(node);
        int mid = (l + r) / 2;
        return queryRange(node * 2, l, mid, ql, qr) + queryRange(node * 2 + 1, mid + 1, r, ql, qr);
    }
    
    void rangeUpdate(int l, int r, int val) { updateRange(1, 0, n - 1, l, r, val); }
    int rangeQuery(int l, int r) { return queryRange(1, 0, n - 1, l, r); }
};

// ============ LC307 区域和检索 - 可变 ============
class NumArray {
public:
    SegTree* st;
    NumArray(vector<int>& nums) {
        if (nums.empty()) { st = nullptr; return; }
        st = new SegTree(nums);
    }
    void update(int index, int val) { if (st) st->pointUpdate(index, val); }
    int sumRange(int left, int right) { return st ? st->rangeQuery(left, right) : 0; }
};

// ============ LC732 我的日程安排III ============
class MyCalendarThree {
public:
    map<int, int> diff;
    int book(int start, int end) {
        diff[start]++;
        diff[end]--;
        int cur = 0, ans = 0;
        for (auto& kv : diff) {
            cur += kv.second;
            ans = max(ans, cur);
        }
        return ans;
    }
};

// ============ LC2251 流星雨中的人数 ============
int numberOfPoints(vector<vector<int>>& nums) {
    map<int, int> diff;
    for (auto& p : nums) {
        diff[p[0]]++;
        diff[p[1]]--; // 差分，end点不包含
    }
    int ans = 0, cur = 0, prev = INT_MIN;
    for (auto& kv : diff) {
        if (cur > 0) ans += kv.first - prev;
        cur += kv.second;
        prev = kv.first;
    }
    return ans;
}

// ============ 测试 ============
int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    SegTree st(arr);
    cout << "SegTree Sum[0,2] = " << st.rangeQuery(0, 2) << " (expect 9)" << endl;
    cout << "SegTree Sum[1,4] = " << st.rangeQuery(1, 4) << " (expect 24)" << endl;
    st.pointUpdate(2, 6);
    cout << "SegTree After update[0,2] = " << st.rangeQuery(0, 2) << " (expect 10)" << endl;
    
    LazySegTree lst(arr);
    lst.rangeUpdate(0, 2, 2);
    cout << "LazySeg Sum[0,2] = " << lst.rangeQuery(0, 2) << " (expect 15: 1+3+5+2*3)" << endl;
    
    NumArray na(arr);
    cout << "NumArray Sum[0,2] = " << na.sumRange(0, 2) << " (expect 9)" << endl;
    na.update(2, 6);
    cout << "NumArray After update[0,2] = " << na.sumRange(0, 2) << " (expect 10)" << endl;
    
    MyCalendarThree cal;
    cout << "\nMyCalendarThree:" << endl;
    cout << "book(10,20) = " << cal.book(10, 20) << " (expect 1)" << endl;
    cout << "book(50,60) = " << cal.book(50, 60) << " (expect 1)" << endl;
    cout << "book(10,40) = " << cal.book(10, 40) << " (expect 2)" << endl;
    cout << "book(5,15) = " << cal.book(5, 15) << " (expect 3)" << endl;
    cout << "book(5,10) = " << cal.book(5, 10) << " (expect 3)" << endl;
    cout << "book(25,55) = " << cal.book(25, 55) << " (expect 3)" << endl;
    
    cout << "\nnumberOfPoints:" << endl;
    vector<vector<int>> nums = {{1,3},{3,7}};
    cout << "1-3, 3-7 => " << numberOfPoints(nums) << " (expect 5)" << endl;
    nums = {{1,3},{1,5},{2,6},{3,4}};
    cout << "1-3,1-5,2-6,3-4 => " << numberOfPoints(nums) << " (expect 6)" << endl;
    
    return 0;
}
