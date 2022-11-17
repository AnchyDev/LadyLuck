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
    enum Gossips
    {
        LADYLUCK_GOSSIPTEXT = 0,
        LADYLUCK_ENTERLOTTERY = 1000
    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "I would like to enter the lottery.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY);
        SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT, creature->GetGUID());
        return true;
    }
};

// Add all scripts in one
void AddLadyLuckScripts()
{
    new LadyLuckCreatureScript();
}
