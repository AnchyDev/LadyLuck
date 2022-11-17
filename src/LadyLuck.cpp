/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

class LadyLuckCreatureScript : public CreatureScript
{
public:
    LadyLuckCreatureScript() : CreatureScript("LadyLuckCreatureScript") { }

private:
    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("Hello {}", player->GetName()));
        return false;
    }
};

// Add all scripts in one
void AddLadyLuckScripts()
{
    new LadyLuckCreatureScript();
}
