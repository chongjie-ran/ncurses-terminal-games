# LC464 Can I Win (Medium)

## 题目
玩家轮流从1到maxChoosableInteger中选择未使用的数字，先达到totalDesired总和者获胜。判断先手是否能必胜。

## 思路
- 状态压缩DP：使用位掩码表示已选数字
- dp[mask] = 先手在当前状态是否能必胜
- 转移：遍历可选数字，选中后判断对手是否必败
- 剪枝：若所有可选数字和 < totalDesired，直接失败

## 复杂度
- 时间: O(2^n * n)
- 空间: O(2^n)

## 实现
```c
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool canIWin(int maxChoosableInteger, int totalDesired) {
    if (totalDesired == 0) return true;
    if (maxChoosableInteger * (maxChoosableInteger + 1) / 2 < totalDesired) return false;
    
    int totalStates = 1 << maxChoosableInteger;
    char* dp = malloc(totalStates);
    memset(dp, -1, totalStates);  // -1: unknown, 0: lose, 1: win
    
    // dp[mask] = 1 if current player can win, 0 if lose, -1 if unknown
    char canWin(int mask) {
        if (dp[mask] != -1) return dp[mask];
        
        for (int i = 1; i <= maxChoosableInteger; i++) {
            int bit = 1 << (i - 1);
            if (!(mask & bit)) {
                // Choose i
                int newMask = mask | bit;
                int newTotal = totalDesired - i;
                
                if (newTotal <= 0) {
                    dp[mask] = 1;
                    return 1;
                }
                
                // Opponent loses after this move
                if (canWin(newMask) == 0) {
                    dp[mask] = 1;
                    return 1;
                }
            }
        }
        
        dp[mask] = 0;
        return 0;
    }
    
    char result = canWin(0);
    free(dp);
    return result == 1;
}
```
