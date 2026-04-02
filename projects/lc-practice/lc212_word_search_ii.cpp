// LC212: Word Search II
// 给定 m×n board 和字符串列表 words, 返回board上能形成的所有单词
// 思路: Trie + DFS backtracking
// 构建Trie → 从每个格子DFS → Trie剪枝 → 找到单词则记录

#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct TrieNode {
    bool isWord = false;
    string word = "";          // 只有当isWord=true时才记录完整单词
    TrieNode* children[26] = {nullptr};
};

class Trie {
public:
    TrieNode* root;
    Trie() { root = new TrieNode(); }

    void insert(const string& w) {
        TrieNode* cur = root;
        for (char c : w) {
            int idx = c - 'a';
            if (!cur->children[idx])
                cur->children[idx] = new TrieNode();
            cur = cur->children[idx];
        }
        cur->isWord = true;
        cur->word = w;
    }
};

class Solution {
public:
    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        int m = board.size(), n = board[0].size();
        Trie trie;
        for (auto& w : words) trie.insert(w);

        vector<string> result;
        vector<vector<bool>> visited(m, vector<bool>(n, false));

        // 4个方向
        int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

        function<void(int,int,TrieNode*)> dfs = [&](int i, int j, TrieNode* node) {
            char c = board[i][j];
            int idx = c - 'a';
            if (!node->children[idx]) return; // Trie中不存在此前缀,剪枝

            TrieNode* next = node->children[idx];
            if (next->isWord) {
                result.push_back(next->word);
                next->isWord = false; // 避免重复添加(单词可能出现在多个位置)
            }

            visited[i][j] = true;
            for (auto& d : dirs) {
                int ni = i + d[0], nj = j + d[1];
                if (ni >= 0 && ni < m && nj >= 0 && nj < n && !visited[ni][nj]) {
                    dfs(ni, nj, next);
                }
            }
            visited[i][j] = false; // backtrack
        };

        // 从每个格子开始DFS
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                dfs(i, j, trie.root);
            }
        }

        return result;
    }
};

// 测试
int main() {
    // board = [
    //   ['o','a','a','n'],
    //   ['e','t','a','e'],
    //   ['i','h','k','r'],
    //   ['i','f','l','v']
    // ]
    // words = ["oath","pea","eat","rain"]
    vector<vector<char>> board = {
        {'o','a','a','n'},
        {'e','t','a','e'},
        {'i','h','k','r'},
        {'i','f','l','v'}
    };
    vector<string> words = {"oath","pea","eat","rain"};

    Solution sol;
    vector<string> result = sol.findWords(board, words);
    cout << "Result: ";
    for (auto& w : result) cout << w << " ";
    cout << endl;

    // 验证
    vector<string> expected = {"oath","eat"};
    cout << "Expected: oath eat" << endl;

    return 0;
}
