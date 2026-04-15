// LC208 实现Trie（前缀树）
#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

class Trie {
private:
    struct Node {
        unordered_map<char, Node*> children;
        bool isEnd = false;
    };
    Node* root;
public:
    Trie() { root = new Node(); }
    
    void insert(string word) {
        Node* node = root;
        for (char c : word) {
            if (!node->children.count(c)) {
                node->children[c] = new Node();
            }
            node = node->children[c];
        }
        node->isEnd = true;
    }
    
    bool search(string word) {
        Node* node = root;
        for (char c : word) {
            if (!node->children.count(c)) return false;
            node = node->children[c];
        }
        return node->isEnd;
    }
    
    bool startsWith(string prefix) {
        Node* node = root;
        for (char c : prefix) {
            if (!node->children.count(c)) return false;
            node = node->children[c];
        }
        return true;
    }
};

int main() {
    Trie trie;
    trie.insert("apple");
    cout << trie.search("apple") << endl;    // 1
    cout << trie.search("app") << endl;      // 0
    cout << trie.startsWith("app") << endl;  // 1
    trie.insert("app");
    cout << trie.search("app") << endl;      // 1
    return 0;
}
