/**
 * LC146 LRU Cache
 * 专题: 设计 - HashMap + 双向链表
 * 
 * 核心: 
 * - get(key): O(1) 获取值并移到头部
 * - put(key,val): O(1) 插入/更新并移到头部
 * - 容量满时删除尾部（最久未使用）
 * 
 * 数据结构:
 * - unordered_map<int, ListNode*> 快速定位
 * - 双向链表维护顺序（头部最新，尾部最旧）
 * - 虚拟头尾节点简化边界
 */

#include <iostream>
#include <unordered_map>
#include <list>
using namespace std;

class LRUCache {
private:
    struct ListNode {
        int key, val;
        ListNode* prev;
        ListNode* next;
        ListNode(int k=0, int v=0): key(k), val(v), prev(nullptr), next(nullptr) {}
    };
    
    int capacity;
    unordered_map<int, ListNode*> mp;  // key -> node ptr
    ListNode* head;  // 虚拟头（最新）
    ListNode* tail;  // 虚拟尾（最旧）
    
    // 把节点移到头部（最新使用）
    void moveToHead(ListNode* node) {
        if (node == head) return;
        // 先断开
        node->prev->next = node->next;
        if (node->next) node->next->prev = node->prev;
        else tail = node->prev;  // node是tail
        // 插入头部
        node->next = head->next;
        if (node->next) node->next->prev = node;
        head->next = node;
        node->prev = head;
    }
    
    // 删除尾部节点（最久未使用）
    void removeTail() {
        if (tail == head) return;
        ListNode* old = tail;
        tail = tail->prev;
        tail->next = nullptr;
        mp.erase(old->key);
        delete old;
    }

public:
    LRUCache(int cap): capacity(cap) {
        head = new ListNode();  // 虚拟头
        tail = new ListNode();  // 虚拟尾
        head->next = tail;
        tail->prev = head;
    }
    
    int get(int key) {
        auto it = mp.find(key);
        if (it == mp.end()) return -1;
        ListNode* node = it->second;
        moveToHead(node);
        return node->val;
    }
    
    void put(int key, int value) {
        ListNode* node;
        auto it = mp.find(key);
        if (it != mp.end()) {
            node = it->second;
            node->val = value;
            moveToHead(node);
        } else {
            node = new ListNode(key, value);
            mp[key] = node;
            node->next = head->next;
            if (node->next) node->next->prev = node;
            head->next = node;
            node->prev = head;
            if (tail->prev == head) tail->prev = node;  // 第一个节点
            if (node->next == nullptr && tail->prev == head) {
                // 修复tail
                tail->prev = node;
            }
            if (mp.size() > capacity) {
                // 删除最老的（tail前一个）
                ListNode* old = tail->prev;
                if (old != head) {
                    old->prev->next = tail;
                    tail->prev = old->prev;
                    mp.erase(old->key);
                    delete old;
                }
            }
        }
    }
    
    ~LRUCache() {
        ListNode* cur = head;
        while (cur) {
            ListNode* next = cur->next;
            delete cur;
            cur = next;
        }
    }
};

// 简洁版本：直接用list + map
class LRUCache2 {
private:
    int cap;
    list<pair<int,int>> lst;  // (key, val)，头部最新
    unordered_map<int, list<pair<int,int>>::iterator> mp;
    
public:
    LRUCache2(int capacity): cap(capacity) {}
    
    int get(int key) {
        auto it = mp.find(key);
        if (it == mp.end()) return -1;
        lst.splice(lst.begin(), lst, it->second);  // 移到头部
        return it->second->second;
    }
    
    void put(int key, int value) {
        if (mp.count(key)) {
            mp[key]->second = value;
            lst.splice(lst.begin(), lst, mp[key]);
        } else {
            lst.emplace_front(key, value);
            mp[key] = lst.begin();
            if (mp.size() > cap) {
                auto last = lst.back();
                mp.erase(last.first);
                lst.pop_back();
            }
        }
    }
};

int main() {
    // 官方示例验证
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    int r1 = cache.get(1); cout << r1 << " ";  // 1
    cache.put(3, 3);                            // 淘汰key=2
    int r2 = cache.get(2); cout << r2 << " ";  // -1
    cache.put(4, 4);                            // 淘汰key=1
    int r3 = cache.get(1); cout << r3 << " ";  // -1
    int r4 = cache.get(3); cout << r4 << " ";  // 3
    int r5 = cache.get(4); cout << r5 << endl; // 4
    // 预期: 1 -1 -1 3 4
    cout << "验证: " << (r1==1 && r2==-1 && r3==-1 && r4==3 && r5==4 ? "✅" : "❌") << endl;
    
    // 测试容量1
    LRUCache cache2(1);
    cache2.put(1, 10);
    cache2.put(2, 20);
    int t1 = cache2.get(1); cout << t1 << " ";  // -1 (被淘汰)
    int t2 = cache2.get(2); cout << t2 << endl;  // 20
    cout << "验证: " << (t1==-1 && t2==20 ? "✅" : "❌") << endl;
    
    // 测试更新
    LRUCache cache3(2);
    cache3.put(1, 1);
    cache3.put(2, 2);
    cache3.put(1, 10);  // 更新key=1
    int u1 = cache3.get(1); cout << u1 << " ";  // 10
    cache3.put(3, 3);  // 淘汰key=2（1刚被访问过）
    int u2 = cache3.get(2); cout << u2 << " ";  // -1
    int u3 = cache3.get(3); cout << u3 << endl;  // 3
    cout << "验证: " << (u1==10 && u2==-1 && u3==3 ? "✅" : "❌") << endl;
    
    return 0;
}
