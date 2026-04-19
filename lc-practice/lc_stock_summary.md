# LC股票买卖DP专题 - 2026-04-19 02:36 CST

## 股票买卖问题家族

| 题号 | 难度 | 核心 | 状态 |
|------|------|------|------|
| LC121 | Easy | 1笔交易: min_price贪心 | ✅ |
| LC122 | Easy | 无限交易: 贪心累加 | ✅ |
| LC123 | Hard | 2笔交易: 分割点DP | ✅ |
| LC188 | Hard | k笔交易: DP+maxDiff | ✅ |
| LC309 | Medium | 冷却期: 状态机DP | ✅ |
| LC714 | Medium | 手续费: DP+fee | ✅ |

## 核心代码模式

### LC121 - 1交易（贪心）
```cpp
int min_price = INT_MAX, max_profit = 0;
for (int price : prices) {
    min_price = min(min_price, price);
    max_profit = max(max_profit, price - min_price);
}
```

### LC122 - 无限交易（贪心）
```cpp
int profit = 0;
for (int i = 1; i < prices.size(); i++)
    if (prices[i] > prices[i-1])
        profit += prices[i] - prices[i-1];
```

### LC123 - 2交易（分割点）
```cpp
// left[i]: max profit [0..i] with 1 transaction
// right[i]: max profit [i..n-1] with 1 transaction
// profit = max(left[i] + right[i])
```

### LC188 - k交易（DP）
```cpp
// dp[i] = max profit with i transactions
// maxDiff = max(maxDiff, dp[i-1] - prices[j])
// dp[i] = max(dp[i], prices[j] + maxDiff)
// Special: if k > n/2, use unlimited transactions
```

### LC309 - 冷却期（3状态）
```cpp
int sell = 0, prev_sell = 0, buy = -prices[0];
for (int i = 1; i < n; i++) {
    int prev_buy = buy;
    buy = max(buy, prev_sell - prices[i]);    // cooldown后可以买
    prev_sell = sell;
    sell = prev_buy + prices[i];
}
```

### LC714 - 手续费（fee在buy时扣）
```cpp
int sell = 0, buy = -prices[0];
for (int i = 1; i < n; i++) {
    buy = max(buy, sell - prices[i] - fee);   // fee在买时扣
    sell = max(sell, buy + prices[i]);
}
```

## 今日累计
- 股票买卖6题: LC121/122/123/188/309/714 ✅
- LC练习累计: ~131题

*悟通 | 2026-04-19 02:50 CST*
