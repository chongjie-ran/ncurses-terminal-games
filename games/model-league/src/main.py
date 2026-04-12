#!/usr/bin/env python3
"""模型联盟大作战 MVP"""
import random, os
from dataclasses import dataclass, field
from typing import List, Optional

INITIAL_GOLD = 100
ATTR = {"智力":{"创造力":1.5,"速度":0.8,"稳定性":0.5},
        "创造力":{"速度":1.5,"智力":0.8,"稳定性":0.5},
        "速度":{"智力":1.5,"创造力":0.8,"稳定性":0.5},
        "稳定性":{"智力":1.2,"创造力":1.2,"速度":1.2}}

@dataclass
class Card:
    name: str; mtype: str; primary: str; power: int; cost: int; skill: str; rarity: int
    def dmg(self, d): 
        return int(self.power * ATTR.get(self.primary,{}).get(d.primary, 1))

@dataclass
class Player:
    gold: int; deck: List[Card] = field(default_factory=list); hand: List[Card] = field(default_factory=list)
    def buy(self, c): 
        if self.gold >= c.cost and len(self.deck) < 30: self.gold -= c.cost; self.deck.append(c); return True
        return False
    def draw(self, n=5):
        for _ in range(n):
            if self.deck and len(self.hand) < 10: self.hand.append(self.deck.pop(random.randint(0,len(self.deck)-1)))

def get_cards():
    return [
        Card("Claude-3.5","Claude","智力",85,8,"思维链",3),
        Card("Claude-3","Claude","智力",70,6,"分析",2),
        Card("Claude-2","Claude","稳定性",50,4,"可靠",1),
        Card("GPT-4o","GPT","创造力",90,9,"生成",3),
        Card("GPT-4","GPT","创造力",75,7,"创意",2),
        Card("GPT-3.5","GPT","速度",55,5,"快速",1),
        Card("Gemini-2","Gemini","速度",80,7,"多模态",3),
        Card("Gemini-1.5","Gemini","速度",65,5,"闪电",2),
        Card("Gemini-1","Gemini","速度",45,3,"敏捷",1),
        Card("Llama-3.1","Llama","稳定性",70,6,"开源",2),
        Card("Llama-3","Llama","稳定性",55,4,"自由",1),
        Card("Llama-2","Llama","创造力",40,3,"社区",1),
    ]

class Game:
    def __init__(self):
        self.p = Player(gold=INITIAL_GOLD)
        self.round = 1; self.stars = 0
        self.phase = "shop"; self.shop = get_cards()[:5]
        self.ehp = 100
    
    def run(self):
        print("="*50); print("模型联盟大作战 MVP"); print("="*50)
        print("收集AI模型卡牌,挑战各路高手!")
        input("回车开始...")
        while True:
            os.system('cls' if os.name=='nt' else 'clear')
            print(f"第{self.round}轮 金币:{self.p.gold} 星星:{self.stars}")
            if self.phase == "shop":
                print("【商店】")
                for i,c in enumerate(self.shop): print(f"  [{i}]{c.name} 战力:{c.power} 费:{c.cost}")
                print("[r]刷新 [b]开战 [q]退出")
                cmd = input("> ").strip()
                if cmd == 'q': break
                elif cmd == 'r' and self.p.gold >= 10:
                    self.p.gold -= 10; self.shop = random.sample(get_cards(),5)
                elif cmd.isdigit():
                    i = int(cmd)
                    if 0<=i<5 and self.p.buy(self.shop[i]): print("购买成功!")
                    input("回车继续...")
                elif cmd == 'b':
                    self.phase = "battle"; self.p.draw(5)
            else:
                print(f"【战斗】我的HP:100 敌人HP:{self.ehp}")
                for i,c in enumerate(self.p.hand): print(f"  [{i}]{c.name}战力:{c.power}")
                cmd = input("> ").strip()
                if cmd.isdigit():
                    i = int(cmd)
                    if 0<=i<len(self.p.hand):
                        pc = self.p.hand.pop(i); ec = random.choice(get_cards())
                        pd,ed = pc.dmg(ec), ec.dmg(pc)
                        self.ehp = max(0, self.ehp - pd)
                        print(f"我:{pc.name}造成{pd} 敌:{ec.name}造成{ed}")
                        if self.ehp <= 0:
                            print("🎉胜利!+30金币+1星"); self.p.gold+=30; self.stars+=1; self.ehp=100; self.phase="shop"; self.round+=1
                        input("回车继续...")
        print(f"游戏结束!星星:{self.stars} 金币:{self.p.gold}")

if __name__ == "__main__": Game().run()
