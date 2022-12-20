#include "LadyLuck.h"

bool LadyLuckCreatureScript::OnGossipHello(Player* player, Creature* creature)
{
    if (!ladyLuckEnabled)
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
    if (!IsInLottery(player))
    {
        PlayerLotteryInfo playerInfo;

        playerInfo.playerGuid = player->GetGUID();

        playerInfo.previousLocation.Map = player->GetMapId();
        playerInfo.previousLocation.X = player->GetPositionX();
        playerInfo.previousLocation.Y = player->GetPositionY();
        playerInfo.previousLocation.Z = player->GetPositionZ();
        playerInfo.previousLocation.O = player->GetOrientation();

        playerInfo.canLoot = true;
        playerInfo.inLottery = true;

        playerLotteryInfo.push_back(playerInfo);

        player->TeleportTo(ladyLuckTele.Map, ladyLuckTele.X, ladyLuckTele.Y, ladyLuckTele.Z, ladyLuckTele.O);
    }
    else
    {
        UpdateCanLoot(player, true);
    }
}

void LadyLuckCreatureScript::SayGoodbye(Player* player, Creature* /*creature*/)
{
    CloseGossipMenuFor(player);
}

void LadyLuckCreatureScript::DeductCurrency(Player* player, uint32 count)
{
    Item* currency = player->GetItemByEntry(ladyLuckCurrency);
    player->DestroyItemCount(currency, count, true);
}

void LadyLuckCreatureScript::ValidateCurrency(Player* player, Creature* creature)
{
    uint32 currencyAmount = player->GetItemCount(ladyLuckCurrency, false);

    if (currencyAmount >= ladyLuckCurrencyCount)
    {
        CloseGossipMenuFor(player);
        DeductCurrency(player, ladyLuckCurrencyCount);
        EnterLottery(player);
    }
    else
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I see, goodbye.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
        SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_FAIL_CURRENCY, creature->GetGUID());
    }
}

void LadyLuckCreatureScript::ValidateMoney(Player* player, Creature* creature)
{
    uint32 playerMoney = player->GetMoney();

    if (playerMoney >= ladyLuckMoney)
    {
        CloseGossipMenuFor(player);
        DeductMoney(player, ladyLuckMoney);
        EnterLottery(player);
    }
    else
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I see, goodbye.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
        SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_FAIL_MONEY, creature->GetGUID());
    }
}

void LadyLuckCreatureScript::DeductMoney(Player* player, uint32 amount)
{
    player->ModifyMoney(-amount, true);
}

bool IsInLottery(Player* player)
{
    for (auto it = playerLotteryInfo.begin(); it != playerLotteryInfo.end(); ++it)
    {
        if (it->playerGuid == player->GetGUID() && it->inLottery)
        {
            return true;
        }
    }

    return false;
}

void UpdateCanLoot(Player* player, bool state)
{
    for (auto it = playerLotteryInfo.begin(); it != playerLotteryInfo.end(); ++it)
    {
        if (it->playerGuid == player->GetGUID())
        {
            it->canLoot = state;
        }
    }
}

bool CanLoot(Player* player)
{
    for (auto it = playerLotteryInfo.begin(); it != playerLotteryInfo.end(); ++it)
    {
        if (it->playerGuid == player->GetGUID())
        {
            return it->canLoot;
        }
    }

    return false;
}

void LadyLuckCreatureScript::PromptExit(Player* player, Creature* creature)
{
    ClearGossipMenuFor(player);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Yes, please.", GOSSIP_SENDER_MAIN, LADYLUCK_EXITLOTTERY);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "No, I would like to open another box.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY);
    SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_EXIT, creature->GetGUID());
}

void LadyLuckCreatureScript::ExitLottery(Player* player)
{
    CloseGossipMenuFor(player);

    for (auto it = playerLotteryInfo.begin(); it != playerLotteryInfo.end(); ++it)
    {
        if (it->playerGuid == player->GetGUID())
        {
            RestorePlayer(player, &it->previousLocation);
            it->inLottery = false;
            break;
        }
    }
}

void LadyLuckCreatureScript::RestorePlayer(Player* player, TeleportInfo* teleInfo)
{
    player->TeleportTo(teleInfo->Map, teleInfo->X, teleInfo->Y, teleInfo->Z, teleInfo->O);
}

