
#include "SefaPoolShop.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPool/Shared/CueComponent.h"
#include "SefaPool/Widgets/PoolBaseShopWidget.h"

void USefaPoolShop::Init(USefaPoolGameInstance* gi_)
{
    Super::Init(gi_);
    TArray<FString> cue_texture_files;
    IFileManager& FileManager = IFileManager::Get();

    //Inititalizing map of cue textures
    FString cue_textures_path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Content/ASSETS/CueStore");
    FileManager.FindFiles(cue_texture_files, *cue_textures_path, TEXT(".uasset"));
    
    active_cue_textures.Empty();
    active_cue_textures.Init(nullptr, cue_texture_files.Num() + 1);
    
    default_cue_textures.Empty();
    default_cue_textures.Init(nullptr, cue_texture_files.Num() + 1);
    for (auto name : cue_texture_files)
    {
        name.RemoveFromEnd(FString(".uasset"));
        FString prefix = name.Mid(0, name.Find(FString("_")));
        if (prefix.IsNumeric())
        {
            auto texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), gi, *FString::Format(TEXT("/Game/ASSETS/CueStore/{0}.{0}"), {name})));
            if (name.Contains(FString("select")))
            {
                if (texture)
                    active_cue_textures[FCString::Atoi(*prefix)] = texture;
                else
                    PRINTF((TEXT("Error when loading texture")), Red);
            }
            else
            {
                if (texture)
                    default_cue_textures[FCString::Atoi(*prefix)] = texture;
                else
                    PRINTF((TEXT("Error when loading texture")), Red);
            }
        }
    }
    
    TArray<FString> chalk_texture_files;
    
    FString chalk_textures_path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Content/ASSETS/UI/Shop/Chalks/Textures");
    FileManager.FindFiles(chalk_texture_files, *chalk_textures_path, TEXT(".uasset"));
    
    buy_chalk_textures.Empty();
    buy_chalk_textures.Init(nullptr, chalk_texture_files.Num() + 1);
    owned_chalk_textures.Empty();
    owned_chalk_textures.Init(nullptr, chalk_texture_files.Num() + 1);
    selected_chalk_textures.Empty();
    selected_chalk_textures.Init(nullptr, chalk_texture_files.Num() + 1);
    
    
    for (auto name : chalk_texture_files)
    {
        name.RemoveFromEnd(FString(".uasset"));
        int first_underscore = name.Find(FString("_"));
        FString prefix = name.Mid(first_underscore + 1,
                                  name.Find(FString("_"),
                                            ESearchCase::CaseSensitive,
                                            ESearchDir::FromStart,
                                            first_underscore + 1) - (first_underscore + 1));
        if (prefix.IsNumeric())
        {
            auto texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), gi, *FString::Format(TEXT("/Game/ASSETS/UI/Shop/Chalks/Textures/{0}.{0}"), {name})));
            if (name.Contains(FString("buy")))
            {
                if (texture)
                    buy_chalk_textures[FCString::Atoi(*prefix)] = texture;
                else
                    PRINTF((TEXT("Error when loading texture")), Red);
            }
            else
            if (name.Contains(FString("owned")))
            {
                if (texture)
                    owned_chalk_textures[FCString::Atoi(*prefix)] = texture;
                else
                    PRINTF((TEXT("Error when loading texture")), Red);
            }
            else
            if (name.Contains(FString("selected")))
            {
                if (texture)
                    selected_chalk_textures[FCString::Atoi(*prefix)] = texture;
                else
                    PRINTF((TEXT("Error when loading texture")), Red);
            }
        }
    }
    
    
    clothClothTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(),
                                                          gi,TEXT("/Game/ASSETS/UI/Shop/ClothTest/ClothImageCloth.ClothImageCloth")));
    
    selectedClothTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(),
                                                             gi, TEXT("/Game/ASSETS/UI/Shop/ClothTest/ClothImageSelected.ClothImageSelected")));
    
    defaultClothTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(),
                                                            gi, TEXT("/Game/ASSETS/UI/Shop/ClothTest/ClothImageDefault.ClothImageDefault")));
}

