

#include "SefaPoolData.h"
#include "SefaPool/Widgets/LoadingWidget.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void USefaPoolData::Init(USefaPoolGameInstance* gi_)
{
    gi = gi_;

    user = NewObject<USefaPoolUser>(gi_);
    stats = NewObject<USefaPoolStats>(gi_);
    shop = NewObject<USefaPoolShop>(gi_);
    friends = NewObject<USefaPoolFriends>(gi_);
    lobby = NewObject<USefaPoolLobby>(gi_);
    contests = NewObject<USefaPoolContests>(gi_);
    inventory = NewObject<USefaPoolInventory>(gi_);
    messages = NewObject<USefaPoolMessages>(gi_);
    settings = NewObject<USefaPoolSettings>(gi_);
    tournaments = NewObject<USefaPoolTournaments>(gi_);
    spectating = NewObject<USefaPoolSpectating>(gi_);
    replays = NewObject<USefaPoolReplays>(gi_);
    purchases = NewObject<USefaPoolPurchases>(gi_);
    championship = NewObject<USefaPoolChampionship>(gi_);

    user->Init(gi_);
    stats->Init(gi_);
    shop->Init(gi_);
    friends->Init(gi_);
    lobby->Init(gi_);
    contests->Init(gi_);
    inventory->Init(gi_);
    messages->Init(gi_);
    settings->Init(gi_);
    tournaments->Init(gi_);
    spectating->Init(gi_);
    replays->Init(gi_);
    purchases->Init(gi_);
    championship->Init(gi_);

    FullLoad();

}

void USefaPoolData::FullSave()
{
    user->Save();
    stats->Save();
    shop->Save();
    friends->Save();
    lobby->Save();
    contests->Save();
    inventory->Save();
    messages->Save();
    settings->Save();
    tournaments->Save();
    spectating->Save();
    replays->Save();
    purchases->Save();
    championship->Save();
}

void USefaPoolData::FullLoad()
{
    user->Load();
    stats->Load();
    shop->Load();
    friends->Load();
    lobby->Load();
    contests->Load();
    inventory->Load();
    messages->Load();
    settings->Load();
    tournaments->Load();
    spectating->Load();
    replays->Load();
    purchases->Load();
    championship->Load();
}
