code = r'''#include "../include/game.h"
#include <iostream>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>

using namespace std;

ModelLeagueGame::ModelLeagueGame() { rng.seed(time(nullptr)); }
ModelLeagueGame::~ModelLeagueGame() {}

void ModelLeagueGame::init() { ui.init(); initCards(); initAlliances(); loadGame(); }

void ModelLeagueGame::initCards() {
    ModelCard card;
    card.id="gpt4";card.name="GPT-4";card.description="OpenAI最强";card.alliance=ALLIANCE_OPENAI;card.rarity=RARITY_LEGENDARY;
    card.attack=95;card.defense=85;card.speed=80;card.hp=100;card.maxHp=100;allCards.push_back(card);
    card.id="claude";card.name="Claude";card.description="Anthropic";card.alliance=ALLIANCE_ANTHROPIC;card.rarity=RARITY_LEGENDARY;
    card.attack=90;card.defense=95;card.speed=75;card.hp=110;card.maxHp=110;allCards.push_back(card);
    card.id="gemini";card.name="Gemini Pro";card.description="Google";card.alliance=ALLIANCE_GOOGLE;card.rarity=RARITY_EPIC;
    card.attack=88;card.defense=80;card.speed=85;card.hp=95;card.maxHp=95;allCards.push_back(card);
    card.id="llama";card.name="Llama 3";card.description="Meta开源";card.alliance=ALLIANCE_INDEPENDENT;card.rarity=RARITY_RARE;
    card.attack=75;card.defense=70;card.speed=70;card.hp=85;card.maxHp=85;allCards.push_back(card);
    card.id="qwen";card.name="Qwen 2.5";card.description="阿里";card.alliance=ALLIANCE_INDEPENDENT;card.rarity=RARITY_RARE;
    card.attack=78;card.defense=72;card.speed=73;card.hp=88;card.maxHp=88;allCards.push_back(card);
    playerCards.push_back(allCards[3]);playerCards.push_back(allCards[4]);
}

void ModelLeagueGame::initAlliances() {
    Alliance a;
    a.name="OpenAI联盟";a.leader="Sam Altman";a.description="追求AGI";a.level=5;alliances.push_back(a);
    a.name="Anthropic联盟";a.leader="Dario Amodei";a.description="安全对齐";a.level=4;alliances.push_back(a);
    a.name="Google DeepMind";a.leader="Demis Hassabis";a.description="科学突破";a.level=5;alliances.push_back(a);
}

string ModelLeagueGame::getRarityString(Rarity r) {
    switch(r){case RARITY_COMMON:return "[普通]";case RARITY_RARE:return "[稀有]";case RARITY_EPIC:return "[史诗]";case RARITY_LEGENDARY:return "[传说]";default:return "[未知]";}
}

void ModelLeagueGame::printSeparator(int w){for(int i=0;i<w;i++)cout<<"=";cout<<endl;}

void ModelLeagueGame::run() {
    bool running=true;
    while(running){showMainMenu();char c=ui.getInput();
        switch(c){case '1':showCardCollection();break;case '2':startBattle();break;case '3':showAllianceList();break;case '4':addGold(100);addExp(50);ui.clear();cout<<"[测试]+100"<<endl;usleep(500000);break;case 'q':case 'Q':running=false;break;}
    }saveGame();
}

void ModelLeagueGame::showMainMenu() {
    ui.clear();printSeparator();cout<<"模型联盟大作战 v1.0 (Stdin版)"<<endl;printSeparator();showResourcePanel();
    cout<<endl<<"[1]卡牌 [2]战斗 [3]联盟 [4]资源 [Q]退出"<<endl<<"请选择:";ui.refresh();
}

void ModelLeagueGame::showCardCollection() {
    ui.clear();printSeparator();cout<<"===卡牌==="<<endl;printSeparator();
    for(size_t i=0;i<playerCards.size();i++){ModelCard& c=playerCards[i];
        cout<<(i+1)<<"."<<c.name<<" "<<getRarityString(c.rarity)<<"(Lv"<<c.level<<")"<<endl;
        cout<<"ATK:"<<c.attack<<" DEF:"<<c.defense<<" SPD:"<<c.speed<<" HP:"<<c.hp<<"/"<<c.maxHp<<endl;}
    cout<<"B返回"<<endl;ui.refresh();char ch;while(true){ch=ui.getInput();if(ch=='b'||ch=='B')break;}
}

ModelCard ModelLeagueGame::drawRandomCard(){
    if(allCards.empty())return ModelCard();
    uniform_int_distribution<> dist(0,(int)allCards.size()-1);
    ModelCard c=allCards[dist(rng)];c.id=c.id+"_"+to_string(time(nullptr));return c;
}

void ModelLeagueGame::addCard(const ModelCard& card){if(playerCards.size()<MAX_CARDS)playerCards.push_back(card);}

void ModelLeagueGame::startBattle() {
    if(playerCards.empty()){ui.clear();cout<<"无可用卡牌"<<endl;ui.getInput();return;}
    ui.clear();cout<<"选择卡牌:"<<endl;
    for(size_t i=0;i<playerCards.size();i++)cout<<(i+1)<<"."<<playerCards[i].name<<"(HP:"<<playerCards[i].hp<<")"<<endl;
    cout<<"选择:";ui.refresh();int ch=ui.getInput()-'0';
    if(ch<1||ch>(int)playerCards.size())return;
    ModelCard enemy=drawRandomCard();enemy.hp=enemy.maxHp;
    currentBattle.playerCard=playerCards[ch-1];currentBattle.enemyCard=enemy;
    currentBattle.state=BATTLE_PLAYER_TURN;currentBattle.turn=1;currentBattle.battleLog="战斗开始!";
    bool end=false;
    while(!end){showBattleScene();
        if(currentBattle.state==BATTLE_PLAYER_TURN){
            cout<<"[A]攻击 [S]防御 [Q]逃跑:"<<endl;ui.refresh();int a=ui.getInput();
            if(a=='q'||a=='Q'){currentBattle.state=BATTLE_DEFEAT;end=true;}
            else if(a=='a'||a=='A'){
                int dmg=calculateDamage(currentBattle.playerCard,currentBattle.enemyCard);
                currentBattle.enemyCard.hp-=dmg;currentBattle.battleLog="你造成"+to_string(dmg)+"伤害!";
                if(currentBattle.enemyCard.hp<=0){currentBattle.state=BATTLE_VICTORY;end=true;}
                else currentBattle.state=BATTLE_ENEMY_TURN;
            }else if(a=='s'||a=='S'){currentBattle.playerCard.defense+=10;currentBattle.battleLog="防御!";currentBattle.state=BATTLE_ENEMY_TURN;}
        }else if(currentBattle.state==BATTLE_ENEMY_TURN){
            usleep(500000);
            int dmg=calculateDamage(currentBattle.enemyCard,currentBattle.playerCard);
            currentBattle.playerCard.hp-=dmg;currentBattle.battleLog="敌人"+to_string(dmg)+"伤害!";
            if(currentBattle.playerCard.hp<=0){currentBattle.state=BATTLE_DEFEAT;end=true;}
            else{currentBattle.state=BATTLE_PLAYER_TURN;currentBattle.turn++;}
        }else end=true;
    }endBattle(currentBattle.state==BATTLE_VICTORY);
}

int ModelLeagueGame::calculateDamage(const ModelCard& atk,const ModelCard& def){
    int base=max(1,atk.attack-def.defense/2);uniform_int_distribution<> dist(-5,10);return base+dist(rng);
}

void ModelLeagueGame::showBattleScene() {
    ui.clear();printSeparator();cout<<"===战斗==="<<endl<<"回合:"<<currentBattle.turn<<endl;printSeparator();
    cout<<"玩家:"<<currentBattle.playerCard.name<<" Lv"<<currentBattle.playerCard.level<<" HP:"<<currentBattle.playerCard.hp<<"/"<<currentBattle.playerCard.maxHp<<endl;
    cout<<"ATK:"<<currentBattle.playerCard.attack<<" DEF:"<<currentBattle.playerCard.defense<<" SPD:"<<currentBattle.playerCard.speed<<endl;
    cout<<" VS "<<endl;
    cout<<"敌人:"<<currentBattle.enemyCard.name<<" Lv"<<currentBattle.enemyCard.level<<" HP:"<<currentBattle.enemyCard.hp<<"/"<<currentBattle.enemyCard.maxHp<<endl;
    printSeparator();cout<<"日志:"<<currentBattle.battleLog<<endl;printSeparator();
    switch(currentBattle.state){case BATTLE_PLAYER_TURN:cout<<">>你的回合<<"<<endl;break;case BATTLE_ENEMY_TURN:cout<<">>敌人回合<<"<<endl;break;case BATTLE_VICTORY:cout<<">>胜利<<"<<endl;break;case BATTLE_DEFEAT:cout<<">>失败<<"<<endl;break;default:break;}
    ui.refresh();
}

void ModelLeagueGame::endBattle(bool win){
    ui.clear();
    if(win){cout<<endl<<"恭喜!"<<endl<<endl;resources.winCount++;addGold(50);addExp(30);}
    else{cout<<endl<<"失败"<<endl<<endl;resources.loseCount++;addExp(10);}
    cout<<"任意键返回"<<endl;ui.refresh();ui.getInput();
}

void ModelLeagueGame::showAllianceList(){
    ui.clear();printSeparator();cout<<"===联盟==="<<endl;printSeparator();
    for(size_t i=0;i<alliances.size();i++){Alliance& a=alliances[i];
        cout<<(i+1)<<"."<<a.name<<endl;cout<<"领袖:"<<a.leader<<" 等级:"<<a.level<<endl;cout<<a.description<<endl<<endl;}
    cout<<"B返回"<<endl;ui.refresh();char ch;while(true){ch=ui.getInput();if(ch=='b'||ch=='B')break;}
}

void ModelLeagueGame::addGold(int a){resources.gold+=a;}
void ModelLeagueGame::addExp(int a){resources.exp+=a;checkLevelUp();}

void ModelLeagueGame::checkLevelUp(){
    int need=resources.level*100;
    while(resources.exp>=need){resources.level++;resources.exp-=need;need=resources.level*100;resources.gold+=200;}
}

void ModelLeagueGame::showResourcePanel(){
    cout<<"金币:"<<resources.gold<<" 钻石:"<<resources.diamond<<endl;
    cout<<"等级:"<<resources.level<<" EXP:"<<resources.exp<<endl;
    cout<<"战绩:"<<resources.winCount<<"胜 "<<resources.loseCount<<"负"<<endl;
}

void ModelLeagueGame::saveGame(){}
void ModelLeagueGame::loadGame(){}

int main(){ModelLeagueGame g;g.init();g.run();return 0;}
'''
with open('/Users/chongjieran/.openclaw/workspace-developer/games/model-league/src/game.cpp', 'w') as f:
    f.write(code)
print('done')
