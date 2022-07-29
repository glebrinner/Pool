// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <functional>
#include "Containers/Queue.h"
#include "SefaPool/Shared/SharedDefs.h"
#include "SefaPool/Shared/HTTP_GI.h"
#include "PoolServerGameInstance.h"
#include "GsAsyncQueueAllocator.generated.h"

USTRUCT()
struct FAllocationRequest
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY()
    FString service = TEXT("");
    
    UPROPERTY()
    FString endpoint = TEXT("");
    
    cool_json_t json;
    
    std::function<void(FString, int, int)> result_callback = nullptr;
};

UCLASS()
class SEFAPOOL_API UGsAsyncQueueAllocator : public UObject
{
	GENERATED_BODY()
    UPoolServerGameInstance* gi;
    
    int32 queue_size;
    
    int32 waiting_size;
    
    FTimerHandle requestAllocationTimer;
public:
    
    void Init(UPoolServerGameInstance* gi_);
    
    //UPROPERTY()
    TQueue<FAllocationRequest> allocation_queue;
    
    bool RequestAllocation(FAllocationRequest&& req);
    
    void ProcessSingleRequest();
    
    virtual void BeginDestroy() override;
};
