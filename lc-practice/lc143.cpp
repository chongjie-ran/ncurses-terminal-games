// LC143: Reorder List
// 快慢指针找中点 → 反转后半段 → 合并两链表
#include <cassert>
#include <iostream>
#include <vector>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// 1->2->3->4->5 => 1->5->2->4->3
void reorderList(ListNode* head) {
    if (!head || !head->next) return;
    // 1. 快慢指针找中点
    ListNode *slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    // 2. 反转后半段
    ListNode *prev = nullptr, *cur = slow->next;
    slow->next = nullptr;
    while (cur) {
        ListNode* nxt = cur->next;
        cur->next = prev;
        prev = cur;
        cur = nxt;
    }
    // 3. 合并：head前半段(正) + prev后半段(反)
    ListNode *p = head, *q = prev;
    while (q) {
        ListNode* pn = p->next;
        ListNode* qn = q->next;
        p->next = q;
        q->next = pn;
        p = pn;
        q = qn;
    }
}

// 辅助：向量转链表
ListNode* vec2list(const vector<int>& v) {
    ListNode dummy(0);
    ListNode* cur = &dummy;
    for (int x : v) { cur->next = new ListNode(x); cur = cur->next; }
    return dummy.next;
}
// 辅助：链表转向量
vector<int> list2vec(ListNode* head) {
    vector<int> r;
    while (head) { r.push_back(head->val); head = head->next; }
    return r;
}

int main() {
    // 1->2->3->4->5 => 1->5->2->4->3
    ListNode* h1 = vec2list({1,2,3,4,5});
    reorderList(h1);
    assert(list2vec(h1) == (vector<int>{1,5,2,4,3}));
    
    // 1->2->3->4 => 1->4->2->3
    ListNode* h2 = vec2list({1,2,3,4});
    reorderList(h2);
    assert(list2vec(h2) == (vector<int>{1,4,2,3}));
    
    cout << "LC143: 2/2 all passed ✅" << endl;
    return 0;
}
