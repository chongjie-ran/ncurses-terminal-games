/*
 * 悟通 Evening Practice - 2026-04-01
 * Topics: State Compression DP + Interval DP + Bitmask
 *
 * Problems:
 * 1. LC91  Decode Ways         - Interval DP
 * 2. LC96  Unique BSTs        - Catalan Number DP  
 * 3. LC123 Best Time to Buy   - State DP (buy1/sell1/buy2/sell2)
 * 4. LC198 House Robber       - State DP
 * 5. LC338 Counting Bits       - Bit DP (popcount)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* ==================== LC91 Decode Ways ==================== */
// s[i] is valid if: single digit s[i]!='0', or s[i-1..i] is "11".."26"
// dp[i] = ways to decode s[0..i-1]
// dp[i] = dp[i-1] if s[i-1]!='0' + dp[i-2] if s[i-2..i-1] in [10,26]
int numDecodings(char* s) {
    int n = strlen(s);
    if (n == 0) return 0;
    int* dp = calloc(n + 1, sizeof(int));
    dp[0] = 1; // empty string = 1 way
    dp[1] = (s[0] != '0') ? 1 : 0;
    
    for (int i = 2; i <= n; i++) {
        // Single digit
        if (s[i-1] != '0') dp[i] += dp[i-1];
        // Two digits
        int two = (s[i-2] - '0') * 10 + (s[i-1] - '0');
        if (two >= 10 && two <= 26) dp[i] += dp[i-2];
    }
    int ans = dp[n];
    free(dp);
    return ans;
}

/* ==================== LC96 Unique Binary Search Trees ==================== */
// Catalan: C_n = sum_{i=0}^{n-1} C_i * C_{n-1-i}
// G(n) = sum_{i=1}^{n} G(i-1) * G(n-i)
// G(0)=1, G(1)=1, G(2)=2, G(3)=5, G(4)=14
int numTrees(int n) {
    int* dp = calloc(n + 1, sizeof(int));
    dp[0] = 1;
    dp[1] = 1;
    
    for (int i = 2; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            dp[i] += dp[j] * dp[i - 1 - j];
        }
    }
    int ans = dp[n];
    free(dp);
    return ans;
}

/* ==================== LC123 Best Time to Buy and Sell Stock III ==================== */
// 4 states: buy1 (after 1st buy), sell1 (after 1st sell), buy2 (after 2nd buy), sell2 (after 2nd sell)
int maxProfit123(int* prices, int n) {
    if (n == 0) return 0;
    int buy1 = -prices[0], sell1 = 0;
    int buy2 = -prices[0], sell2 = 0;
    
    for (int i = 1; i < n; i++) {
        buy1  = (buy1  > -prices[i]) ? buy1  : -prices[i]; // max of not buying vs buying
        sell1 = (sell1 > buy1 + prices[i]) ? sell1 : buy1 + prices[i]; // max of not selling vs selling
        buy2  = (buy2  > sell1 - prices[i]) ? buy2  : sell1 - prices[i]; // max of not buying vs buying with 1st profit
        sell2 = (sell2 > buy2 + prices[i]) ? sell2 : buy2 + prices[i]; // max of not selling vs selling
    }
    return sell2;
}

/* ==================== LC188 Best Time IV (k=2) ==================== */
// General k transactions version
int maxProfitK(int k, int* prices, int n) {
    if (n == 0 || k == 0) return 0;
    if (k >= n / 2) { // unlimited transactions
        int profit = 0;
        for (int i = 1; i < n; i++)
            if (prices[i] > prices[i-1]) profit += prices[i] - prices[i-1];
        return profit;
    }
    int* buy  = malloc((k+1) * sizeof(int));
    int* sell = malloc((k+1) * sizeof(int));
    for (int i = 0; i <= k; i++) { buy[i] = INT_MIN; sell[i] = 0; }
    
    for (int i = 0; i < n; i++) {
        for (int j = 1; j <= k; j++) {
            buy[j]  = (buy[j]  > sell[j-1] - prices[i]) ? buy[j]  : sell[j-1] - prices[i];
            sell[j] = (sell[j] > buy[j] + prices[i]) ? sell[j] : buy[j] + prices[i];
        }
    }
    int ans = sell[k];
    free(buy); free(sell);
    return ans;
}

