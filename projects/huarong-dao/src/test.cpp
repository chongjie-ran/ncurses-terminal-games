#include "board.h"
#include "solver.h"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <tuple>
#include <ctime>

int main() {
    Board start = Board::initial();
    std::cout << "=== 华容道 BFS 求解 ===\n";
    start.print();

    Solver solver;
    auto t0 = std::clock();
    auto result = solver.solve(start);
    auto ms = (std::clock() - t0) * 1000 / CLOCKS_PER_SEC;

    if (result.moves > 0) {
        std::cout << "最短步数: " << result.moves << " | 耗时: " << ms << " ms | 状态数: " << result.path.size() << "\n";
    } else if (result.moves == 0) {
        std::cout << "已是目标状态 (0步)\n";
    } else {
        std::cout << "无解! (探索了 " << result.path.size() << " 状态)\n";
    }
    return result.moves >= 0 ? 0 : 1;
}
