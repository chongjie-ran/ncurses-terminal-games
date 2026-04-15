// LC212 单词搜索II - Trie + DFS
#include <iostream>
#include <vector>
#include <unordered_set>
using namespace std;

struct TrieNode {
    TrieNode* children[26] = {nullptr};
    string word;
};

class Solution {
public:
    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        TrieNode* root = new TrieNode();
        for (const string& w : words) {
            TrieNode* node = root;
            for (char c : w) {
                int idx = c - 'a';
                if (!node->children[idx]) node->children[idx] = new TrieNode();
                node = node->children[idx];
            }
            node->word = w;
        }
        
        unordered_set<string> result;
        int m = board.size(), n = board[0].size();
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                dfs(board, root, i, j, result);
            }
        }
        return vector<string>(result.begin(), result.end());
    }
private:
    void dfs(vector<vector<char>>& board, TrieNode* node, int i, int j, unordered_set<string>& result) {
        char c = board[i][j];
        int idx = c - 'a';
        if (c == '#' || !node->children[idx]) return;
        
        node = node->children[idx];
        if (!node->word.empty()) {
            result.insert(node->word);
            // 不return，继续找更长单词
        }
        
        board[i][j] = '#';
        int m = board.size(), n = board[0].size();
        if (i > 0) dfs(board, node, i-1, j, result);
        if (i+1 < m) dfs(board, node, i+1, j, result);
        if (j > 0) dfs(board, node, i, j-1, result);
        if (j+1 < n) dfs(board, node, i, j+1, result);
        board[i][j] = c;
    }
};

int main() {
    Solution sol;
    vector<vector<char>> board = {
        {'o','a','a','n'},
        {'e','t','a','e'},
        {'i','h','k','r'},
        {'i','f','l','v'}
    };
    vector<string> words = {"oath","pea","eat","rain"};
    vector<string> result = sol.findWords(board, words);
    for (const string& w : result) cout << w << " ";  // oath eat
    cout << endl;
    return 0;
}
