#include "solver.h"
#include "board.h"
#include <queue>
#include <unordered_map>

Solver::Result Solver::solve(const Board& start) {
    Result result;
    result.moves = -1;

    if (start.isGoal()) {
        result.moves = 0;
        result.path.push_back(start.stateKey());
        return result;
    }

    std::queue<Board> q;
    q.push(start);
    dist_[start.stateKey()] = 0;
    parent_[start.stateKey()] = "";

    while (!q.empty()) {
        Board cur = q.front(); q.pop();
        std::string curKey = cur.stateKey();
        int curDist = dist_[curKey];

        auto moves = cur.validMoves();
        for (auto& mv : moves) {
            int pi, dr, dc;
            std::tie(pi, dr, dc) = mv;
            Board nxt = cur.afterMove(pi, dr, dc);
            std::string nxtKey = nxt.stateKey();

            if (dist_.find(nxtKey) == dist_.end()) {
                dist_[nxtKey] = curDist + 1;
                parent_[nxtKey] = curKey;
                if (nxt.isGoal()) {
                    result.moves = dist_[nxtKey];
                    // reconstruct path
                    std::string k = nxtKey;
                    while (k != "") {
                        result.path.push_back(k);
                        k = parent_[k];
                    }
                    reverse(result.path.begin(), result.path.end());
                    return result;
                }
                q.push(nxt);
            }
        }
    }
    return result;
}
