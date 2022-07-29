// Fill out your copyright notice in the Description page of Project Settings.


#include "GsAsyncQueueAllocator.h"

void UGsAsyncQueueAllocator::Init(UPoolServerGameInstance* gi_)
{
    gi = gi_;
    queue_size = 0;
}

#define LOG_SIZES \
UE_LOG(LogTemp, Warning, TEXT("UGsAsyncQueueAllocator::RequestAllocation queue %d waiting %d"), queue_size, waiting_size)


bool UGsAsyncQueueAllocator::RequestAllocation(FAllocationRequest&& req)
{
    if (req.service.Len() == 0 ||
        req.endpoint.Len() == 0 ||
        !req.result_callback)
        return false;
    
    if (queue_size == 0)
    {
        gi->TimerManager->SetTimer(requestAllocationTimer, this,
                                   &UGsAsyncQueueAllocator::ProcessSingleRequest, 0.5f, true, 0.5f);
    }
    allocation_queue.Enqueue(req);
    queue_size++;
    LOG_SIZES
    return true;
}
 

void UGsAsyncQueueAllocator::ProcessSingleRequest()
{
    if (!gi) return;
    if (waiting_size >= ALLOCATION_REQUEST_CONCURRENCY) return;
    
    if (queue_size == 0 || allocation_queue.IsEmpty())
    {
        queue_size = 0;
        allocation_queue.Empty();
        return;
    }
    
    FAllocationRequest* request = new FAllocationRequest;
    allocation_queue.Dequeue(*request);
    queue_size--;
    if (queue_size == 0)
        gi->TimerManager->ClearTimer(requestAllocationTimer);
    waiting_size++;
    LOG_SIZES
    UE_LOG(LogTemp, Warning, TEXT("UGsAsyncQueueAllocator::RequestAllocation waiting size %d"),
           queue_size)
    
    gi->MakeRequestWithFail(request->service,
                            request->endpoint,
                            request->json,
        [request, this](cool_json_t resp) {
            waiting_size--;
            LOG_SIZES
            if (resp["status"] != 200) {
                UE_LOG(LogTemp, Warning, TEXT("UGsAsyncQueueAllocator::ProcessSingleRequest::callback Request internal/begin_type_1vs1  wasn't succesful status_code: %d"), resp["status"].get<int>());
                RequestAllocation(std::move(*request));
            }
            else {
                if (!resp.contains("ip") ||
                    !resp.contains("port") ||
                    !resp.contains("match_id"))
                {
                    UE_LOG(LogTemp,
                           Warning,
                           TEXT("UGsAsyncQueueAllocator::ProcessSingleRequest::callback bad json (expected ip, port and match_id) %s"), resp.dump().c_str());
                    RequestAllocation(std::move(*request));
                }
                else
                {
                    int port = resp["port"].get<int>();
                    FString ip = resp["ip"].get<FString>();
                    int match_id = resp["match_id"].get<int>();
                    request->result_callback(ip, port, match_id);
                }
            }
            delete request;
        },
        [request, this]() {
            waiting_size--;
            UE_LOG(LogTemp, Warning, TEXT("UGsAsyncQueueAllocator::ProcessSingleRequest::callback Request internal/begin_type_1vs1 failed"));
            RequestAllocation(std::move(*request));
            delete request;
        });
}

void UGsAsyncQueueAllocator::BeginDestroy()
{
    Super::BeginDestroy();
    if (!gi) return;
    gi->TimerManager->ClearTimer(requestAllocationTimer);
}
