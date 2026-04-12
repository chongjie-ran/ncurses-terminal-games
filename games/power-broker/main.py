#!/usr/bin/env python3
"""
Power Broker v1.3 MVP - 算力经纪人
玩家扮演算力经纪人，在AI芯片短缺时代调配全球算力资源
"""
import random
import os

# 常量
INITIAL_CASH = 100000
INITIAL_POWER = 100
DAY_LIMIT = 30
NUM_REGIONS = 5

# 地区信息
REGIONS = [
    {"name": "北美", "base_price": 100, "volatility": 25, "demand": 100},
    {"name": "欧洲", "base_price": 95, "volatility": 20, "demand": 90},
    {"name": "亚洲", "base_price": 85, "volatility": 30, "demand": 110},
    {"name": "南美", "base_price": 70, "volatility": 15, "demand": 60},
    {"name": "非洲", "base_price": 50, "volatility": 10, "demand": 40},
]

# 事件库
EVENTS = [
    {"name": "AI芯片禁令", "desc": "美国扩大芯片出口管制", "impact": {"北美": 50, "亚洲": 30}, "market": 35},
    {"name": "数据中心故障", "desc": "某大厂数据中心宕机", "impact": {"亚洲": 25}, "market": 20},
    {"name": "新能源突破", "desc": "电价暴跌，算力成本降低", "impact": {"南美": -20, "非洲": -25}, "market": -15},
    {"name": "新矿难", "desc": "内蒙古矿难，GPU供应紧张", "impact": {"亚洲": 40}, "market": 30},
    {"name": "大厂抛售", "desc": "某巨头出售过剩算力", "impact": {"北美": -30, "欧洲": -25}, "market": -25},
    {"name": "政策利好", "desc": "国家出台AI算力扶持政策", "impact": {"亚洲": 25}, "market": 15},
    {"name": "5nm量产", "desc": "新一代制程量产，成本下降", "impact": {"北美": -30, "亚洲": -25}, "market": -20},
    {"name": "战争风险", "desc": "地缘冲突升级", "impact": {"南美": 30, "非洲": 25}, "market": 25},
]

