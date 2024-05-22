#ifndef MODULE_LADYLUCK_H
#define MODULE_LADYLUCK_H

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include <AI/ScriptedAI/ScriptedGossip.h>
#include <vector>

struct LotteryLoot
{
    uint32 itemId;
    uint32 itemCount;
    uint32 levelMin;
    uint32 levelMax;
    uint32 roll;
};

std::vector<LotteryLoot> lotteryLootPool;

bool CanLoot(Player* /*player*/);
std::vector<LotteryLoot> GetLootForRoll(uint32 /*roll*/);

class LadyLuckCreatureScript : public CreatureScript
{
public:
    LadyLuckCreatureScript() : CreatureScript("LadyLuckCreatureScript") { }

private:
    enum LadyLuckGossips
    {
        LADYLUCK_GOSSIPTEXT = 444111,
        LADYLUCK_GOSSIPTEXT_SUCCESS = 444112,
        LADYLUCK_GOSSIPTEXT_FAIL_CURRENCY = 444113,
        LADYLUCK_GOSSIPTEXT_FAIL_MONEY = 444114,
        LADYLUCK_GOSSIPTEXT_EXIT = 444115,
        LADYLUCK_GOSSIPTEXT_SELECT_COST = 444116,

        LADYLUCK_ENTERLOTTERY = 1000,
        LADYLUCK_ENTERLOTTERY_SUCCESS = 1001,
        LADYLUCK_ENTERLOTTERY_RETRY = 1002,
        LADYLUCK_ENTERLOTTERY_GOLD = 1003,
        LADYLUCK_ENTERLOTTERY_CURRENCY = 1004,

        LADYLUCK_EXITLOTTERY = 1101,

        LADYLUCK_GOODBYE = 1500
    };

    bool OnGossipHello(Player* /*player*/, Creature* /*creature*/) override;
    bool OnGossipSelect(Player* /*player*/, Creature* /*creature*/, uint32 /*sender*/, uint32 /*action*/) override;

    void EnterLottery(Player* /*player*/);
    void ExitLottery(Player* /*player*/);
    bool IsInLottery(Player* /*player*/);

    void RestorePlayerLocation(Player* /*player*/);
    void PromptExit(Player* /*player*/, Creature* /*creature*/);
    void SayGoodbye(Player* /*player*/, Creature* /*creature*/);
};

class LadyLuckGameObjectScript : public GameObjectScript
{
public:
    LadyLuckGameObjectScript() : GameObjectScript("LadyLuckGameObjectScript") { }
private:
    enum LotteryBoxGossips
    {
        LOTTERYBOX_GOSSIPTEXT = 444211,
        LOTTERYBOX_GOSSIPTEXT_DENY = 444212,
        LOTTERYBOX_OPEN = 1000,
        LOTTERYBOX_GOODBYE = 1500
    };

    bool OnGossipHello(Player* /*player*/, GameObject* /*go*/) override;
    bool OnGossipSelect(Player* /*player*/, GameObject* /*go*/, uint32 /*sender*/, uint32 /*action*/) override;

    void OpenLotteryBox(Player* /*player*/);
    bool CanLoot(Player* /*player*/);
    void DeductUnlockItem(Player* /*player*/);
    void RewardLootItem(Player* /*player*/);
};

class LadyLuckWorldScript : public WorldScript
{
public:
    LadyLuckWorldScript() : WorldScript("LadyLuckWorldScript") { }
private:
    void OnAfterConfigLoad(bool /*reload*/) override;
};
#endif //MODULE_LADYLUCK_H