FStaticCueInfo USefaPoolShop::GetStaticCueInfo(int num)
{
    FStaticCueInfo cue_widget_info;
    if (num < 1 || num > PREDEFINED_CUES_N)
    {
        PRINTF((TEXT("Wrong cue number in GetStaticCueInfo, number is: %d"), num), Red);
        return cue_widget_info;
    }
    cue_widget_info.power = FCueInfo::GetPower(num);
    cue_widget_info.effect = FCueInfo::GetEffect(num);
    cue_widget_info.time = FCueInfo::GetTime(num);
    
    cue_widget_info.maxDurability = predefined_cue_durabilities[num];
    
    if (predefined_cue_cost_currency[num])
    {
        if (localized_cue_cost.Contains(num))
        {
            cue_widget_info.price = localized_cue_cost[num];
            cue_widget_info.priceCurrency = localCurrency;
        }
        else
        {
            cue_widget_info.price = FString::FromInt(predefined_cue_prices[num]/100);
            cue_widget_info.price += ".";
            cue_widget_info.price += FString::FromInt(predefined_cue_prices[num]%100);
            cue_widget_info.priceCurrency = TEXT("€");
        }
    }
    else
    {
        cue_widget_info.price = FString::FromInt(predefined_cue_prices[num]);
        cue_widget_info.priceCurrency = TEXT("tokens");
    }
    
    cue_widget_info.rechargePrice = TEXT("100");
    cue_widget_info.rechargeCurrency = TEXT("tokens");
    
    cue_widget_info.defaultTexture = default_cue_textures[num];
    cue_widget_info.selectedTexture = active_cue_textures[num];
    
    return cue_widget_info;
}


FStaticChalkInfo USefaPoolShop::GetStaticChalkInfo(int num)
{
    FStaticChalkInfo chalk_widget_info;
    if (num < 1 || num > PREDEFINED_CHALKS_N)
    {
        PRINTF((TEXT("Wrong chalk number in GetStaticChalkInfo, number is: %d"), num), Red);
        return chalk_widget_info;
    }
    
    chalk_widget_info.maxDurability = predefined_chalk_durabilities[num];
    
    if (predefined_chalk_cost_currency[num])
    {
        if (localized_cloth_cost.Contains(num))
        {
            chalk_widget_info.price = localized_cloth_cost[num];
            chalk_widget_info.priceCurrency = localCurrency;
        }
        else
        {
            chalk_widget_info.price = FString::FromInt(predefined_chalk_prices[num]/100);
            chalk_widget_info.price += ".";
            chalk_widget_info.price += FString::FromInt(predefined_chalk_prices[num]%100);
            chalk_widget_info.priceCurrency = TEXT("€");
        }
    }
    else
    {
        chalk_widget_info.price = FString::FromInt(predefined_chalk_prices[num]);
        chalk_widget_info.priceCurrency = TEXT("tokens");
    }
    
    chalk_widget_info.rechargePrice = TEXT("100");
    chalk_widget_info.rechargeCurrency = TEXT("tokens");
    
    chalk_widget_info.defaultTexture = owned_chalk_textures[num];
    chalk_widget_info.selectedTexture = selected_chalk_textures[num];
    chalk_widget_info.buyTexture = buy_chalk_textures[num];
    return chalk_widget_info;
}


FStaticClothInfo USefaPoolShop::GetStaticClothInfo(int num)
{
    FStaticClothInfo cloth_widget_info;
    if (num < 1 || num > PREDEFINED_CLOTHS_N)
    {
        PRINTF((TEXT("Wrong cloth number in GetStaticClothInfo, number is: %d"), num), Red);
        return cloth_widget_info;
    }
    auto info = clothsInfo[num];
    cloth_widget_info.name = info.name;
    if (predefined_cloth_cost_currency[num])
    {
        if (localized_cloth_cost.Contains(num))
        {
            cloth_widget_info.price = localized_cloth_cost[num];
            cloth_widget_info.priceCurrency = localCurrency;
        }
        else
        {
            cloth_widget_info.price = FString::FromInt(predefined_cloth_prices[num]/100);
            cloth_widget_info.price += ".";
            cloth_widget_info.price += FString::FromInt(predefined_cloth_prices[num]%100);
            cloth_widget_info.priceCurrency = TEXT("€");
        }
    }
    else
    {
        cloth_widget_info.price = FString::FromInt(predefined_cloth_prices[num]);
        cloth_widget_info.priceCurrency = TEXT("tokens");
    }
    
    
    FColor color(info.r, info.g, info.b, 255);
    FLinearColor colorLinear = color.ReinterpretAsLinear();
    colorLinear = colorLinear.LinearRGBToHSV();
    colorLinear.G = 1.0f - (1.0f - colorLinear.G) / 1.5f;
    cloth_widget_info.tint = colorLinear.HSVToLinearRGB();
    
    cloth_widget_info.tint.A = info.adj;
    cloth_widget_info.defaultTexture = defaultClothTexture;
    cloth_widget_info.selectedTexture = selectedClothTexture;
    cloth_widget_info.clothTexture = clothClothTexture;
    
    return cloth_widget_info;
}