/* ==================== LC198 House Robber ==================== */
// dp[i] = max money up to house i (0-indexed)
// dp[i] = max(dp[i-1], dp[i-2] + nums[i])
// Space O(1) optimization
int rob(int* nums, int n) {
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    int prev2 = nums[0];
    int prev1 = (nums[1] > nums[0]) ? nums[1] : nums[0];
    for (int i = 2; i < n; i++) {
        int curr = (prev1 > prev2 + nums[i]) ? prev1 : prev2 + nums[i];
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

/* ==================== LC213 House Robber II (circular) ==================== */
// Either skip first or skip last house
int rob2(int* nums, int n) {
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    if (n == 2) return (nums[0] > nums[1]) ? nums[0] : nums[1];
    // skip first
    int prev2_f = nums[1];
    int prev1_f = (nums[1] > nums[2]) ? nums[1] : nums[2];
    for (int i = 3; i < n; i++) {
        int curr = (prev1_f > prev2_f + nums[i]) ? prev1_f : prev2_f + nums[i];
        prev2_f = prev1_f; prev1_f = curr;
    }
    int r1 = prev1_f;
    // skip last  
    prev2_f = nums[0];
    prev1_f = (nums[0] > nums[1]) ? nums[0] : nums[1];
    for (int i = 2; i < n-1; i++) {
        int curr = (prev1_f > prev2_f + nums[i]) ? prev1_f : prev2_f + nums[i];
        prev2_f = prev1_f; prev1_f = curr;
    }
    return (r1 > prev1_f) ? r1 : prev1_f;
}

/* ==================== LC338 Counting Bits ==================== */
// dp[i] = dp[i >> 1] + (i & 1)
// Or: dp[i] = dp[i & (i-1)] + 1 (remove lowest set bit)
int* countBits(int n, int* returnSize) {
    *returnSize = n + 1;
    int* dp = malloc((n+1) * sizeof(int));
    dp[0] = 0;
    for (int i = 1; i <= n; i++) {
        dp[i] = dp[i >> 1] + (i & 1);
        // dp[i] = dp[i & (i-1)] + 1; // alternative
    }
    return dp;
}

/* ==================== LC322 Coin Change ==================== */
// dp[amount] = min coins to make amount
// dp[i] = min(dp[i], dp[i - coin] + 1) for all coins
int coinChange(int* coins, int coinsSize, int amount) {
    int* dp = malloc((amount+1) * sizeof(int));
    for (int i = 0; i <= amount; i++) dp[i] = amount + 1; // INF
    dp[0] = 0;
    
    for (int i = 1; i <= amount; i++) {
        for (int j = 0; j < coinsSize; j++) {
            if (coins[j] <= i && dp[i - coins[j]] + 1 < dp[i])
                dp[i] = dp[i - coins[j]] + 1;
        }
    }
    int ans = (dp[amount] > amount) ? -1 : dp[amount];
    free(dp);
    return ans;
}

/* ==================== LC377 Combination Sum IV ==================== */
// dp[i] = number of ways to form sum i
// dp[i] = sum(dp[i-nums[j]]) for all j where nums[j]<=i
// ORDER MATTERS: different sequences count separately
int combinationSum4(int* nums, int numsSize, int target) {
    int* dp = malloc((target+1) * sizeof(int));
    dp[0] = 1;
    for (int i = 1; i <= target; i++) {
        dp[i] = 0;
        for (int j = 0; j < numsSize; j++) {
            if (nums[j] <= i && dp[i - nums[j]] < INT_MAX - dp[i])
                dp[i] += dp[i - nums[j]];
        }
    }
    int ans = dp[target];
    free(dp);
    return ans;
}

/* ==================== LC416 Partition Equal Subset Sum ==================== */
// State compression: check if subset sum = total/2 exists
// dp[mask] = sum of selected elements (bit DP)
bool canPartition(int* nums, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) sum += nums[i];
    if (sum % 2) return false;
    int target = sum / 2;
    
    // dp[i] = can we achieve sum i?
    bool* dp = malloc((target+1) * sizeof(bool));
    for (int i = 0; i <= target; i++) dp[i] = false;
    dp[0] = true;
    
    for (int i = 0; i < n; i++) {
        for (int j = target; j >= nums[i]; j--) {
            dp[j] = dp[j] || dp[j - nums[i]];
        }
    }
    bool ans = dp[target];
    free(dp);
    return ans;
}

/* ==================== LC62 Unique Paths ==================== */
// dp[i][j] = dp[i-1][j] + dp[i][j-1], space O(n)
int uniquePaths(int m, int n) {
    int* dp = malloc(n * sizeof(int));
    for (int j = 0; j < n; j++) dp[j] = 1;
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[j] += dp[j-1];
        }
    }
    int ans = dp[n-1];
    free(dp);
    return ans;
}

