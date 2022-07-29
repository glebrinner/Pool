// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPoolDataBlock.h"
#include "SefaPoolShop.h"
#include "SefaPoolPurchases.generated.h"

/**
 * 
 */
//enum class EConsumableType : uint8;

USTRUCT(BlueprintType)
struct FPurchasedItem
{
	GENERATED_USTRUCT_BODY()

	FString typeName;

	UPROPERTY(BlueprintReadOnly)
	EConsumableType itemType;
	UPROPERTY(BlueprintReadOnly)
	int item_id;
	UPROPERTY(BlueprintReadOnly)
	FDateTime timestamp;
};



UCLASS()
class SEFAPOOL_API USefaPoolPurchases : public USefaPoolDataBlock
{
	GENERATED_BODY()
public: 
	UPROPERTY(BlueprintReadOnly)
	TArray<FPurchasedItem> itemsPurchaseArray;

	virtual void Init(USefaPoolGameInstance* gi_);

	virtual void Save();

	virtual void Load();


	virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
		const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);

	UFUNCTION(BlueprintCallable)
	TArray<FPurchasedItem> GetPurchasedItemsFiltered(FString filter);
	
};
