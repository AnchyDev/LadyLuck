/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include <AI/ScriptedAI/ScriptedGossip.h>

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
        LADYLUCK_ENTERLOTTERY = 1000,
        LADYLUCK_ENTERLOTTERY_SUCCESS = 1001,
        LADYLUCK_GOODBYE = 1500
    };

    enum LadyLuckIds
    {
        LADYLUCK_CURRENCY = 37711 //ItemId
    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_DOT, "I would like to enter the lottery.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY);
       // AddGossipItemFor(player, GOSSIP_ICON_DOT, "Goodbye.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
        SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT, creature->GetGUID());
        return true;
    }

    void EnterLottery(Player* player)
    {
        CloseGossipMenuFor(player);
        ChatHandler(player->GetSession()).SendSysMessage("EnterLottery");
    }

    void SayGoodbye(Player* player, Creature* creature)
    {
        CloseGossipMenuFor(player);
        ChatHandler(player->GetSession()).SendSysMessage("SayGoodbye");
    }

    bool CanEnterLottery(Player* player)
    {
        uint32 currencyAmount = player->GetItemCount(LADYLUCK_CURRENCY, false);

        if (currencyAmount < 1)
        {
            return false;
        }

        return true;
    }

    void ValidateCurrency(Player* player, Creature* creature)
    {
        if (CanEnterLottery(player))
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "Yes, I'm sure.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY_SUCCESS);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "No, I would rather not.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
            SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_SUCCESS, creature->GetGUID());
            ChatHandler(player->GetSession()).SendSysMessage("EnterLotteryS");
        }
        else
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "I see, goodbye.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
            SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_FAIL, creature->GetGUID());
            ChatHandler(player->GetSession()).SendSysMessage("EnterLotteryF");
        }
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (sender != GOSSIP_SENDER_MAIN)
        {
            return false;
        }

        switch (action)
        {
        case LADYLUCK_ENTERLOTTERY:
            ValidateCurrency(player, creature);
            break;

        case LADYLUCK_ENTERLOTTERY_SUCCESS:
            EnterLottery(player);
            break;

        case LADYLUCK_GOODBYE:
            SayGoodbye(player, creature);
            break;
        }

        return true;
    }
};

// Add all scripts in one
void AddLadyLuckScripts()
{
    new LadyLuckCreatureScript();
}