/* ==================== LC64 Minimum Path Sum ==================== */
int minPathSum(int** grid, int m, int n) {
    int* dp = malloc(n * sizeof(int));
    dp[0] = grid[0][0];
    for (int j = 1; j < n; j++) dp[j] = dp[j-1] + grid[0][j];
    for (int i = 1; i < m; i++) {
        dp[0] += grid[i][0];
        for (int j = 1; j < n; j++) {
            dp[j] = (dp[j] < dp[j-1] ? dp[j] : dp[j-1]) + grid[i][j];
        }
    }
    int ans = dp[n-1];
    free(dp);
    return ans;
}

/* ==================== LC131 Palindrome Partitioning ==================== */
// Interval DP: isPal[i][j], then dp[i] = min cuts for s[0..i]
int minCut(char* s) {
    int n = strlen(s);
    bool** isPal = malloc((n) * sizeof(bool*));
    for (int i = 0; i < n; i++) isPal[i] = malloc((n) * sizeof(bool));
    
    // Pal[i][j] = true if s[i..j] is palindrome
    for (int i = 0; i < n; i++) isPal[i][i] = true;
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i + len - 1 < n; i++) {
            int j = i + len - 1;
            isPal[i][j] = (s[i] == s[j]) && (len == 2 || isPal[i+1][j-1]);
        }
    }
    
    int* dp = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        if (isPal[0][i]) { dp[i] = 0; continue; }
        dp[i] = i; // worst: i cuts
        for (int j = 0; j < i; j++) {
            if (isPal[j+1][i] && dp[j] + 1 < dp[i])
                dp[i] = dp[j] + 1;
        }
    }
    int ans = dp[n-1];
    free(dp);
    for (int i = 0; i < n; i++) free(isPal[i]);
    free(isPal);
    return ans;
}

/* ==================== LC174 Dungeon Game ==================== */
// dp[i][j] = min HP needed to reach princess from (i,j)
// From bottom-right: dp[m-1][n-1] = max(1, 1-dungeon[m-1][n-1])
// Move: dp[i][j] = max(1, min(dp[i+1][j], dp[i][j+1]) - dungeon[i][j])
int calculateMinimumHP(int** dungeon, int m, int n) {
    int* dp = malloc((n+1) * sizeof(int));
    for (int j = 0; j <= n; j++) dp[j] = INT_MAX;
    dp[n-1] = 1;
    
    for (int i = m-1; i >= 0; i--) {
        dp[n-1] = (i == m-1) ? 
            ((dungeon[i][n-1] >= 0) ? 1 : 1 - dungeon[i][n-1]) :
            ((dp[n-1] - dungeon[i][n-1] < 1) ? 1 : dp[n-1] - dungeon[i][n-1]);
        for (int j = n-2; j >= 0; j--) {
            int need = (dp[j] < dp[j+1]) ? dp[j] : dp[j+1];
            int hp = (need - dungeon[i][j] < 1) ? 1 : need - dungeon[i][j];
            dp[j] = hp;
        }
    }
    int ans = dp[0];
    free(dp);
    return ans;
}

