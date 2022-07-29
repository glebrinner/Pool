// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CueComponent.h"
#include "PoolPlayerAimLine.h"
#include "SharedDefs.h"
#include "PoolClientGameInstance.h"
#include "SefaPool/Data/SefaPoolData.h"

#include "UObject/Object.h"

#include "OrientationManager.h"

#include "WidgetManager.h"


#include "SefaPoolGameInstance.generated.h"

/**
 *
 */




USTRUCT(BlueprintType)
struct FGameStatistics
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "GameStatistics")
        TArray<int32> cue_shots;

    UPROPERTY(BlueprintReadWrite, Category = "GameStatistics")
        TArray<int32> cue_pocketed_balls;

    UPROPERTY(BlueprintReadWrite, Category = "GameStatistics")
        int32 avrg_num_of_shots_per_game;

};



UENUM(BlueprintType)
enum class EMainMenuPage : uint8
{
    NO_PAGE    = 0 UMETA(DisplayName = "No page"),
    SOLO       = 1 UMETA(DisplayName = "Solo"), 
    VERSUS     = 2 UMETA(DisplayName = "Versus"), 
    TRAINING   = 3 UMETA(DisplayName = "Training"), 
    TOURNAMENT = 4 UMETA(DisplayName = "Tournament"),
    SHOP       = 5 UMETA(DisplayName = "Shop"),
    GAMEMODE   = 6 UMETA(DisplayName = "GameMode"),
    ACCOUNT    = 7 UMETA(DisplayName = "Account")
};

USTRUCT(BlueprintType)
struct SEFAPOOL_API FItemDurability
{
GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "ItemDurability")
    TArray<int32> cue_durabilities;
    UPROPERTY(BlueprintReadWrite, Category = "ItemDurability")
    TArray<int32> cue_usages;
    UPROPERTY(BlueprintReadWrite, Category = "ItemDurability")
    TArray<int32> cue_recharges;
    UPROPERTY(BlueprintReadWrite, Category = "ItemDurability")
    TArray<int32> chalk_durabilities;
    UPROPERTY(BlueprintReadWrite, Category = "ItemDurability")
    TArray<int32> chalk_usages;
    UPROPERTY(BlueprintReadWrite, Category = "ItemDurability")
    TArray<int32> chalk_recharges;
public:
    bool ChangeConsumableDurability(EConsumableType type, int num, int value);
    bool RechargeConsumable(EConsumableType type, int num);
    int  GetDurability(EConsumableType type, int num);
    //void SetCueDurability(int32* cue_durability, int size);
    //void SetChalkDurability(int32* chalk_durability_array, int size);
    FItemDurability();
};




UCLASS()
class SEFAPOOL_API USefaPoolGameInstance : public UPoolClientGameInstance
{
    GENERATED_BODY()

    virtual void Init();
    
public:
    void SetItemDurability(const FItemDurability &item_durability);
    
    
    UFUNCTION(BlueprintCallable)
    void SwitchToOrientation(E_ScreenOrientation orientation);
    
    void RequestEntyDataForMainMenu(const FResponseDelegat& succesful,
                                    const FResponseDelegat& unsuccesful);
    
    
    UPROPERTY(BlueprintReadWrite)
    EDifficultyLevels DiffLevel;
    
    UPROPERTY(BlueprintReadWrite)
    UWidgetManager* widgetManager;
    
    UPROPERTY(BlueprintReadWrite)
    USefaPoolData* data;
    
    UPROPERTY(BlueprintReadWrite)
    UOrientationManager* orientationManager;
    
    UFUNCTION(BlueprintImplementableEvent)
    void InitWidgetManager();
    
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OpenLevel(const FString& Level, const FString& Options);
    
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowLoadingScreen();
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void HideLoadingScreen();
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowPopup(const FString& title, const FString& description, const FString& button);
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowInteractivePopup(
        const FString& title, 
        const FString& description, 
        const FString& leftButton,
        const FString& rightButton,
        const FResponseDelegat& left,
        const FResponseDelegat& right);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowNewMessagePopup(const FRemoteMessageInfo& message);
    
    UFUNCTION(BlueprintImplementableEvent)
    void PullIAPInfoV1(const TArray<FString>& iap_ids);
    UFUNCTION(BlueprintImplementableEvent)
    void PullIAPInfoV2(const TArray<FString>& iap_ids);
    UFUNCTION(BlueprintImplementableEvent)
    void MakeInAppPurchaseV1(const FString& iap_id,
                             bool consumable, 
                             const FResponseDelegat& succesful,
                             const FResponseDelegat& unsuccesful);
    UFUNCTION(BlueprintImplementableEvent)
    void MakeInAppPurchaseV2(const FString& iap_id,
                             bool consumable,
                             const FResponseDelegat& succesful,
                             const FResponseDelegat& unsuccesful);

    virtual void PlayingOffline();
    
private:
    FItemDurability item_durability_;
    FGameStatistics game_statistics_;
    
};


void from_json(const cool_json_t& json, EDifficultyLevels& level);
void to_json(cool_json_t& json, const EDifficultyLevels& level);
