#include "LadyLuck.h"

bool LadyLuckCreatureScript::OnGossipHello(Player* player, Creature* creature)
{
    if (!sConfigMgr->GetOption<bool>("LadyLuck.Enable", false))
    {
        return true;
    }

    ClearGossipMenuFor(player);

    if (IsInLottery(player))
    {
        PromptExit(player, creature);
    }
    else
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I would like to enter the lottery room.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Goodbye.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
        SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT, creature->GetGUID());
    }

    return true;
}

void LadyLuckCreatureScript::EnterLottery(Player* player)
{
    player->m_recallMap = player->GetMapId();
    player->m_recallX = player->GetPositionX();
    player->m_recallY = player->GetPositionY();
    player->m_recallZ = player->GetPositionZ();
    player->m_recallO = player->GetOrientation();

    player->TeleportTo(sConfigMgr->GetOption<uint32>("LadyLuck.TeleMap", 449),
        sConfigMgr->GetOption<float>("LadyLuck.TeleX", 0.072697),
        sConfigMgr->GetOption<float>("LadyLuck.TeleY", 9.618815),
        sConfigMgr->GetOption<float>("LadyLuck.TeleZ", -0.227239),
        sConfigMgr->GetOption<float>("LadyLuck.TeleO", 1.584149));
}

void LadyLuckCreatureScript::SayGoodbye(Player* player, Creature* /*creature*/)
{
    CloseGossipMenuFor(player);
}

bool LadyLuckCreatureScript::IsInLottery(Player* player)
{
    return player->GetMapId() == sConfigMgr->GetOption<uint32>("LadyLuck.TeleMap", 449);
}

void LadyLuckCreatureScript::PromptExit(Player* player, Creature* creature)
{
    ClearGossipMenuFor(player);

    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Yes, please.", GOSSIP_SENDER_MAIN, LADYLUCK_EXITLOTTERY);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "No, I would like to open another box.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);

    SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_EXIT, creature->GetGUID());
}

void LadyLuckCreatureScript::ExitLottery(Player* player)
{
    CloseGossipMenuFor(player);
    RestorePlayerLocation(player);
}

void LadyLuckCreatureScript::RestorePlayerLocation(Player* player)
{
    player->TeleportTo(player->m_recallMap, player->m_recallX, player->m_recallY, player->m_recallZ, player->m_recallO);
}

bool LadyLuckCreatureScript::OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
    {
        return false;
    }

    switch (action)
    {
    case LADYLUCK_ENTERLOTTERY:
        EnterLottery(player);
        break;

    case LADYLUCK_EXITLOTTERY:
        ExitLottery(player);
        break;

    case LADYLUCK_GOODBYE:
        SayGoodbye(player, creature);
        break;
    }

    return true;
}


bool LadyLuckGameObjectScript::OnGossipHello(Player* player, GameObject* go)
{
    if (!sConfigMgr->GetOption<bool>("LadyLuck.Enable", false))
    {
        return true;
    }

    ClearGossipMenuFor(player);

    if (CanLoot(player))
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Unlock the box>", GOSSIP_SENDER_MAIN, LOTTERYBOX_OPEN);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Open a different box>", GOSSIP_SENDER_MAIN, LOTTERYBOX_GOODBYE);
        SendGossipMenuFor(player, LOTTERYBOX_GOSSIPTEXT, go->GetGUID());
    }
    else
    {
        SendGossipMenuFor(player, LOTTERYBOX_GOSSIPTEXT_DENY, go->GetGUID());
    }
    
    return true;
}

bool LadyLuckGameObjectScript::CanLoot(Player* player)
{
    return player->HasItemCount(sConfigMgr->GetOption<uint32>("LadyLuck.UnlockItem", 5518), sConfigMgr->GetOption<uint32>("LadyLuck.UnlockItemCount", 1));
}

void LadyLuckGameObjectScript::DeductUnlockItem(Player* player)
{
    player->DestroyItemCount(sConfigMgr->GetOption<uint32>("LadyLuck.UnlockItem", 5518), sConfigMgr->GetOption<uint32>("LadyLuck.UnlockItemCount", 1), true);
}

std::vector<LotteryLoot> GetLootForRoll(Player* player, uint32 roll)
{
    std::vector<LotteryLoot> lootPool;

    for (auto it = lotteryLootPool.begin(); it != lotteryLootPool.end(); ++it)
    {
        if (roll <= it->roll &&
            ((player->getLevel() >= it->levelMin || it->levelMin == 0) &&
                player->getLevel() <= it->levelMax))
        {
            lootPool.push_back(*it);
        }
    }

    return lootPool;
}

void LadyLuckGameObjectScript::OpenLotteryBox(Player* player)
{
    CloseGossipMenuFor(player);

    if (!CanLoot(player))
    {
        return;
    }

    DeductUnlockItem(player);
    RewardLootItem(player);
}

void LadyLuckGameObjectScript::RewardLootItem(Player* player)
{
    uint32 roll = urand(0, 100);
    std::vector<LotteryLoot> lootPool = GetLootForRoll(player, roll);

    LotteryLoot lootItem = {};

    if (lootPool.size() == 0)
    {
        //Failed to meet criteria with roll, re-roll with a roll of 100.
        lootPool = GetLootForRoll(player, 100);
        if (lootPool.size() == 0)
        {
            LOG_WARN("module", "Player '{}' tried to loot an item from the lottery box, but no item matched his criteria!", player->GetName());
            return;
        }
    }

    roll = urand(0, lootPool.size() - 1);
    lootItem = lootPool.at(roll);

    if (lootItem.itemId == 0)
    {
        LOG_WARN("module", "Player '{}' tried to loot an item from the lottery box, but loot item is 0!", player->GetName());
        return;
    }

    if (lootItem.itemId)
    {
        ItemPosCountVec dest;
        if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, lootItem.itemId, lootItem.itemCount) == EQUIP_ERR_OK)
        {
            Item* item = player->StoreNewItem(dest, lootItem.itemId, true);
            player->SendNewItem(item, lootItem.itemCount, true, false);
        }
        else
        {
            player->SendItemRetrievalMail({ {lootItem.itemId, lootItem.itemCount} });
        }
    }
}

bool LadyLuckGameObjectScript::OnGossipSelect(Player* player, GameObject* /*go*/, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
    {
        return false;
    }

    switch (action)
    {
    case LOTTERYBOX_OPEN:
        OpenLotteryBox(player);
        break;

    case LOTTERYBOX_GOODBYE:
        CloseGossipMenuFor(player);
        break;
    }

    return true;
}

void LadyLuckWorldScript::OnAfterConfigLoad(bool reload)
{
    if (reload)
    {
        lotteryLootPool.clear();
    }

    QueryResult result = WorldDatabase.Query("SELECT item_id, item_count, level_min, level_max, roll FROM ladyluck_lottery_loot");

    if (!result)
    {
        LOG_ERROR("module", "Failed to load `ladyluck_lottery_loot table!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        LotteryLoot lotteryLoot;

        lotteryLoot.itemId = fields[0].Get<uint32>();
        lotteryLoot.itemCount = fields[1].Get<uint32>();
        lotteryLoot.levelMin = fields[2].Get<uint32>();
        lotteryLoot.levelMax = fields[3].Get<uint32>();
        lotteryLoot.roll = fields[4].Get<uint32>();

        lotteryLootPool.push_back(lotteryLoot);
    } while (result->NextRow());
}

void AddLadyLuckScripts()
{
    new LadyLuckWorldScript();
    new LadyLuckCreatureScript();
    new LadyLuckGameObjectScript();
}