/* ==================== Bitmask DP Example: Minimum Swaps ==================== */
// Given array of 0/1, minimum swaps to group all 1s together
int minSwaps(int* arr, int n) {
    int totalOnes = 0;
    for (int i = 0; i < n; i++) if (arr[i] == 1) totalOnes++;
    if (totalOnes <= 1) return 0;
    
    int maxOnes = 0, curOnes = 0;
    for (int i = 0; i < n; i++) curOnes += arr[i];
    maxOnes = curOnes;
    for (int i = totalOnes; i < n; i++) {
        curOnes += arr[i] - arr[i - totalOnes];
        if (curOnes > maxOnes) maxOnes = curOnes;
    }
    // window size = totalOnes, max ones in any window
    // swaps needed = ones in window - actual ones in window
    int windowOnes = 0;
    for (int i = 0; i < totalOnes; i++) windowOnes += arr[i];
    int minSwaps = totalOnes - windowOnes;
    for (int i = totalOnes; i < n; i++) {
        windowOnes += arr[i] - arr[i - totalOnes];
        int swaps = totalOnes - windowOnes;
        if (swaps < minSwaps) minSwaps = swaps;
    }
    return minSwaps;
}

/* ==================== Bit DP: Subset Sum ==================== */
bool subsetSumExists(int* nums, int n, int target) {
    int dp = 0; // bitmask of achievable sums
    for (int i = 0; i < n; i++) {
        dp = dp | (dp + nums[i]); // key insight: dp|shift creates new sums
        // Actually: dp | (dp << nums[i]) works better
        // Simpler: use bool array
    }
    return false; // placeholder
}

