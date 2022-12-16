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

    bool OnGossipHello(Player* player, Creature* creature)
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I would like to enter the lottery.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Goodbye.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
        SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT, creature->GetGUID());

        return true;
    }

    void EnterLottery(Player* player)
    {
        CloseGossipMenuFor(player);
        DeductCurrency(player, 1);
        player->TeleportTo(0, -8653, -508, 145.406999, 5);
    }

    void SayGoodbye(Player* player, Creature* /*creature*/)
    {
        CloseGossipMenuFor(player);
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

    void DeductCurrency(Player* player, uint32 count)
    {
        Item* currency = player->GetItemByEntry(LADYLUCK_CURRENCY);
        player->DestroyItemCount(currency, count, true);
    }

    void ValidateCurrency(Player* player, Creature* creature)
    {
        if (CanEnterLottery(player))
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Yes, I'm sure.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY_SUCCESS);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "No, I would rather not.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
            SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_SUCCESS, creature->GetGUID());
        }
        else
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I see, goodbye.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
            SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_FAIL, creature->GetGUID());
        }
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
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

class LadyLuckGameObjectScript : public GameObjectScript
{
public:
    LadyLuckGameObjectScript() : GameObjectScript("LadyLuckGameObjectScript") { }
private:
    enum LotteryBoxGossips
    {
        LOTTERYBOX_GOSSIPTEXT = 444211,
        LOTTERYBOX_OPEN = 1000,
        LOTTERYBOX_GOODBYE = 1500
    };

    bool OnGossipHello(Player* player, GameObject* go)
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Open", GOSSIP_SENDER_MAIN, LOTTERYBOX_OPEN);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I would like to open another box.", GOSSIP_SENDER_MAIN, LOTTERYBOX_GOODBYE);
        SendGossipMenuFor(player, LOTTERYBOX_GOSSIPTEXT, go->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 sender, uint32 action)
    {
        if (sender != GOSSIP_SENDER_MAIN)
        {
            return false;
        }

        switch (action)
        {
        case LOTTERYBOX_OPEN:
            ChatHandler(player->GetSession()).SendSysMessage("OPEN");
            break;

        case LOTTERYBOX_GOODBYE:
            ChatHandler(player->GetSession()).SendSysMessage("GOODBYE");
            break;
        }

        return true;
    }
};

// Add all scripts in one
void AddLadyLuckScripts()
{
    new LadyLuckCreatureScript();
    new LadyLuckGameObjectScript();
}