void LadyLuckCreatureScript::DisplayLotteryOptions(Player* player, Creature* creature)
{
    ClearGossipMenuFor(player);

    if (ladyLuckMoney > 0)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I would like to use gold.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY_GOLD, "Are you sure you would like to use gold?", ladyLuckMoney, false);
    }

    if (ladyLuckCurrency > 0)
    {
        std::string itemName = sObjectMgr->GetItemTemplate(ladyLuckCurrency)->Name1;
        ladyLuckCurrencyStr = Acore::StringFormatFmt("Are you sure you would like to use currency?|n|nThis will cost you :|n{}x[{}]", ladyLuckCurrencyCount, itemName);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I would like to use currency.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY_CURRENCY, ladyLuckCurrencyStr, 0, false);
    }

    SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT_SELECT_COST, creature);
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
        DisplayLotteryOptions(player, creature);
        break;

    case LADYLUCK_ENTERLOTTERY_CURRENCY:
        ValidateCurrency(player, creature);
        break;

    case LADYLUCK_ENTERLOTTERY_GOLD:
        ValidateMoney(player, creature);
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
    if (!ladyLuckEnabled)
    {
        return true;
    }

    ClearGossipMenuFor(player);

    if (CanLoot(player))
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Open the box", GOSSIP_SENDER_MAIN, LOTTERYBOX_OPEN);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I want to open a different box.", GOSSIP_SENDER_MAIN, LOTTERYBOX_GOODBYE);
        SendGossipMenuFor(player, LOTTERYBOX_GOSSIPTEXT, go->GetGUID());
    }
    else
    {
        SendGossipMenuFor(player, LOTTERYBOX_GOSSIPTEXT_DENY, go->GetGUID());
    }
    
    return true;
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
            UpdateCanLoot(player, false);
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

void LadyLuckWorldScript::OnShutdownInitiate(ShutdownExitCode /*code*/, ShutdownMask /*mask*/)
{
    for (auto& pLotInfo : playerLotteryInfo)
    {
        if (pLotInfo.inLottery)
        {
            CharacterDatabase.Execute("REPLACE INTO `ladyluck_restore_info` (guid, canLoot, map, x, y, z, o) VALUES ({}, {}, {}, {}, {}, {}, {})",
                pLotInfo.playerGuid.GetRawValue(), pLotInfo.canLoot,
                pLotInfo.previousLocation.Map,
                pLotInfo.previousLocation.X, pLotInfo.previousLocation.Y, pLotInfo.previousLocation.Z, pLotInfo.previousLocation.O);
        }
        else
        {
            CharacterDatabase.Execute("DELETE FROM `ladyluck_restore_info` WHERE guid = {}", pLotInfo.playerGuid.GetRawValue());
        }
    }
}

void LadyLuckWorldScript::OnAfterConfigLoad(bool reload)
{
    if (reload)
    {
        lotteryLootPool.clear();
    }

    ladyLuckEnabled = sConfigMgr->GetOption<bool>("LadyLuck.Enable", false);

    ladyLuckCurrency = sConfigMgr->GetOption<uint32>("LadyLuck.Currency", 37711);
    ladyLuckCurrencyCount = sConfigMgr->GetOption<uint32>("LadyLuck.CurrencyCount", 3);

    ladyLuckMoney = sConfigMgr->GetOption<uint32>("LadyLuck.Money", 1000000);

    ladyLuckTele.Map = sConfigMgr->GetOption<uint32>("LadyLuck.TeleMap", 449);
    ladyLuckTele.X = sConfigMgr->GetOption<float>("LadyLuck.TeleX", 0.072697);
    ladyLuckTele.Y = sConfigMgr->GetOption<float>("LadyLuck.TeleY", 9.618815);
    ladyLuckTele.Z = sConfigMgr->GetOption<float>("LadyLuck.TeleZ", -0.227239);
    ladyLuckTele.O = sConfigMgr->GetOption<float>("LadyLuck.TeleO", 1.584149);

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

    result = CharacterDatabase.Query("SELECT guid, canLoot, map, x, y, z, o FROM ladyluck_restore_info");

    if (!result)
    {
        LOG_ERROR("module", "Failed to load `ladyluck_restore_info` table!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        TeleportInfo teleInfo =
        {
            fields[2].Get<uint32>(), //Map
            fields[3].Get<float>(), //X
            fields[4].Get<float>(), //Y
            fields[5].Get<float>(), //Z
            fields[6].Get<float>() //O
        };
        PlayerLotteryInfo pLotInfo =
        {
            teleInfo, //TeleportInfo
            ObjectGuid(fields[0].Get<uint64>()), //Guid
            fields[1].Get<bool>(), //CanLoot
            true //InLottery
        };

        playerLotteryInfo.push_back(pLotInfo);
    } while (result->NextRow());
}

// Add all scripts in one
void AddLadyLuckScripts()
{
    new LadyLuckWorldScript();
    new LadyLuckCreatureScript();
    new LadyLuckGameObjectScript();
}
