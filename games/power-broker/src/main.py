#!/usr/bin/env python3
"""
算力经纪人 - MVP
"""

import random, os
from dataclasses import dataclass, field
from typing import List, Dict, Optional

INITIAL_CASH, INITIAL_POWER = 100000, 100
GAME_DAYS = 30
MIN_PRICE, MAX_PRICE = 20, 500

MARKET_EVENTS = [
    {"name": "AI芯片禁令", "desc": "美国扩大芯片出口管制", "impact": +40},
    {"name": "数据中心故障", "desc": "某大厂数据中心宕机", "impact": +25},
    {"name": "新能源突破", "desc": "电价暴跌，算力成本降低", "impact": -20},
    {"name": "新矿难", "desc": "内蒙古矿难，GPU供应紧张", "impact": +30},
    {"name": "大厂抛售", "desc": "某巨头出售过剩算力", "impact": -35},
    {"name": "政策利好", "desc": "国家出台AI算力扶持政策", "impact": +15},
    {"name": "黑客攻击", "desc": "交易所遭攻击，市场恐慌", "impact": -15},
    {"name": "5nm量产", "desc": "新一代制程量产，成本下降", "impact": -25},
    {"name": "算力泡沫", "desc": "市场过热，监管介入", "impact": -20},
    {"name": "战争风险", "desc": "地缘冲突升级，资源紧张", "impact": +35},
]

REGIONS_DEF = [
    {"name": "北美", "base_price": 100, "volatility": 1.2, "demand": 1.5},
    {"name": "欧洲", "base_price": 95, "volatility": 1.0, "demand": 1.3},
    {"name": "亚洲", "base_price": 85, "volatility": 1.5, "demand": 1.8},
    {"name": "南美", "base_price": 70, "volatility": 0.8, "demand": 0.8},
    {"name": "非洲", "base_price": 50, "volatility": 0.6, "demand": 0.5},
]

@dataclass
class PowerRegion:
    name: str
    base_price: float
    volatility: float
    demand: float
    current_price: float = 0
    supply: int = 0
    demand_amount: int = 0
    
    def __post_init__(self):
        self.current_price = self.base_price
        self.supply = random.randint(50, 200)
        self.demand_amount = int(self.supply * self.demand * random.uniform(0.5, 1.5))
    
    def update_price(self, event: Optional[Dict] = None):
        change = random.uniform(-0.1, 0.1) * self.volatility
        if self.supply > 0:
            change += (self.demand_amount / self.supply - 1) * 0.05
        if event:
            change += event["impact"] / 100.0
        self.current_price = max(MIN_PRICE, min(MAX_PRICE, self.current_price * (1 + change)))
        self.supply = random.randint(50, 200)
        self.demand_amount = int(self.supply * self.demand * random.uniform(0.5, 1.5))

@dataclass
class Player:
    cash: float
    inventory: int = 0
    history: List[Dict] = field(default_factory=list)
    
    def buy(self, amount: int, price: float) -> bool:
        cost = amount * price
        if cost > self.cash:
            print(f"  资金不足! 需要${cost:,.0f},只有${self.cash:,.0f}")
            return False
        self.cash -= cost
        self.inventory += amount
        self.history.append({"type": "buy", "amount": amount, "price": price})
        return True
    
    def sell(self, amount: int, price: float) -> bool:
        if amount > self.inventory:
            print(f"  库存不足!只有{self.inventory}单位")
            return False
        self.cash += amount * price
        self.inventory -= amount
        self.history.append({"type": "sell", "amount": amount, "price": price})
        return True
    
    def total_assets(self, price: float) -> float:
        return self.cash + self.inventory * price
    
    def profit(self) -> float:
        return self.cash + self.inventory * 100 - (INITIAL_CASH + INITIAL_POWER * 100)

@dataclass
class Market:
    regions: List[PowerRegion] = field(default_factory=list)
    event: Optional[Dict] = None
    
    def __post_init__(self):
        for r in REGIONS_DEF:
            self.regions.append(PowerRegion(**r))
    
    def update(self):
        self.event = random.choice(MARKET_EVENTS) if random.random() < 0.3 else None
        for r in self.regions:
            r.update_price(self.event)

class PowerBrokerGame:
    def __init__(self):
        self.player = Player(cash=INITIAL_CASH, inventory=INITIAL_POWER)
        self.market = Market()
        self.day = 1
        self.running = True
    
    def clear(self):
        os.system('cls' if os.name == 'nt' else 'clear')
    
    def show(self):
        print("=" * 60)
        print("  算力经纪人 - Power Broker MVP")
        print("=" * 60)
        print(f"\n【第{self.day}/{GAME_DAYS}天】")
        print("-" * 40)
        avg = sum(r.current_price for r in self.market.regions) / len(self.market.regions)
        p = self.player.profit()
        print(f"现金: ${self.player.cash:,.0f}")
        print(f"库存: {self.player.inventory}单位")
        print(f"总资产: ${self.player.total_assets(avg):,.0f}")
        print(f"盈亏: ${p:+,.0f}")
        if self.market.event:
            e = self.market.event
            print(f"\n事件: {e['name']} - {e['desc']} ({e['impact']:+.0f}%)")
        print("\n【全球算力市场】")
        print("-" * 50)
        print(f"{'地区':<8}{'价格':>10}{'供应':>8}{'需求':>8}{'机会':>8}")
        print("-" * 50)
        for r in self.market.regions:
            opp = "买入" if r.current_price < r.base_price * 0.9 else "卖出" if r.current_price > r.base_price * 1.1 else "观望"
            print(f"{r.name:<8}${r.current_price:>8.0f}{r.supply:>8}{r.demand_amount:>8}{opp:>8}")
        print("-" * 50)
        print("\n操作: buy<地区><数量> sell<地区><数量> next info quit")
    
    def run(self):
        self.clear()
        print("算力经纪人MVP")
        print(f"目标:{GAME_DAYS}天最大化资产 初始:${INITIAL_CASH:,}+{INITIAL_POWER}单位")
        input("回车开始...")
        while self.running and self.day <= GAME_DAYS:
            self.clear()
            self.show()
            cmd = input("> ").strip().lower().split()
            if not cmd:
                continue
            if cmd[0] == 'quit':
                break
            elif cmd[0] == 'next':
                self.market.update()
                self.day += 1
            elif cmd[0] == 'info':
                print(f"现金:${self.player.cash:,.0f} 库存:{self.player.inventory} 交易:{len(self.player.history)}笔")
                input("回车继续...")
            elif cmd[0] in ('buy', 'sell') and len(cmd) >= 3:
                r = next((x for x in self.market.regions if x.name == cmd[1]), None)
                if r and cmd[2].isdigit():
                    amount = int(cmd[2])
                    if cmd[0] == 'buy':
                        self.player.buy(amount, r.current_price)
                    else:
                        self.player.sell(amount, r.current_price)
                input("回车继续...")
        # 结束
        self.clear()
        print("【游戏结束】")
        p = self.player.profit()
        pct = p / (INITIAL_CASH + INITIAL_POWER * 100) * 100
        print(f"最终资产:${self.player.total_assets(100):,.0f}")
        print(f"盈亏:${p:+,.0f}({pct:+.1f}%)")
        grade = "S级" if pct > 50 else "A级" if pct > 20 else "B级" if pct > 0 else "C级" if pct > -20 else "D级"
        print(f"评级:{grade}")
        print(f"交易次数:{len(self.player.history)}")

if __name__ == "__main__":
    PowerBrokerGame().run()