FStaticTokenPakInfo USefaPoolShop::GetStaticTokenPakInfo(int num)
{
    FStaticTokenPakInfo tokenpak_widget_info;
    if (num < 1 || num > PREDEFINED_TOKENPAKS_N)
    {
        PRINTF((TEXT("Wrong tokenpak number in GetStaticTokenPakInfo, number is: %d"), num), Red);
        return tokenpak_widget_info;
    }
    
    tokenpak_widget_info.name = FString::FromInt(predefined_token_paks_amounts[num]) + TEXT(" tokens");
    tokenpak_widget_info.amount = predefined_token_paks_amounts[num];
    if (predefined_token_paks_cost_currency[num])
    {
        if (localized_token_pak_cost.Contains(num))
        {
            tokenpak_widget_info.price = localized_token_pak_cost[num];
            tokenpak_widget_info.priceCurrency = localCurrency;
        }
        else
        {
            tokenpak_widget_info.price = FString::FromInt(predefined_token_paks_prices[num]/100);
            tokenpak_widget_info.price += ".";
            tokenpak_widget_info.price += FString::FromInt(predefined_token_paks_prices[num]%100);
            tokenpak_widget_info.priceCurrency = TEXT("€");
        }
    }
    else
    {
        tokenpak_widget_info.price = FString::FromInt(predefined_token_paks_prices[num]);
        tokenpak_widget_info.priceCurrency = TEXT("tokens");
    }
    
    return tokenpak_widget_info;
}

FStaticBundleInfo USefaPoolShop::GetStaticBundleInfo(int num)
{
    FStaticBundleInfo bundle_widget_info;
    if (num < 1 || num > PREDEFINED_BUNDLES_N)
    {
        PRINTF((TEXT("Wrong bundle number in GetStaticBundleInfo, number is: %d"), num), Red);
        return bundle_widget_info;
    }
    
    bundle_widget_info.name = FString::FromInt(num);
    bundle_widget_info.amountTokens = predefined_bundle_token_amounts[num];
    bundle_widget_info.cueN   = predefined_bundle_cues[num];
    bundle_widget_info.chalkN = predefined_bundle_chalks[num];
    
    if (predefined_bundle_cost_currency[num])
    {
        if (localized_bundle_cost.Contains(num))
        {
            bundle_widget_info.price = localized_bundle_cost[num];
            bundle_widget_info.priceCurrency = localCurrency;
        }
        else
        {
            bundle_widget_info.price = FString::FromInt(predefined_bundle_prices[num]/100);
            bundle_widget_info.price += ".";
            bundle_widget_info.price += FString::FromInt(predefined_bundle_prices[num]%100);
            bundle_widget_info.priceCurrency = TEXT("€");
        }
    }
    else
    {
        bundle_widget_info.price = FString::FromInt(predefined_bundle_prices[num]);
        bundle_widget_info.priceCurrency = TEXT("tokens");
    }
    
    return bundle_widget_info;
}

void USefaPoolShop::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "shop", 0);
}
    
