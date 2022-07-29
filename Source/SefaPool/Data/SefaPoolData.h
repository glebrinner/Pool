#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SefaPoolUser.h"
#include "SefaPoolStats.h"
#include "SefaPoolShop.h"
#include "SefaPoolFriends.h"
#include "SefaPoolLobby.h"
#include "SefaPoolContests.h"
#include "SefaPoolInventory.h"
#include "SefaPoolMessages.h"
#include "SefaPoolSettings.h"
#include "SefaPoolTournaments.h"
#include "SefaPoolSpectating.h"
#include "SefaPoolReplays.h"
#include "SefaPoolChampionship.h"
#include "SefaPoolPurchases.h"
#include "SefaPoolData.generated.h"

class USefaPoolGameInstance;

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolData : public UObject
{
    GENERATED_BODY()
        USefaPoolGameInstance* gi;
public:

    UPROPERTY(BlueprintReadOnly)
        USefaPoolUser* user;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolStats* stats;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolShop* shop;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolFriends* friends;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolLobby* lobby;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolContests* contests;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolInventory* inventory;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolMessages* messages;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolSettings* settings;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolTournaments* tournaments;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolSpectating* spectating;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolReplays* replays;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolPurchases* purchases;

    UPROPERTY(BlueprintReadOnly)
        USefaPoolChampionship* championship;

    void Init(USefaPoolGameInstance* gi_);

    void Offline(USefaPoolGameInstance* gi_);

    UFUNCTION(BlueprintCallable)
        void FullSave();
    UFUNCTION(BlueprintCallable)
        void FullLoad();

};
