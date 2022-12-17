#ifndef MODULE_LADYLUCK_H
#define MODULE_LADYLUCK_H

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include <AI/ScriptedAI/ScriptedGossip.h>
#include <vector>

struct TeleportInfo
{
    uint32 Map;
    float X;
    float Y;
    float Z;
    float O;
};

struct PlayerLotteryInfo
{
    TeleportInfo previousLocation;
    ObjectGuid playerGuid;
    bool canLoot;
};

bool ladyLuckEnabled;

uint32 ladyLuckCurrency;
uint32 ladyLuckCurrencyCount;

TeleportInfo ladyLuckTele;

std::vector<PlayerLotteryInfo> playerLotteryInfo;

bool IsInLottery(Player* /*player*/);
void UpdateCanLoot(Player* /*player*/, bool /*state*/);
bool CanLoot(Player* /*player*/);

class LadyLuckCreatureScript : public CreatureScript
{
public:
    LadyLuckCreatureScript() : CreatureScript("LadyLuckCreatureScript") { }

private:
    enum LadyLuckGossips
    {
        LADYLUCK_GOSSIPTEXT = 444111,
        LADYLUCK_GOSSIPTEXT_SUCCESS = 444112,
        LADYLUCK_GOSSIPTEXT_FAIL = 444113,
        LADYLUCK_GOSSIPTEXT_EXIT = 444114,

        LADYLUCK_ENTERLOTTERY = 1000,
        LADYLUCK_ENTERLOTTERY_SUCCESS = 1001,
        LADYLUCK_ENTERLOTTERY_RETRY = 1002,

        LADYLUCK_EXITLOTTERY = 1101,

        LADYLUCK_GOODBYE = 1500
    };

    bool OnGossipHello(Player* /*player*/, Creature* /*creature*/) override;
    bool OnGossipSelect(Player* /*player*/, Creature* /*creature*/, uint32 /*sender*/, uint32 /*action*/) override;
    void ValidateCurrency(Player* /*player*/, Creature* /*creature*/);
    void DeductCurrency(Player* /*player*/, uint32 /*count*/);
    bool CanEnterLottery(Player* /*player*/);
    void EnterLottery(Player* /*player*/, bool /*retry*/);
    void ExitLottery(Player* /*player*/);
    void RestorePlayer(Player* /*player*/, TeleportInfo* /*teleInfo*/);
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
        LOTTERYBOX_OPEN = 1000,
        LOTTERYBOX_DENY = 1001,
        LOTTERYBOX_GOODBYE = 1500
    };

    bool OnGossipHello(Player* /*player*/, GameObject* /*go*/) override;
    bool OnGossipSelect(Player* /*player*/, GameObject* /*go*/, uint32 /*sender*/, uint32 /*action*/) override;
};

class LadyLuckWorldScript : public WorldScript
{
public:
    LadyLuckWorldScript() : WorldScript("LadyLuckWorldScript") { }
private:
    void OnAfterConfigLoad(bool /*reload*/) override;
};
#endif //MODULE_LADYLUCK_H
