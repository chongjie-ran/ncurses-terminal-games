// LC 297. Serialize and Deserialize Binary Tree
// 方法：前序遍历 + 递归
// 序列化：前序遍历，NULL用#表示，节点用,分隔
// 反序列化：从序列重建二叉树

#include <iostream>
#include <sstream>
#include <queue>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Codec {
public:
    // 序列化：前序遍历
    string serialize(TreeNode* root) {
        stringstream ss;
        serializeHelper(root, ss);
        return ss.str();
    }
    
    void serializeHelper(TreeNode* node, stringstream& ss) {
        if (!node) {
            ss << "#,";
            return;
        }
        ss << node->val << ",";
        serializeHelper(node->left, ss);
        serializeHelper(node->right, ss);
    }
    
    // 反序列化：前序遍历重建
    TreeNode* deserialize(string data) {
        stringstream ss(data);
        queue<string> tokens;
        string token;
        while (getline(ss, token, ',')) {
            tokens.push(token);
        }
        return deserializeHelper(tokens);
    }
    
    TreeNode* deserializeHelper(queue<string>& tokens) {
        if (tokens.empty()) return NULL;
        string t = tokens.front();
        tokens.pop();
        if (t == "#") return NULL;
        TreeNode* node = new TreeNode(stoi(t));
        node->left = deserializeHelper(tokens);
        node->right = deserializeHelper(tokens);
        return node;
    }
};

// 测试
int main() {
    // 构建测试树
    //      1
    //     / \
    //    2   3
    //       / \
    //      4   5
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->right->left = new TreeNode(4);
    root->right->right = new TreeNode(5);
    
    Codec codec;
    string encoded = codec.serialize(root);
    cout << "Serialized: " << encoded << endl;
    
    TreeNode* restored = codec.deserialize(encoded);
    string re_encoded = codec.serialize(restored);
    cout << "Re-serialized: " << re_encoded << endl;
    cout << "Match: " << (encoded == re_encoded ? "YES" : "NO") << endl;
    
    // 测试空树
    TreeNode* nullRoot = NULL;
    string nullEncoded = codec.serialize(nullRoot);
    cout << "\nNull tree serialized: '" << nullEncoded << "'" << endl;
    
    // 测试单节点
    TreeNode* single = new TreeNode(42);
    string singleEncoded = codec.serialize(single);
    cout << "Single node serialized: " << singleEncoded << endl;
    
    return 0;
}