void USefaPoolShop::Load()
{
    USefaPoolShop* LoadedGame = Cast<USefaPoolShop>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "shop", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load shop")), Yellow)
    }
    else
    {
        predefined_cue_durabilities = LoadedGame->predefined_cue_durabilities;
        predefined_cue_prices = LoadedGame->predefined_cue_prices;
        predefined_cue_cost_currency = LoadedGame->predefined_cue_cost_currency;
        predefined_chalk_durabilities = LoadedGame->predefined_chalk_durabilities;
        predefined_chalk_prices = LoadedGame->predefined_chalk_prices;
        predefined_chalk_cost_currency = LoadedGame->predefined_chalk_cost_currency;
        predefined_cloth_prices = LoadedGame->predefined_cloth_prices;
        predefined_cloth_cost_currency = LoadedGame->predefined_cloth_cost_currency;
        clothsInfo = LoadedGame->clothsInfo;
        PRINTF((TEXT("Shop successfully loaded")), Green)
    }
}



/*
 {
   "bundles": [
     {
       "ShopChalks_id": 2002,
       "ShopCues_id": 1015,
       "id": 5,
       "price": 5599,
       "priceCurrency": "fiat",
       "token_amount": 50000,
       "iap_id": "..."
     }
   ],
   "chalks": [
     {
       "id": 2,
       "number_of_shots": 500,
       "price": 399,
       "priceCurrency": "fiat",
       "rechargeCurrency": "tokens",
       "recharge_price": 200,
       "iap_id": "..."
     }
   ],
   "cloths": [
     {
       "adj": 1500,
       "blue_num": 0,
       "green_num": 100,
       "id": 26,
       "price": 100,
       "priceCurrency": "tokens",
       "red_num": 0,
       "iap_id": "..."
     }
   ],
   "cues": [
     {
       "effect": 55,
       "hits": 500,
       "id": 17,
       "power": 55,
       "price": 6799,
       "priceCurrency": "fiat",
       "rechargeCurrency": "tokens",
       "recharge_price": 5000,
       "time": 2440,
       "iap_id": "..."
     }
   ],
   "hash": "4941020805711779390",
   "match": false,
   "pics": [
     {
       "id": 1,
       "price": 100,
       "priceCurrency": "tokens",
       "iap_id": "..."
     }
   ],
   "status": 200,
   "tokenpaks": [
     {
       "amount": 9000,
       "id": 4,
       "price": 999,
       "priceCurrency": "fiat",
       "iap_id": "..."
     },
   ]
 }*/
    
