#!/usr/bin/env python3
"""
模型联盟大作战 - MVP
Model League Battle - MVP

卡牌策略游戏：不同AI模型组成联盟对战
"""

import random
import os

# ============ 常量 ============
INITIAL_HP = 100
CARD_DECK_SIZE = 20

# ============ 模型卡牌定义 ============
MODEL_CARDS = [
    {"name": "Claude", "cost": 8, "attack": 25, "defense": 15, "skill": "思维链", "type": "balanced"},
    {"name": "GPT-4", "cost": 7, "attack": 28, "defense": 12, "skill": "创意生成", "type": "attack"},
    {"name": "Gemini", "cost": 6, "attack": 20, "defense": 20, "skill": "多模态", "type": "defense"},
    {"name": "Llama", "cost": 4, "attack": 15, "defense": 15, "skill": "开源精神", "type": "balanced"},
    {"name": "Mistral", "cost": 5, "attack": 18, "defense": 17, "skill": "高效推理", "type": "balanced"},
    {"name": "Grok", "cost": 9, "attack": 35, "defense": 10, "skill": "幽默感", "type": "attack"},
    {"name": "DeepSeek", "cost": 5, "attack": 17, "defense": 18, "skill": "性价比", "type": "defense"},
    {"name": "Qwen", "cost": 6, "attack": 19, "defense": 16, "skill": "中文优化", "type": "balanced"},
]

# ============ 技能系统 ============
SKILLS = {
    "思维链": lambda p, o: (p, max(0, o - 5)),  # 反射5点伤害
    "创意生成": lambda p, o: (p + 10, o),         # 回复10HP
    "多模态": lambda p, o: (p + 5, o - 5),      # 回复5HP并造成5点伤害
    "开源精神": lambda p, o: (p, o - 10),         # 造成10点穿透伤害
    "高效推理": lambda p, o: (p, max(0, o - 8)), # 造成8点伤害
    "幽默感": lambda p, o: (p + 15, o - 15),      # 造成15点伤害但自己也损10HP
    "性价比": lambda p, o: (p, max(0, o - 12)),  # 造成12点伤害
    "中文优化": lambda p, o: (p + 8, o - 3),     # 回复8HP并造成3点伤害
}

# ============ 游戏状态 ============
class GameState:
    def __init__(self):
        self.turn = 1
        self.player_hp = INITIAL_HP
        self.enemy_hp = INITIAL_HP
        self.player_energy = 3
        self.enemy_energy = 3
        self.player_deck = self._build_deck()
        self.enemy_deck = self._build_deck()
        self.player_hand = []
        self.enemy_hand = []
        self.player_field = []
        self.enemy_field = []
        self._draw_initial_cards()
    
    def _build_deck(self):
        """构建卡组"""
        deck = []
        for card in MODEL_CARDS:
            deck.extend([card.copy() for _ in range(2)])  # 每张卡2张
        random.shuffle(deck)
        return deck[:CARD_DECK_SIZE]
    
    def _draw_initial_cards(self):
        """初始抽牌"""
        for _ in range(5):
            if self.player_deck:
                self.player_hand.append(self.player_deck.pop())
            if self.enemy_deck:
                self.enemy_hand.append(self.enemy_deck.pop())

# ============ 战斗系统 ============
def use_skill(skill_name, player_hp, enemy_hp):
    """使用技能"""
    if skill_name in SKILLS:
        return SKILLS[skill_name](player_hp, enemy_hp)
    return player_hp, enemy_hp

def battle_card(attacker, defender):
    """卡牌对战"""
    damage = max(0, attacker["attack"] - defender.get("defense", 0))
    return damage

def draw_card(state, is_player=True):
    """抽牌"""
    deck = state.player_deck if is_player else state.enemy_deck
    hand = state.player_hand if is_player else state.enemy_hand
    if deck and len(hand) < 10:
        hand.append(deck.pop())
        return True
    return False

