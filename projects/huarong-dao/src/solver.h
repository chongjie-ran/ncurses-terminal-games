#ifndef SOLVER_H
#define SOLVER_H

#include "board.h"
#include <queue>
#include <unordered_map>
#include <vector>

// BFS solver for Huarong Dao — find minimum moves
class Solver {
public:
    struct Result {
        int moves;
        std::vector<std::string> path;  // state keys in order
    };

    // BFS: returns minimum moves to goal, -1 if unsolvable
    Result solve(const Board& start);

private:
    std::unordered_map<std::string, int> dist_;
    std::unordered_map<std::string, std::string> parent_;
};

#endif
