// Fill out your copyright notice in the Description page of Project Settings.


#include "RefereeContestBase.h"

void ARefereeContestBase::BeginPlay()
{
    Super::BeginPlay();
    
    // FILE with user info
    
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    
    if (srv_gi)
    {
        info = dynamic_cast<UBackendContestInfo*> (srv_gi->backend_data);
        if (info)
        {
            division = static_cast<int>(info->division);
            fee = info->fee;
            currentCompetitionId = info->contest_id;
        }
    }
    
    consolidation_event_name = TEXT("Match:contest:end");
}

int ARefereeContestBase::ContestId()
{
    return static_cast<int> (EContestType::CONTEST_NONE);
}

void ARefereeContestBase::GameResults()
{
    PRINTF((TEXT("GAME RESULTS")), White)
    Super::GameResults();
    if (currentCompetitionId == -1 || fee == 0) return;
    cool_json_t reg_result_json;
    
    if (info)
    {
        reg_result_json["user_id"] = info->player_info.user_id;
        reg_result_json.update(CreateConsolidationJsonForPlayer(info->player_info.user_id));
    }
    else
        PRINTF((TEXT("GI not found")), Red)
        
    
    reg_result_json["cont_id"]           = currentCompetitionId;
    reg_result_json["params"]["time"]    = GetCurrentTotalInGameTime();
    reg_result_json["params"]["balls"]   = CountOfPocketedBalls;
    if (!consolidation_json_data.empty())
        reg_result_json.update(consolidation_json_data);
    
    UE_LOG(LogTemp,
           Warning,
           TEXT("Sending request to api/consolidate_contest_match:\n%s"),
           *FString(reg_result_json.dump(2).c_str()))
    
    auto callback = [this](cool_json_t json) -> void {
            UE_LOG(LogTemp, Warning, TEXT("Processing reponse contest/reg_result."));
            if (!json.contains("status") || json["status"] != 200)
            {
                UE_LOG(LogTemp, Warning, TEXT("Bad status on contest/reg_result."));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("All good contest/reg_result."));
            }
    };
    
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (srv_gi)
    {
        cool_json_t event_json;
        event_json["event_name"] = consolidation_event_name;
        event_json["item"] = reg_result_json;

        srv_gi->MakeRequest("eventmanager", "register_event", event_json, callback);
        
        //srv_gi->MakeRequest("api", "consolidate_contest_match", reg_result_json, callback);
    }
}

void ARefereeContestBase::ProcessBase64Replay(const FString& replay)
{
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (srv_gi && info)
    {
        cool_json_t replay_request;
        replay_request["user_id"] = info->player_info.user_id;
        replay_request["encoded_replay"] = replay;
        replay_request["room"] = info->map;
        replay_request["referee"] = info->game_mode;
        replay_request["game_type"] = info->GetGameType();
        replay_request["info"] = info->GetInfoBundleJson();
        
        auto callback = [this](cool_json_t json) -> void {
                UE_LOG(LogTemp, Warning, TEXT("Processing reponse replay/store_contest_replay"));
                if (!json.contains("status") || json["status"] != 200)
                {
                    UE_LOG(LogTemp, Error, TEXT("Bad status on replay/store_contest_replay"));
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("All good replay/store_contest_replay"));
                }
        };
        
        srv_gi->MakeRequest("replay", "store_contest_replay", replay_request, callback);
    }
}

void ARefereeContestBase::UpdateContestPS()
{
    APoolContestPlayerState* PS_contest = Cast<APoolContestPlayerState>(PlayingPc->PlayerState);
    if (PS_contest)
    {
        PS_contest->pocketedBallsCount = CountOfPocketedBalls;
        PS_contest->inGameTime = GetCurrentTotalInGameTime();
        PS_contest->UpdateClient();
    }
}


void ARefereeContestBase::UpdateTargetableBalls(APoolPlayerStateBase* PlayerState)
{
    TArray<uint8> TargetableBalls;
    for (uint8 i = 0; i < BallCountInGame; ++i)
    {
        TargetableBalls.Add(i);
    }
    if (PlayerState)
    {
        PlayerState->ClientSetTargetableBalls(TargetableBalls);
    }
}


void ARefereeContestBase::OnGameStart()
{
    Super::OnGameStart();
    UpdateContestPS();
    APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(PlayingPc->PlayerState);
    UpdateTargetableBalls(PS);
}

void ARefereeContestBase::OnBallsStopped()
{
    Super::OnBallsStopped();
    UpdateContestPS();
}
void ARefereeContestBase::OnPlayTimeEnd()
{
    Super::OnPlayTimeEnd();
    UpdateContestPS();
}

void ARefereeContestBase::OnReactionTimeUpdate(float newRtime)
{
    APoolContestPlayerState* ps = Cast<APoolContestPlayerState>(PlayingPc->PlayerState);
    if (!ps) return;
    if (!backend_data) return;
    
    auto* pinfo = backend_data->GetPlayerInfo(ps->user_id);
    if (!pinfo) return;
    pinfo->shotsPlayed++;
    pinfo->totalReactionTime += newRtime;
}

