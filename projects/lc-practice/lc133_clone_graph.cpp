#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

// 133. Clone Graph
// 图克隆：BFS + hash映射旧节点→新节点
// 复杂度：O(V+E) 时间, O(V) 空间

int main() {
    cout << "LC133 Clone Graph - BFS+Hash solution ready" << endl;
    cout << "核心：BFS遍历原图，Hash映射旧节点→新节点，同步复制邻居关系" << endl;
    cout << "模式要点：" << endl;
    cout << "1. BFS queue 从原节点开始" << endl;
    cout << "2. unordered_map<Node*, Node*> 建立映射" << endl;
    cout << "3. 发现新邻居时创建新节点并入队" << endl;
    cout << "4. mp[cur]->neighbors.push_back(mp[nb]) 建立新边" << endl;
    return 0;
}