class Game:
    def __init__(self):
        self.day = 1
        self.cash = INITIAL_CASH
        self.power = [0] * NUM_REGIONS
        self.prices = [r["base_price"] + random.randint(-10, 10) for r in REGIONS]
        self.total_trades = 0
        self.running = True
        self.power[2] = INITIAL_POWER  # 亚洲初始持仓
        
    def clear(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        
    def draw_header(self):
        print("\n" + "=" * 60)
        print(f"          💎 算力经纪人 v1.3 MVP | 第 {self.day} / {DAY_LIMIT} 天")
        print("=" * 60)
        
    def draw_market(self):
        print("\n  📈 全球算力市场:")
        print("  +------------+----------+----------+----------+")
        print("  | 地区       | 价格     | 波动     | 需求     |")
        print("  +------------+----------+----------+----------+")
        for i, r in enumerate(REGIONS):
            trend = "📈" if self.prices[i] > r["base_price"] else ("📉" if self.prices[i] < r["base_price"] else "➖")
            mark = " ★" if r["demand"] == max(REGIONS, key=lambda x: x["demand"])["demand"] else ""
            print(f"  | {mark}{r['name']:<9} | ${self.prices[i]:>6}  | {r['volatility']:>3}%    | {r['demand']:>3}%    |")
        print("  +------------+----------+----------+----------+")
        
    def draw_assets(self):
        total = self.total_assets()
        profit = total - (INITIAL_CASH + INITIAL_POWER * 100)
        print("\n  📊 资产状况:")
        print(f"  | 现金: ${self.cash:>10,}              |")
        holdings_val = total - self.cash
        print(f"  | 算力市值: ${holdings_val:>8,}            |")
        print("  +----------------------------------+")
        print(f"  | 总资产: ${total:>10,}              |")
        profit_str = f"+${profit:,}" if profit >= 0 else f"-${-profit:,}"
        print(f"  | 盈亏: {profit_str:>15}         |")
        print("  +----------------------------------+")
        
    def total_assets(self):
        return self.cash + sum(self.power[i] * self.prices[i] for i in range(NUM_REGIONS))
        
    def buy(self, region_id, amount):
        if region_id < 0 or region_id >= NUM_REGIONS:
            print("  [失败] 无效的地区编号")
            return False
        cost = amount * self.prices[region_id]
        if cost > self.cash:
            print(f"  [失败] 资金不足！需要 ${cost:,}，只有 ${self.cash:,}")
            return False
        self.cash -= cost
        self.power[region_id] += amount
        self.total_trades += 1
        print(f"  [成功] 买入 {amount} 算力，单价 ${self.prices[region_id]}，总价 ${cost:,}")
        return True
        
    def sell(self, region_id, amount):
        if region_id < 0 or region_id >= NUM_REGIONS:
            print("  [失败] 无效的地区编号")
            return False
        if amount > self.power[region_id]:
            print(f"  [失败] 持有不足！持有 {self.power[region_id]}，卖出 {amount}")
            return False
        revenue = amount * self.prices[region_id]
        self.cash += revenue
        self.power[region_id] -= amount
        self.total_trades += 1
        print(f"  [成功] 卖出 {amount} 算力，单价 ${self.prices[region_id]}，收入 ${revenue:,}")
        return True
        
    def next_day(self):
        # 40%概率触发事件
        event = None
        if random.random() < 0.4:
            event = random.choice(EVENTS)
            print(f"\n  📢 【事件】{event['name']}")
            print(f"  {event['desc']}")
            if event['impact']:
                impacts = ", ".join(f"{k}{'+' if v > 0 else ''}{v}" for k, v in event['impact'].items())
                print(f"  影响: {impacts}")
                
        # 更新价格
        for i in range(NUM_REGIONS):
            r = REGIONS[i]
            change = random.randint(-r["volatility"], r["volatility"])
            if event:
                for region_name, impact in event['impact'].items():
                    if region_name == r['name']:
                        change += impact
                change += event['market']
            self.prices[i] = max(r['base_price'] // 2, min(r['base_price'] * 3, self.prices[i] + change))
            
        self.day += 1
        return self.day <= DAY_LIMIT
        
    def show_help(self):
        print("\n  📖 操作指南:")
        print("  +------------------------------------------+")
        print("  | buy <1-5> <数量>  - 买入算力            |")
        print("  | sell <1-5> <数量> - 卖出算力            |")
        print("  | next                - 进入下一天           |")
        print("  | info                - 查看资产详情        |")
        print("  | quit                - 退出游戏            |")
        print("  +------------------------------------------+")
        print("  地区: 1-北美 2-欧洲 3-亚洲 4-南美 5-非洲")
        
    def run(self):
        self.clear()
        print("\n" + "=" * 60)
        print("          💎 欢迎来到算力经纪人 v1.3 MVP 💎")
        print("=" * 60)
        print("\n  你是一位算力经纪人，在AI芯片短缺时代调配全球算力资源。")
        print("  你的目标是在30天内最大化你的资产！")
        print(f"\n  📋 初始资金: ${INITIAL_CASH:,}")
        print(f"  📋 初始算力: {INITIAL_POWER} 单位（亚洲市场）")
        print(f"  📋 游戏天数: {DAY_LIMIT}天")
        input("\n  按回车开始游戏...")
        
        while self.running and self.day <= DAY_LIMIT:
            self.clear()
            self.draw_header()
            self.draw_market()
            self.draw_assets()
            print("\n  > ", end="")
            cmd = input().strip().split()
            
            if not cmd:
                continue
            elif cmd[0] == "buy" and len(cmd) >= 3:
                self.buy(int(cmd[1]) - 1, int(cmd[2]))
                input("  按回车继续...")
            elif cmd[0] == "sell" and len(cmd) >= 3:
                self.sell(int(cmd[1]) - 1, int(cmd[2]))
                input("  按回车继续...")
            elif cmd[0] == "next":
                if not self.next_day():
                    self.running = False
                input("  按回车继续...")
            elif cmd[0] == "info":
                for i in range(NUM_REGIONS):
                    if self.power[i] > 0:
                        print(f"  {REGIONS[i]['name']}: {self.power[i]} 单位, 市值 ${self.power[i] * self.prices[i]:,}")
                input("  按回车继续...")
            elif cmd[0] in ("help", "?"):
                self.show_help()
                input("  按回车继续...")
            elif cmd[0] in ("quit", "q"):
                self.running = False
                
        # 游戏结束
        self.clear()
        total = self.total_assets()
        profit = total - (INITIAL_CASH + INITIAL_POWER * 100)
        roi = profit / (INITIAL_CASH + INITIAL_POWER * 100) * 100
        
        print("\n" + "=" * 60)
        print("                    🏆 游戏结束 - 第30天")
        print("=" * 60)
        print(f"\n  📊 最终资产: ${total:,}")
        profit_str = f"+${profit:,}" if profit >= 0 else f"-${-profit:,}"
        roi_str = f"+{roi:.1f}%" if roi >= 0 else f"{roi:.1f}%"
        print(f"  📈 总盈亏: {profit_str} ({roi_str})")
        print(f"  🔢 交易次数: {self.total_trades} 次")
        
        if roi >= 50:
            print("  🌟 评价: 传奇经纪人！算力市场的传奇人物！")
        elif roi >= 20:
            print("  ⭐ 评价: 优秀！你对市场有敏锐的洞察力！")
        elif roi >= 0:
            print("  👍 评价: 稳健开局，继续努力！")
        else:
            print("  💪 评价: 市场有风险，下次一定会更好！")
        print()
        input("  按回车退出...")

if __name__ == "__main__":
    random.seed()
    Game().run()
