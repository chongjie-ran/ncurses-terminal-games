// LC208 Implement Trie (Prefix Tree)
// Medium - Trie
#include <string>
#include <vector>
using namespace std;

class TrieNode {
public:
    TrieNode* children[26];
    bool is_end;
    TrieNode() {
        for (int i = 0; i < 26; i++) children[i] = nullptr;
        is_end = false;
    }
};

class Trie {
    TrieNode* root;
public:
    Trie() { root = new TrieNode(); }
    
    void insert(string word) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) node->children[idx] = new TrieNode();
            node = node->children[idx];
        }
        node->is_end = true;
    }
    
    bool search(string word) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) return false;
            node = node->children[idx];
        }
        return node->is_end;
    }
    
    bool startsWith(string prefix) {
        TrieNode* node = root;
        for (char c : prefix) {
            int idx = c - 'a';
            if (!node->children[idx]) return false;
            node = node->children[idx];
        }
        return true;
    }
};

/**
 * 核心洞察:
 * - Trie是前缀树,每个节点有26个子指针(字母表)
 * - insert: 沿路径走,没有就创建,最后标记end
 * - search: 沿路径走,不存在返回false,存在但不是end也返回false
 * - startsWith: 同search但不检查end
 * 
 * 时间复杂度: O(n) per operation, n=word/prefix长度
 * 空间复杂度: O(ALPHABET_SIZE * n) 最坏情况
 */
