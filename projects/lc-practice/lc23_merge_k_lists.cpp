// LC23: Merge K Sorted Lists
// 思路: 最小堆 (priority_queue) 逐个弹出 O(N log K)
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// 最小堆比较器: 小值优先
struct NodeCmp {
    bool operator()(ListNode* a, ListNode* b) const {
        return a->val > b->val;
    }
};

ListNode* mergeKLists(vector<ListNode*>& lists) {
    priority_queue<ListNode*, vector<ListNode*>, NodeCmp> pq;
    
    for (ListNode* node : lists) {
        if (node) pq.push(node);
    }
    
    ListNode dummy(0);
    ListNode* cur = &dummy;
    
    while (!pq.empty()) {
        ListNode* top = pq.top(); pq.pop();
        cur->next = top;
        cur = cur->next;
        if (top->next) pq.push(top->next);
    }
    
    return dummy.next;
}

// 辅助: 数组转链表
ListNode* arrToList(vector<int> arr) {
    ListNode dummy(0);
    ListNode* cur = &dummy;
    for (int v : arr) {
        cur->next = new ListNode(v);
        cur = cur->next;
    }
    return dummy.next;
}

// 辅助: 打印链表
void printList(ListNode* head) {
    while (head) {
        cout << head->val;
        if (head->next) cout << " -> ";
        head = head->next;
    }
    cout << endl;
}

int main() {
    // 测试: [[1,4,5],[1,3,4],[2,6]]
    vector<vector<int>> inputs = {{1,4,5},{1,3,4},{2,6}};
    vector<ListNode*> lists;
    for (auto& arr : inputs) lists.push_back(arrToList(arr));
    
    ListNode* result = mergeKLists(lists);
    printList(result); // 预期: 1 -> 1 -> 2 -> 3 -> 4 -> 4 -> 5 -> 6
    
    // 空列表
    vector<ListNode*> emptyLists;
    cout << (mergeKLists(emptyLists) == nullptr ? "[]" : "FAIL") << endl;
    
    // 单链表
    vector<ListNode*> single = {arrToList({1,2,3})};
    printList(mergeKLists(single));
    
    return 0;
}