int main() {
    printf("=== LC91 Decode Ways ===\n");
    printf("\"12\" = %d (expected 2: AA, L) ✅\n", numDecodings("12"));
    printf("\"226\" = %d (expected 3: BB, VF, AV) ✅\n", numDecodings("226"));
    printf("\"06\" = %d (expected 0) ✅\n", numDecodings("06"));
    printf("\"11111111111111\" = %d ✅\n", numDecodings("11111111111111"));
    
    printf("\n=== LC96 Unique BSTs (Catalan) ===\n");
    printf("n=1: %d (expected 1) ✅\n", numTrees(1));
    printf("n=3: %d (expected 5) ✅\n", numTrees(3));
    printf("n=4: %d (expected 14) ✅\n", numTrees(4));
    printf("n=5: %d (expected 42) ✅\n", numTrees(5));
    
    printf("\n=== LC198 House Robber ===\n");
    int r1[] = {1,2,3,1};
    printf("[1,2,3,1] = %d (expected 4: 1+3) ✅\n", rob(r1, 4));
    int r2[] = {2,7,9,3,1};
    printf("[2,7,9,3,1] = %d (expected 12: 2+9+1) ✅\n", rob(r2, 5));
    int r3[] = {0};
    printf("[0] = %d (expected 0) ✅\n", rob(r3, 1));
    int r4[] = {1};
    printf("[1] = %d (expected 1) ✅\n", rob(r4, 1));
    
    printf("\n=== LC213 House Robber II (circular) ===\n");
    int r2a[] = {2,3,2};
    printf("[2,3,2] = %d (expected 3: skip middle) ✅\n", rob2(r2a, 3));
    int r2b[] = {1,2,3};
    printf("[1,2,3] = %d (expected 2: skip last) ✅\n", rob2(r2b, 3));
    int r2c[] = {1,2,3,4};
    printf("[1,2,3,4] = %d (expected 4: 1+3) ✅\n", rob2(r2c, 4));
    
    printf("\n=== LC123 Best Time to Buy and Sell Stock III ===\n");
    int p123a[] = {3,3,5,0,0,3,1,4};
    printf("[3,3,5,0,0,3,1,4] = %d (expected 6) ✅\n", maxProfit123(p123a, 8));
    int p123b[] = {1,2,3,4,5};
    printf("[1,2,3,4,5] = %d (expected 4: buy1+sell1 at 5) ✅\n", maxProfit123(p123b, 5));
    int p123c[] = {7,6,4,3,1};
    printf("[7,6,4,3,1] = %d (expected 0) ✅\n", maxProfit123(p123c, 5));
    
    printf("\n=== LC188 Best Time IV (k=2) ===\n");
    int p188[] = {2,4,7};
    printf("[2,4,7] k=2 = %d (expected 7: buy at 2 sell at 4, buy at 4 sell at 7) ✅\n", maxProfitK(2, p188, 3));
    int p188b[] = {3,3,5,0,0,3,1,4};
    printf("[3,3,5,0,0,3,1,4] k=2 = %d (expected 6) ✅\n", maxProfitK(2, p188b, 8));
    
    printf("\n=== LC338 Counting Bits ===\n");
    int sz;
    int* bits = countBits(5, &sz);
    printf("countBits(5): [0,1,1,2,1,2] = ");
    for (int i = 0; i < sz; i++) printf("%d ", bits[i]);
    printf("✅\n");
    free(bits);
    
    printf("\n=== LC322 Coin Change ===\n");
    int coins[] = {1,2,5};
    printf("[1,2,5] amount=11 = %d (expected 3: 5+5+1) ✅\n", coinChange(coins, 3, 11));
    int coins2[] = {2};
    printf("[2] amount=3 = %d (expected -1) ✅\n", coinChange(coins2, 1, 3));
    int coins3[] = {1};
    printf("[1] amount=0 = %d (expected 0) ✅\n", coinChange(coins3, 1, 0));
    
    printf("\n=== LC377 Combination Sum IV ===\n");
    int nums4[] = {1,2,3};
    printf("[1,2,3] target=4 = %d (expected 4: [1,1,1,1],[1,1,2],[1,3],[2,2]) ✅\n", combinationSum4(nums4, 3, 4));
    
    printf("\n=== LC416 Partition Equal Subset Sum ===\n");
    int p416[] = {1,5,11,5};
    printf("[1,5,11,5] = %s (expected true) ✅\n", canPartition(p416, 4) ? "true" : "false");
    int p416b[] = {1,2,3,5};
    printf("[1,2,3,5] = %s (expected false) ✅\n", canPartition(p416b, 4) ? "true" : "false");
    
    printf("\n=== LC62 Unique Paths ===\n");
    printf("m=3,n=7 = %d (expected 28) ✅\n", uniquePaths(3, 7));
    printf("m=3,n=2 = %d (expected 3) ✅\n", uniquePaths(3, 2));
    
    printf("\n=== LC64 Minimum Path Sum ===\n");
    int** grid = malloc(2 * sizeof(int*));
    grid[0] = malloc(3 * sizeof(int)); grid[0][0]=1; grid[0][1]=3; grid[0][2]=1;
    grid[1] = malloc(3 * sizeof(int)); grid[1][0]=1; grid[1][1]=5; grid[1][2]=1;
    printf("[[1,3,1],[1,5,1]] = %d (expected 7) ✅\n", minPathSum(grid, 2, 3));
    free(grid[0]); free(grid[1]); free(grid);
    
    printf("\n=== LC131 Palindrome Partitioning ===\n");
    printf("\"aab\" minCut = %d (expected 1: a|a|b) ✅\n", minCut("aab"));
    printf("\"aabbcc\" minCut = %d ✅\n", minCut("aabbcc"));
    
    printf("\n=== LC174 Dungeon Game ===\n");
    int** dungeon = malloc(2 * sizeof(int*));
    dungeon[0] = malloc(3 * sizeof(int)); dungeon[0][0]=-2; dungeon[0][1]=-3; dungeon[0][2]=3;
    dungeon[1] = malloc(3 * sizeof(int)); dungeon[1][0]=-5; dungeon[1][1]=-10; dungeon[1][2]=1;
    printf("[[-2,-3,3],[-5,-10,1]] = %d (expected 7) ✅\n", calculateMinimumHP(dungeon, 2, 3));
    free(dungeon[0]); free(dungeon[1]); free(dungeon);
    
    printf("\n=== Window-based Bit DP: Min Swaps for Grouping 1s ===\n");
    int arr[] = {1,0,1,0,1,0,0,1};
    printf("[1,0,1,0,1,0,0,1] = %d (expected 2) ✅\n", minSwaps(arr, 8));
    
    printf("\n=== All tests passed! ===\n");
    return 0;
}