void USefaPoolShop::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                      const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "shop/static_info"))
        {
            UE_LOG(LogTemp, Warning, TEXT("    SHOP: %s"), *(FString(resp_json.dump(2).c_str())))
            
            all_iap_ids.Empty();
            
            if (resp_json.contains("cues") &&
                resp_json["cues"].is_array())
            {
                for (auto & cue_item : resp_json["cues"].items())
                {
                    auto& cue = cue_item.value();
                    int id_ = cue["id"];
                    predefined_cue_durabilities[id_] = cue["hits"].get<int>();
                    predefined_cue_prices[id_] = cue["price"].get<int>();
                    predefined_cue_cost_currency[id_] = cue["priceCurrency"].get<std::string>() == "fiat";
                    cue_iap_id[id_] = cue["iap_id"].get<FString>();
                    all_iap_ids.Add(cue_iap_id[id_]);
                }
            }
            if (resp_json.contains("chalks") &&
                resp_json["chalks"].is_array())
            {
                for (auto & chalk_item : resp_json["chalks"].items())
                {
                    auto& chalk = chalk_item.value();
                    int id_ = chalk["id"];
                    
                    predefined_chalk_durabilities[id_] = chalk["hits"].get<int>();
                    predefined_chalk_prices[id_] = chalk["price"].get<int>();
                    predefined_chalk_cost_currency[id_] = chalk["priceCurrency"].get<std::string>() == "fiat";
                    chalk_iap_id[id_] = chalk["iap_id"].get<FString>();
                    all_iap_ids.Add(chalk_iap_id[id_]);
                }
            }
            if (resp_json.contains("cloths") &&
                resp_json["cloths"].is_array())
            {
                for (auto & cloth_item : resp_json["cloths"].items())
                {
                    auto& cloth = cloth_item.value();
                    int id_ = cloth["id"];
                    predefined_cloth_prices[id_] = cloth["price"];
                    predefined_cloth_cost_currency[id_] = cloth["priceCurrency"].get<std::string>() == "fiat";
                    cloth_iap_id[id_] = cloth["iap_id"].get<FString>();
                    all_iap_ids.Add(cloth_iap_id[id_]);
                }
            }
            if (resp_json.contains("tokenpaks") &&
                resp_json["tokenpaks"].is_array())
            {
                for (auto & tokenpak_item : resp_json["tokenpaks"].items())
                {
                    auto& tokenpak = tokenpak_item.value();
                    int id_ = tokenpak["id"];
                    predefined_token_paks_prices[id_] = tokenpak["price"];
                    predefined_token_paks_cost_currency[id_] = tokenpak["priceCurrency"].get<std::string>() == "fiat";
                    predefined_token_paks_amounts[id_] = tokenpak["amount"];
                    tokenpak_iap_id[id_] = tokenpak["iap_id"].get<FString>();
                    all_iap_ids.Add(tokenpak_iap_id[id_]);
                }
            }
            if (resp_json.contains("bundles") &&
                resp_json["bundles"].is_array())
            {
                for (auto & bundle_item : resp_json["bundles"].items())
                {
                    auto& bundle = bundle_item.value();
                    int id_ = bundle["id"];
                    predefined_bundle_prices[id_] = bundle["price"];
                    predefined_bundle_cost_currency[id_] = bundle["priceCurrency"].get<std::string>() == "fiat";
                    predefined_bundle_token_amounts[id_] = bundle["token_amount"];
                    predefined_bundle_cues[id_]   = bundle["ShopCues_id"].get<int>() % 1000;
                    predefined_bundle_chalks[id_] = bundle["ShopChalks_id"].get<int>() % 1000;
                    bundle_iap_id[id_] = bundle["iap_id"].get<FString>();
                    all_iap_ids.Add(bundle_iap_id[id_]);
                }
            }
            if (successful) successful(this);
            Save();
            
            #if PLATFORM_IOS
            gi->PullIAPInfoV1(all_iap_ids);
            #elif PLATFORM_ANDROID
            gi->PullIAPInfoV2(all_iap_ids);
            #endif
            
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequestWithFail("shop", "static_info", cool_json_t(), callback, [](){PRINTF((TEXT("Request failed")), Red)});
}


void USefaPoolShop::ValidateReceipt(const FString& item_id, const FString& receipt)
{
    if (!gi)
    {
        return;
    }
    
    auto callback = [this](cool_json_t resp_json) -> void {
        
    };
    cool_json_t req;
    req["item_iap_id"] = item_id;
    req["receipt"] = receipt;
#if PLATFORM_IOS
    req["platform"] = "ios";
#elif PLATFORM_ANDROID
    req["platform"] = "android";
#endif
    gi->MakeRequestWithFail("shop", "validate_iap", req, callback, [](){PRINTF((TEXT("Request failed")), Red)});
}

void USefaPoolShop::ProcessIAPInfoV1(const TArray<FInAppPurchaseProductInfo>& infos)
{
    for (auto& info : infos)
    {
        int foundIdx = 0;
        localCurrency = info.CurrencyCode;
        FString price = info.DisplayPrice;
        price = price.Replace(*localCurrency, TEXT(""));
        price.RemoveFromEnd(TEXT(" "));
        price.RemoveFromStart(TEXT(" "));
        
        if (cue_iap_id.Find(info.Identifier, foundIdx))
            localized_cue_cost.Add(foundIdx, price);
        if (chalk_iap_id.Find(info.Identifier, foundIdx))
            localized_chalk_cost.Add(foundIdx, price);
        if (tokenpak_iap_id.Find(info.Identifier, foundIdx))
            localized_token_pak_cost.Add(foundIdx, price);
        if (cloth_iap_id.Find(info.Identifier, foundIdx))
            localized_token_pak_cost.Add(foundIdx, price);
        if (bundle_iap_id.Find(info.Identifier, foundIdx))
            localized_bundle_cost.Add(foundIdx, price);
    }
    UPoolBaseShopWidget* shop = dynamic_cast<UPoolBaseShopWidget*>(gi->widgetManager->GetWidget("MainShop"));
    if (shop) shop->Refresh();
}