# ============ AI对战 ============
def enemy_turn(state):
    """敌方AI回合"""
    # 简单AI：优先使用能量打最高攻击力的牌
    playable = [c for c in state.enemy_hand if c["cost"] <= state.enemy_energy]
    if playable:
        # 选择攻击力最高的牌
        best = max(playable, key=lambda x: x["attack"])
        state.enemy_hand.remove(best)
        state.enemy_field.append(best)
        state.enemy_energy -= best["cost"]
        return f"敌方出牌: {best['name']} (ATK:{best['attack']})"
    return "敌方观望"

# ============ UI显示 ============
def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear')

def print_banner():
    print("=" * 50)
    print("🤖 模型联盟大作战 - MVP 🤖")
    print("=" * 50)

def print_state(state):
    """显示游戏状态"""
    print(f"\n【第 {state.turn} 回合】")
    print(f"你的HP: {state.player_hp} | 能量: {state.player_energy}/10")
    print(f"敌方HP: {state.enemy_hp} | 能量: {state.enemy_energy}/10")
    print()
    print("--- 你的手牌 ---")
    for i, card in enumerate(state.player_hand):
        print(f"  [{i}] {card['name']}: ATK={card['attack']} DEF={card['defense']} COST={card['cost']} [{card['type']}]")

def print_help():
    print("\n--- 操作 ---")
    print("  [0-9] 出牌")
    print("  [d]   抽牌")
    print("  [q]   退出")

# ============ 主循环 ============
def main():
    print_banner()
    print("\n游戏说明:")
    print("- 每回合你有3点能量")
    print("- 打出卡牌消耗能量")
    print("- 卡牌自动攻击敌方HP")
    print("- 先把对方HP打到0获胜!")
    input("\n按回车开始...")
    
    state = GameState()
    
    while state.player_hp > 0 and state.enemy_hp > 0:
        clear_screen()
        print_banner()
        print_state(state)
        print_help()
        
        cmd = input("\n> ").strip().lower()
        
        if cmd == 'q':
            print("游戏退出")
            break
        elif cmd == 'd':
            if draw_card(state, True):
                print("抽了一张牌!")
            else:
                print("无法抽牌!")
        elif cmd.isdigit():
            idx = int(cmd)
            if 0 <= idx < len(state.player_hand):
                card = state.player_hand[idx]
                if card["cost"] <= state.player_energy:
                    state.player_hand.pop(idx)
                    state.player_field.append(card)
                    state.player_energy -= card["cost"]
                    
                    # 造成伤害
                    damage = battle_card(card, {})
                    state.enemy_hp = max(0, state.enemy_hp - damage)
                    print(f"你出牌: {card['name']} 造成 {damage} 点伤害!")
                    
                    # 触发技能
                    p_hp, e_hp = use_skill(card["skill"], state.player_hp, state.enemy_hp)
                    if p_hp != state.player_hp or e_hp != state.enemy_hp:
                        state.player_hp = p_hp
                        state.enemy_hp = max(0, e_hp)
                        print(f"技能'{card['skill']}'触发!")
                else:
                    print("能量不足!")
            else:
                print("无效选择!")
        
        # 敌方回合
        if state.enemy_hp > 0:
            result = enemy_turn(state)
            if "出牌" in result:
                card = state.enemy_field[-1]
                damage = battle_card(card, {})
                state.player_hp = max(0, state.player_hp - damage)
            print(result)
        
        # 回合结束
        state.turn += 1
        state.player_energy = min(10, state.player_energy + 3)
        state.enemy_energy = min(10, state.enemy_energy + 3)
        
        # 抽牌
        draw_card(state, True)
        draw_card(state, False)
        
        input("\n按回车继续...")
    
    # 游戏结束
    clear_screen()
    print_banner()
    if state.player_hp > 0:
        print("\n🎉 恭喜获胜! 🎉")
    else:
        print("\n💀 你输了... 💀")
    print(f"最终回合: {state.turn}")

if __name__ == "__main__":
    main()
