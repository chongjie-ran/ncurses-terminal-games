#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <random>
#include <iostream>

// 游戏常量
const int MAX_CARDS = 20;
const int MAX_ALLIANCE_MEMBERS = 5;

// 联盟类型
enum AllianceType {
    ALLIANCE_OPENAI = 0,    // OpenAI 联盟
    ALLIANCE_ANTHROPIC,     // Anthropic 联盟
    ALLIANCE_GOOGLE,        // Google 联盟
    ALLIANCE_INDEPENDENT    // 独立模型
};

// 卡牌稀有度
enum Rarity {
    RARITY_COMMON = 1,
    RARITY_RARE = 2,
    RARITY_EPIC = 3,
    RARITY_LEGENDARY = 4
};

// AI模型卡牌
struct ModelCard {
    std::string id;
    std::string name;           // 模型名称
    std::string description;    // 描述
    AllianceType alliance;      // 所属联盟
    Rarity rarity;              // 稀有度
    int attack;                 // 攻击力
    int defense;                // 防御力
    int speed;                  // 速度
    int hp;                     // 生命值
    int maxHp;                  // 最大生命值
    int level;                  // 等级
    int exp;                    // 经验值
    bool isLocked;              // 是否锁定
    
    ModelCard() : attack(0), defense(0), speed(0), hp(0), maxHp(0), 
                  level(1), exp(0), isLocked(false) {}
};

// 玩家资源
struct PlayerResources {
    int gold;       // 金币
    int diamond;    // 钻石
    int exp;        // 总经验
    int level;      // 玩家等级
    int winCount;   // 胜利次数
    int loseCount;  // 失败次数
    
    PlayerResources() : gold(1000), diamond(100), exp(0), level(1), 
                        winCount(0), loseCount(0) {}
};

// 联盟信息
struct Alliance {
    std::string name;
    std::string leader;
    std::string description;
    std::vector<std::string> memberIds;
    int totalPower;     // 总战力
    int level;          // 联盟等级
    
    Alliance() : totalPower(0), level(1) {}
};

// 战斗状态
enum BattleState {
    BATTLE_START,
    BATTLE_PLAYER_TURN,
    BATTLE_ENEMY_TURN,
    BATTLE_VICTORY,
    BATTLE_DEFEAT
};

// 战斗场景
struct BattleScene {
    ModelCard playerCard;
    ModelCard enemyCard;
    BattleState state;
    int turn;
    std::string battleLog;
    
    BattleScene() : state(BATTLE_START), turn(1) {}
};

// ============ StdinUI: 纯stdio替代NCurses ============
class StdinUI {
public:
    StdinUI() {}
    ~StdinUI() {}
    
    void init() {
        // 无需初始化
    }
    
    void clear() {
        std::cout << "\033[2J\033[H";
    }
    
    void print(const std::string& text) {
        std::cout << text << std::endl;
    }
    
    void printAt(int y, int x, const std::string& text) {
        // 简化版本，直接输出
        std::cout << text << std::endl;
    }
    
    char getInput() {
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) return '\n';
        return line[0];
    }
    
    void refresh() {
        std::cout << std::flush;
    }
    
    void drawBox(int y, int x, int h, int w) {
        for (int i = 0; i < w; i++) {
            std::cout << "-";
        }
        std::cout << std::endl;
    }
};

// 游戏主类
class ModelLeagueGame {
private:
    std::vector<ModelCard> playerCards;   // 玩家卡牌
    std::vector<ModelCard> allCards;      // 所有卡牌
    std::vector<Alliance> alliances;      // 联盟列表
    PlayerResources resources;            // 玩家资源
    BattleScene currentBattle;            // 当前战斗
    std::mt19937 rng;                     // 随机数生成器
    StdinUI ui;                           // UI接口
    
public:
    ModelLeagueGame();
    ~ModelLeagueGame();
    
    // 初始化
    void init();
    void initCards();
    void initAlliances();
    
    // 游戏循环
    void run();
    void showMainMenu();
    void handleMenu(int choice);
    
    // 卡牌系统
    void showCardCollection();
    void addCard(const ModelCard& card);
    ModelCard* getCard(const std::string& id);
    void upgradeCard(const std::string& id);
    ModelCard drawRandomCard();
    
    // 战斗系统
    void startBattle();
    void updateBattle();
    void showBattleScene();
    int calculateDamage(const ModelCard& attacker, const ModelCard& defender);
    void endBattle(bool playerWin);
    
    // 联盟系统
    void showAllianceList();
    void joinAlliance(int allianceIdx);
    void showAllianceDetail(int idx);
    
    // 资源管理
    void addGold(int amount);
    void addExp(int amount);
    void checkLevelUp();
    void showResourcePanel();
    
    // 存档系统
    void saveGame();
    void loadGame();
    
    // 辅助方法
    std::string getRarityString(Rarity r);
    void printSeparator(int width = 50);
};

#endif // GAME_H
