# 模型联盟大作战 (Model League) v1.3

## 游戏介绍
收集AI模型卡牌，组建联盟，与对手进行回合制对战！

## v1.3丰富化功能 ✅
- 8种AI模型卡牌（Claude/GPT-5/Gemini/DeepSeek等）
- 3种卡牌类型：THINK/EFF/BALANCE
- 3个联盟阵营：SHANXIAN(速度)/JUDNAO(智慧)/HUNDUN(混沌)
- 回合制战斗引擎（含联盟加成计算）
- 金币和经验资源系统

## 运行方法
```bash
cd ~/.openclaw/workspace-developer/games/model-league
make && ./model-league
```

## 阵营加成
| 阵营 | 加成 | 说明 |
|------|------|------|
| SHANXIAN | ATK+10% | 速度至上 |
| JUDNAO | DEF+10% | 智慧为王 |
| HUNDUN | SPD+15% | 混沌为王 |

## 卡牌一览
| 卡牌 | 类型 | 阵营 | ATK | DEF | SPD | 稀有度 |
|------|------|------|-----|-----|-----|--------|
| Claude | THINK | JUDNAO | 85 | 70 | 60 | *** |
| GPT-5 | THINK | JUDNAO | 90 | 65 | 55 | *** |
| Gemini | BALANCE | SHANXIAN | 75 | 75 | 80 | ** |
| DeepSeek | EFFICIENT | SHANXIAN | 70 | 60 | 95 | ** |
| Llama | BALANCE | HUNDUN | 72 | 72 | 72 | * |
| Mistral | EFFICIENT | SHANXIAN | 68 | 58 | 90 | ** |
| Qwen | THINK | JUDNAO | 88 | 68 | 58 | *** |
| Grok | EFFICIENT | HUNDUN | 80 | 50 | 88 | *** |

## 操作菜单
| 按键 | 功能 |
|------|------|
| 1 | 商店购买卡牌 |
| 2 | 查看收藏 |
| 3 | 组卡组 |
| 4 | 战斗 |
| 5 | 联盟大厅 |
| 6 | 挖矿 |
| q | 退出 |

v1.3 | 悟通 | 2026-04-09
