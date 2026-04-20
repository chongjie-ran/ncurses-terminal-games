// LC208: Implement Trie (Prefix Tree)
// Pattern: Trie Node + insert/search/startsWith in O(m) time where m = key length

#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

class Trie {
    struct Node {
        bool is_end = false;
        unordered_map<char, Node*> children;
    };
    Node* root;

public:
    Trie() { root = new Node(); }

    void insert(string word) {
        Node* cur = root;
        for (char c : word) {
            if (!cur->children.count(c)) {
                cur->children[c] = new Node();
            }
            cur = cur->children[c];
        }
        cur->is_end = true;
    }

    bool search(string word) {
        Node* cur = root;
        for (char c : word) {
            if (!cur->children.count(c)) return false;
            cur = cur->children[c];
        }
        return cur->is_end;
    }

    bool startsWith(string prefix) {
        Node* cur = root;
        for (char c : prefix) {
            if (!cur->children.count(c)) return false;
            cur = cur->children[c];
        }
        return true;
    }
};

int main() {
    Trie trie;
    trie.insert("apple");
    
    cout << (trie.search("apple") == true) << " (expect 1)" << endl;     // true
    cout << (trie.search("app") == false) << " (expect 1)" << endl;     // false
    cout << (trie.startsWith("app") == true) << " (expect 1)" << endl;  // true
    trie.insert("app");
    cout << (trie.search("app") == true) << " (expect 1)" << endl;     // true
    
    Trie t2;
    t2.insert("a");
    cout << (t2.search("a") == true) << " (expect 1)" << endl;          // true
    cout << (t2.startsWith("b") == false) << " (expect 1)" << endl;   // false
    cout << (t2.search("ap") == false) << " (expect 1)" << endl;      // false
    
    return 0;
}
