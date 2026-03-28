// LC208: Implement Trie (Prefix Tree)
// 字典树/前缀树
// insert O(m), search O(m), startsWith O(m)  m=word.length

#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

class Trie {
private:
    struct Node {
        bool isWord = false;
        unordered_map<char, Node*> children;
    };
    Node* root;

public:
    Trie() {
        root = new Node();
    }
    
    void insert(const string& word) {
        Node* cur = root;
        for (char c : word) {
            if (!cur->children.count(c))
                cur->children[c] = new Node();
            cur = cur->children[c];
        }
        cur->isWord = true;
    }
    
    bool search(const string& word) {
        Node* cur = root;
        for (char c : word) {
            if (!cur->children.count(c)) return false;
            cur = cur->children[c];
        }
        return cur->isWord;
    }
    
    bool startsWith(const string& prefix) {
        Node* cur = root;
        for (char c : prefix) {
            if (!cur->children.count(c)) return false;
            cur = cur->children[c];
        }
        return true;
    }
};

// 测试
int main() {
    Trie t;
    t.insert("apple");
    cout << (t.search("apple") == true ? "PASS" : "FAIL") << " search(apple)=true\n";      // true
    cout << (t.search("app") == false ? "PASS" : "FAIL") << " search(app)=false\n";         // false
    cout << (t.startsWith("app") == true ? "PASS" : "FAIL") << " startsWith(app)=true\n";   // true
    t.insert("app");
    cout << (t.search("app") == true ? "PASS" : "FAIL") << " search(app)=true\n";           // true
    return 0;
}
