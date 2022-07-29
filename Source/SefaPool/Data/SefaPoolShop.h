
#pragma once

#include "SefaPoolDataBlock.h"
#include "Engine/Texture2D.h"
#include "Interfaces/OnlineStoreInterface.h"
#include "InAppPurchaseQueryCallbackProxy2.h"

#include "SefaPoolShop.generated.h"


USTRUCT(BlueprintType)
struct FStaticCueInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        float effect = -1;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        float power = -1;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        float time = -1;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        int maxDurability = -1;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        FString price;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        FString priceCurrency;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        FString rechargePrice;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        FString rechargeCurrency;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        UTexture2D* defaultTexture;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        UTexture2D* selectedTexture;

};

USTRUCT(BlueprintType)
struct FStaticChalkInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
        int maxDurability = -1;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
        FString price;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
        FString priceCurrency;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
        FString rechargePrice;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
        FString rechargeCurrency;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
        UTexture2D* defaultTexture;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
        UTexture2D* selectedTexture;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
        UTexture2D* buyTexture;
};

USTRUCT(BlueprintType)
struct FStaticClothInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
        FString name;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
        FString price;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
        FString priceCurrency;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
        FLinearColor tint;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
        UTexture2D* defaultTexture;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
        UTexture2D* selectedTexture;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
        UTexture2D* clothTexture;
};

USTRUCT(BlueprintType)
struct FClothInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
    FString name;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
    uint8 r;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
    uint8 g;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
    uint8 b;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
    float adj;
};


USTRUCT(BlueprintType)
struct FStaticTokenPakInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "TokenPak")
        FString name;
    UPROPERTY(BlueprintReadWrite, Category = "TokenPak")
        int amount;
    UPROPERTY(BlueprintReadWrite, Category = "TokenPak")
        FString price;
    UPROPERTY(BlueprintReadWrite, Category = "TokenPak")
        FString priceCurrency;
};

USTRUCT(BlueprintType)
struct FStaticBundleInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Bundle")
        FString name;
    UPROPERTY(BlueprintReadWrite, Category = "Bundle")
        int amountTokens;
    UPROPERTY(BlueprintReadWrite, Category = "Bundle")
        int cueN;
    UPROPERTY(BlueprintReadWrite, Category = "Bundle")
        int chalkN;
    UPROPERTY(BlueprintReadWrite, Category = "Bundle")
        FString price;
    UPROPERTY(BlueprintReadWrite, Category = "Bundle")
        FString priceCurrency;
};




UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolShop : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FString localCurrency;
    
    UPROPERTY()
    TArray<FString> cue_iap_id = {TEXT("NO_ID"),
        TEXT("cue_1"),
        TEXT("cue_2"),
        TEXT("cue_3"),
        TEXT("cue_4"),
        TEXT("cue_5"),
        TEXT("cue_6"),
        TEXT("cue_7"),
        TEXT("cue_8"),
        TEXT("cue_9"),
        TEXT("cue_10"),
        TEXT("cue_11"),
        TEXT("cue_12"),
        TEXT("cue_13"),
        TEXT("cue_14"),
        TEXT("cue_15"),
        TEXT("cue_16"),
        TEXT("cue_17"),
        TEXT("cue_18"),
        TEXT("cue_19"),
        TEXT("cue_20")
    };
    TArray<FString> chalk_iap_id = {TEXT("NO_ID"),
        TEXT("chalk_1"),
        TEXT("chalk_2"),
        TEXT("chalk_3"),
        TEXT("chalk_4")
    };
    TArray<FString> tokenpak_iap_id = {TEXT("NO_ID"),
        TEXT("tokenpak_500_c"),
        TEXT("tokenpak_1500_c"),
        TEXT("tokenpak_4000_c"),
        TEXT("tokenpak_9000_c"),
        TEXT("tokenpak_20000_c"),
        TEXT("tokenpak_50000_c"),
        TEXT("tokenpak_100000_c"),
        TEXT("tokenpak_200000_c")
    };
    TArray<FString> cloth_iap_id = {TEXT("NO_ID"),
        TEXT("cloth_1"),
        TEXT("cloth_2"),
        TEXT("cloth_3"),
        TEXT("cloth_4"),
        TEXT("cloth_5"),
        TEXT("cloth_6"),
        TEXT("cloth_7"),
        TEXT("cloth_8"),
        TEXT("cloth_9"),
        TEXT("cloth_10"),
        TEXT("cloth_11"),
        TEXT("cloth_12"),
        TEXT("cloth_13"),
        TEXT("cloth_14"),
        TEXT("cloth_15"),
        TEXT("cloth_16"),
        TEXT("cloth_17"),
        TEXT("cloth_18"),
        TEXT("cloth_19"),
        TEXT("cloth_20"),
        TEXT("cloth_21"),
        TEXT("cloth_22"),
        TEXT("cloth_23"),
        TEXT("cloth_24"),
        TEXT("cloth_25"),
        TEXT("cloth_26"),
        TEXT("cloth_27"),
        TEXT("cloth_28")
    };
    TArray<FString> bundle_iap_id = {TEXT("NO_ID"),
        TEXT("bundle_1"),
        TEXT("bundle_2"),
        TEXT("bundle_3"),
        TEXT("bundle_4"),
        TEXT("bundle_5"),
        TEXT("bundle_6"),
        TEXT("bundle_7")
    };
    UPROPERTY(BlueprintReadOnly)
    TArray<FString> all_iap_ids;
    
    
    
    UPROPERTY()
    TArray<int> predefined_cue_durabilities = {-1, 200, 250, 250, 300, 320, 350, 380, 400, 420, 450,
                                                         450, 450, 450, 450, 450, 450, 500, 500, 750, 750};
    UPROPERTY()
    TArray<int> predefined_cue_prices = { -1, 1500, 5000, 10000, 15000, 20000,
                                                  25000, 30000, 35000, 42000, 50000,
                                                      3999, 4399, 4899, 5399, 5899, 6399, 6799, 7299, 7599, 7899 };
    UPROPERTY()
    TArray<bool> predefined_cue_cost_currency = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    UPROPERTY()
    TMap<int, FString> localized_cue_cost;
    
    UPROPERTY()
    TArray<int> predefined_chalk_durabilities = {-1, 200, 500, 2000, 5000};
    UPROPERTY()
    TArray<int> predefined_chalk_prices = { -1, 199, 399, 499, 799};
    UPROPERTY()
    TArray<bool> predefined_chalk_cost_currency = { 0, 1, 1, 1, 1};
    
    UPROPERTY()
    TMap<int, FString> localized_chalk_cost;
    
    UPROPERTY()
    TArray<int> predefined_token_paks_amounts = {-1, 500, 1500, 4000, 9000, 20000, 50000, 100000, 200000};
    UPROPERTY()
    TArray<int> predefined_token_paks_prices = { -1, 99, 199, 499, 999, 1999, 3999, 7499, 9999};
    UPROPERTY()
    TArray<bool> predefined_token_paks_cost_currency = { 0, 1, 1, 1, 1, 1, 1, 1, 1};
    
    UPROPERTY()
    TMap<int, FString> localized_token_pak_cost;
    
    UPROPERTY()
    TArray<int> predefined_bundle_token_amounts = {-1, 1500, 2000, 5000, 10000, 50000, 100000, 200000};
    UPROPERTY()
    TArray<int> predefined_bundle_prices = { -1, 1299, 1799, 2299, 3499, 5599, 7299, 11900};
    UPROPERTY()
    TArray<int> predefined_bundle_cues = { -1, 3, 6, 9, 13, 15, 17, 18};
    UPROPERTY()
    TArray<int> predefined_bundle_chalks = { -1, 1, 2, 3, 4, 2, 3, 4};
    UPROPERTY()
    TArray<bool> predefined_bundle_cost_currency = { 0, 1, 1, 1, 1, 1, 1, 1, 1};
    
    UPROPERTY()
    TMap<int, FString> localized_bundle_cost;
    
    
    UPROPERTY()
    TArray<int> predefined_cloth_prices = { -1,
        50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50
    };
    UPROPERTY()
    TArray<bool> predefined_cloth_cost_currency = { 0,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0
    };
    
    UPROPERTY()
    TMap<int, FString> localized_cloth_cost;
    
    UPROPERTY()
    TArray<UTexture2D*> default_cue_textures;
    
    UPROPERTY()
    TArray<UTexture2D*> active_cue_textures;
    
    UPROPERTY()
    TArray<UTexture2D*> buy_chalk_textures;
    
    UPROPERTY()
    TArray<UTexture2D*> owned_chalk_textures;
    
    UPROPERTY()
    TArray<UTexture2D*> selected_chalk_textures;
    UPROPERTY()
    UTexture2D* defaultClothTexture;
    UPROPERTY()
    UTexture2D* selectedClothTexture;
    UPROPERTY()
    UTexture2D* clothClothTexture;
    UPROPERTY()
    TArray<FClothInfo> clothsInfo = {
        {TEXT("NOCLOTH"), {},},
        {TEXT("Vert jaune"), 17, 71, 42, 8},
        {TEXT("Vert bleu"),  18, 74, 60, 8},
        {TEXT("Vert anglais"), 43, 92, 41, 8},
        {TEXT("Vert foncé"), 23, 61, 39, 8},
        {TEXT("Vert pomme"), 57, 139, 50, 8},
        {TEXT("Vert olive"),  96, 84, 54, 8},
        {TEXT("Vert épicéa"), 27, 45, 26, 8},
        {TEXT("Bleu marine"), 20, 18, 30, 8},
        {TEXT("Bleu royal"), 31, 46, 100, 8},
        {TEXT("Bleu pétrole"), 22, 53, 58, 8},
        {TEXT("Bleu électrique"), 62, 100, 128, 8},
        {TEXT("Bleu tournoi"), 46, 98, 156, 8},
        {TEXT("Bleu poudre"), 100, 117, 114, 8},
        {TEXT("Vin"), 84, 26, 33, 8},
        {TEXT("Bordeaux"), 110, 40, 46, 8},
        {TEXT("Rouge"), 151, 27, 43, 8},
        {TEXT("Violet"), 104, 72, 105, 8},
        {TEXT("Fuchsia"), 146, 40, 97, 8},
        {TEXT("Vieux rose"), 181, 141, 170, 8},
        {TEXT("Orange"), 218, 79, 57, 8},
        {TEXT("Chocolat"), 82, 57, 50, 8},
        {TEXT("Taupe"), 121, 105, 98, 8},
        {TEXT("Fauve"), 164, 124, 73, 8},
        {TEXT("Or"), 169, 139, 74, 8},
        {TEXT("Gris"), 148, 149, 129, 8},
        {TEXT("Gris ardoise"), 82, 77, 77, 8},
        {TEXT("Ecru"), 225, 210, 182, 8},
        {TEXT("Noir"), 51, 45, 44, 8},
    };
    
public:
    
    virtual void Init(USefaPoolGameInstance* gi_);
    
    
    UFUNCTION(BlueprintCallable)
    FStaticCueInfo   GetStaticCueInfo(int num);
    UFUNCTION(BlueprintCallable)
    FStaticChalkInfo GetStaticChalkInfo(int num);
    UFUNCTION(BlueprintCallable)
    FStaticClothInfo GetStaticClothInfo(int num);
    UFUNCTION(BlueprintCallable)
    FStaticTokenPakInfo GetStaticTokenPakInfo(int num);
    UFUNCTION(BlueprintCallable)
    FStaticBundleInfo GetStaticBundleInfo(int num);
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    UFUNCTION(BlueprintCallable)
    virtual void ValidateReceipt(const FString& item_id, const FString& receipt);
    
    UFUNCTION(BlueprintCallable)
    void ProcessIAPInfoV1(const TArray<FInAppPurchaseProductInfo>& infos);
    UFUNCTION(BlueprintCallable)
    void ProcessIAPInfoV2(const TArray<FOnlineProxyStoreOffer>& infos);
    
    void ProcessBuyItem(EConsumableType type,
                        int id_,
                        const FResponseDelegat& successful,
                        const FResponseDelegat& unsuccessful);

};
