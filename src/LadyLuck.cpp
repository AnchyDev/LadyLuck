#include "LadyLuck.h"

bool LadyLuckCreatureScript::OnGossipHello(Player* player, Creature* creature)
{
    ClearGossipMenuFor(player);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I would like to enter the lottery.", GOSSIP_SENDER_MAIN, LADYLUCK_ENTERLOTTERY);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Goodbye.", GOSSIP_SENDER_MAIN, LADYLUCK_GOODBYE);
    SendGossipMenuFor(player, LADYLUCK_GOSSIPTEXT, creature->GetGUID());

    return true;
}

void LadyLuckCreatureScript::EnterLottery(Player* player)
{
    CloseGossipMenuFor(player);
    DeductCurrency(player, ladyLuckCurrencyCount);
    player->TeleportTo(ladyLuckTele.Map, ladyLuckTele.X, ladyLuckTele.Y, ladyLuckTele.Z, ladyLuckTele.O);
}

void LadyLuckCreatureScript::SayGoodbye(Player* player, Creature* /*creature*/)
{
    CloseGossipMenuFor(player);
}

bool LadyLuckCreatureScript::CanEnterLottery(Player* player)
{
    uint32 currencyAmount = player->GetItemCount(ladyLuckCurrency, false);

    if (currencyAmount < ladyLuckCurrencyCount)
    {
        return false;
    }

    return true;
}

void LadyLuckCreatureScript::DeductCurrency(Player* player, uint32 count)
{
    Item* currency = player->GetItemByEntry(ladyLuckCurrency);
    player->DestroyItemCount(currency, count, true);
}

void LadyLuckCreatureScript::ValidateCurrency(Player* player, Creature* creature)
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

bool LadyLuckCreatureScript::OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
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


bool LadyLuckGameObjectScript::OnGossipHello(Player* player, GameObject* go)
{
    ClearGossipMenuFor(player);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Open", GOSSIP_SENDER_MAIN, LOTTERYBOX_OPEN);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I would like to open another box.", GOSSIP_SENDER_MAIN, LOTTERYBOX_GOODBYE);
    SendGossipMenuFor(player, LOTTERYBOX_GOSSIPTEXT, go->GetGUID());

    return true;
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
        ChatHandler(player->GetSession()).SendSysMessage("OPEN");
        break;

    case LOTTERYBOX_GOODBYE:
        ChatHandler(player->GetSession()).SendSysMessage("GOODBYE");
        break;
    }

    return true;
}

void LadyLuckWorldScript::OnAfterConfigLoad(bool /*reload*/)
{
    ladyLuckEnabled = sConfigMgr->GetOption<bool>("LadyLuck.Enable", false);

    ladyLuckCurrency = sConfigMgr->GetOption<uint32>("LadyLuck.Currency", 37711);
    ladyLuckCurrencyCount = sConfigMgr->GetOption<uint32>("LadyLuck.CurrencyCount", 3);

    ladyLuckTele.Map = sConfigMgr->GetOption<uint32>("LadyLuck.TeleMap", 449);
    ladyLuckTele.X = sConfigMgr->GetOption<float>("LadyLuck.TeleX", 0.072697);
    ladyLuckTele.Y = sConfigMgr->GetOption<float>("LadyLuck.TeleY", 9.618815);
    ladyLuckTele.Z = sConfigMgr->GetOption<float>("LadyLuck.TeleZ", -0.227239);
    ladyLuckTele.O = sConfigMgr->GetOption<float>("LadyLuck.TeleO", 1.584149);
}

// Add all scripts in one
void AddLadyLuckScripts()
{
    new LadyLuckWorldScript();
    new LadyLuckCreatureScript();
    new LadyLuckGameObjectScript();
}