void USefaPoolShop::ProcessIAPInfoV2(const TArray<FOnlineProxyStoreOffer>& infos)
{
    for (auto& info : infos)
    {
        localCurrency = info.CurrencyCode;
        FString price = info.PriceText.ToString();
        price = price.Replace(*localCurrency, TEXT(""));
        price.RemoveFromEnd(TEXT(" "));
        price.RemoveFromStart(TEXT(" "));
        
        int foundIdx = 0;
        if (cue_iap_id.Find(info.OfferId, foundIdx))
            localized_cue_cost.Add(foundIdx, price);
        if (chalk_iap_id.Find(info.OfferId, foundIdx))
            localized_chalk_cost.Add(foundIdx, price);
        if (tokenpak_iap_id.Find(info.OfferId, foundIdx))
            localized_token_pak_cost.Add(foundIdx, price);
        if (cloth_iap_id.Find(info.OfferId, foundIdx))
            localized_token_pak_cost.Add(foundIdx, price);
        if (bundle_iap_id.Find(info.OfferId, foundIdx))
            localized_bundle_cost.Add(foundIdx, price);
    }
    UPoolBaseShopWidget* shop = dynamic_cast<UPoolBaseShopWidget*>(gi->widgetManager->GetWidget("MainShop"));
    if (shop) shop->Refresh();
}


void USefaPoolShop::ProcessBuyItem(EConsumableType type,
                                   int id_,
                                   const FResponseDelegat& successful,
                                   const FResponseDelegat& unsuccessful)
{
    FString endpoint = TEXT("buy_item");
    std::string item_type;
    bool active = false;
    FString iap_id;
    bool iap = false;
    switch(type)
    {
        case EConsumableType::CUE:
            item_type = "cue";
            active = true;
            iap_id = cue_iap_id[id_];
            iap = predefined_cue_cost_currency[id_];
            break;
        case EConsumableType::CHALK:
            item_type = "chalk";
            active = true;
            iap_id = chalk_iap_id[id_];
            iap = predefined_chalk_cost_currency[id_];
            break;
        case EConsumableType::CLOTH:
            item_type = "cloth";
            active = true;
            iap_id = cloth_iap_id[id_];
            iap = predefined_cloth_cost_currency[id_];
            break;
        case EConsumableType::TOKENPAK:
            item_type = "tokenpak";
            active = true;
            iap_id = tokenpak_iap_id[id_];
            iap = predefined_token_paks_cost_currency[id_];
            break;
        case EConsumableType::BUNDLE:
            item_type = "bundle";
            active = true;
            iap_id = bundle_iap_id[id_];
            iap = predefined_bundle_cost_currency[id_];
            break;
    }
    
    if (!active)
    {
        PRINTF((TEXT("In development")), Red)
        return;
    }
    
    if (iap)
    {
        #if PLATFORM_IOS
        gi->MakeInAppPurchaseV1(iap_id, type == EConsumableType::TOKENPAK, successful, unsuccessful);
        #elif PLATFORM_ANDROID
        gi->MakeInAppPurchaseV2(iap_id, type == EConsumableType::TOKENPAK, successful, unsuccessful);
        #endif
        return;
    }
    
    cool_json_t json;
    json["type"] = item_type;
    json["item_id"] = id_;
    auto callback = [successful, unsuccessful, id_, type, this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "shop/buy_item"))
        {
            if (resp_json.contains("bought") &&
                resp_json["bought"].get<bool> ())
            {
                switch(type)
                {
                    case EConsumableType::CUE:
                        gi->data->inventory->cue_durabilities[id_] = \
                        gi->data->shop->GetStaticCueInfo(id_).maxDurability;
                        break;
                    case EConsumableType::CHALK:
                        gi->data->inventory->chalk_durabilities[id_] = \
                        gi->data->shop->GetStaticChalkInfo(id_).maxDurability;
                        break;
                    case EConsumableType::CLOTH:
                        gi->data->inventory->cloth_ownership[id_] = true;
                        break;
                }
                successful.Execute();
            }
            else
            {
                unsuccessful.Execute();
            }
        }
        UPoolBaseShopWidget* shop = dynamic_cast<UPoolBaseShopWidget*>(gi->widgetManager->GetWidget("MainShop"));
        if (shop) shop->Refresh();
    };
    
    gi->MakeRequest("shop", endpoint, json, callback);
}
