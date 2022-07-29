// Fill out your copyright notice in the Description page of Project Settings.


#include "Pool8BallReferee.h"

#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ShotInfo.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"
#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"

APool8BallReferee::APool8BallReferee()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APool8BallReferee::BeginPlay()
{
	Super::BeginPlay();
	objects->ReplayManager->InitializeGameMode(ERecordedGameMode::Pool8Ball);
	shotInfo.Clear();
    if (!dynamic_cast<UBackendFriendly1v1Info*> (backend_data))
        consolidation_event_name = TEXT("Match:1v1:end");
    else
        consolidation_event_name = TEXT("Match:friendly:end");
}

void APool8BallReferee::OnGameStart()
{
	Super::OnGameStart();
	shotInfo.openTable = true;
}


void APool8BallReferee::OnPlayTimeEnd()
{
    playingPcs[shotInfo.currentPlayerNum]->ShotTimeHasEnded();
    shotInfo.shotResult = E_ShotResult::FOUL;
    CheckRules_Finalize();
}

void APool8BallReferee::OnReactionTimeUpdate(float newRtime)
{
    if (!backend_data) return;
    
    auto* pinfo = backend_data->GetPlayerInfo(currentPlayers.ps->user_id);
    if (!pinfo) return;
    pinfo->shotsPlayed++;
    pinfo->totalReactionTime += newRtime;
}

void APool8BallReferee::ProcessBallPocketed(int num, bool first)
{

    currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                            MESSAGE_POCKETED_CUE_BALL + num);
    currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                  MESSAGE_OPPONENT_POCKETED_CUE_BALL + num);
    
    
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
	if (num == 0)
	{
		shotInfo.cuePocketed = true;
		gs->ReturnCueBall();
	}
	else
	{
		if (first) shotInfo.firstPocketedBall = num;
		gs->ballsInGame[num] = false;
		gs->StorePocketedBall(num);
        
		if (num == 8) shotInfo.blackPocketed = true;
		else
			if (num < 8) shotInfo.lowPocketed = true;
			else
				if (num > 8) shotInfo.highPocketed = true;
	}
}

void APool8BallReferee::ProcessBallKnockedOff(int num)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
    
    currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                            MESSAGE_KNOCKED_OFF_CUE_BALL + num);
    currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                  MESSAGE_OPPONENT_KNOCKED_OFF_CUE_BALL + num);
    
    
	if (num == 0)
	{
		gs->ReturnCueBall();
		shotInfo.cueKnocked = true;
	}
	else
	{
		gs->ReturnFoulBall(num);
	}
}

void APool8BallReferee::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
    
    for (int i = 0; i < hitLog.Num(); i++)
    {
        if (hitLog[i].obj1 == 0 && hitLog[i].obj2 != SIDERAIL_HIT && !shotInfo.cueBallFirstOtherBallHit)
            shotInfo.cueBallFirstOtherBallHit = hitLog[i].obj2;
        if (shotInfo.cueBallFirstOtherBallHit && hitLog[i].obj2 == SIDERAIL_HIT)
            shotInfo.numRailTouchesAfterFirstHit++;
    }
    
	shotInfo.readyToFinalize = true;
}

bool APool8BallReferee::CanPlayerShoot(APoolPlayerControllerBase* player)
{
	if (shotInfo.currentPlayerNum >= 0 &&
        shotInfo.currentPlayerNum <= 1 &&
        playingPcs[shotInfo.currentPlayerNum] == player)
        	return true;
	else
		return false;
}

void APool8BallReferee::NextShot(uint8 pc_num)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
	if (gs)
	{
		if (firstShot)
			gs->SetRuleset(RULES_BEHIND_HEADSTRING);
		else
			gs->SetRuleset(RULES_ANYWHERE);
	}
    APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
    APool8BallPlayerState* ps = Cast<APool8BallPlayerState>(pc->PlayerState);
    APool8BallPlayerController* other_pc = Cast<APool8BallPlayerController>(playingPcs[1 - shotInfo.currentPlayerNum]);
    APool8BallPlayerState* other_ps = Cast<APool8BallPlayerState>(other_pc->PlayerState);
    
    ps->shots++;
    
    currentPlayers = {pc, ps, other_pc, other_ps};
    
}

void APool8BallReferee::CheckRules()
{
    UE_LOG(LogTemp,Error, TEXT("%s: firstShot = %d"), *FString(__PRETTY_FUNCTION__), firstShot)
	if (firstShot)
		CheckRules_FirstShot();
	else
		CheckRules_NotFirstShot();
}

void APool8BallReferee::CheckRules_FirstShot()
{
    UE_LOG(LogTemp,Error, TEXT("%s: blackPocketed = %d"), *FString(__PRETTY_FUNCTION__), shotInfo.blackPocketed)
    
	E_ShotResult& shotResult = shotInfo.shotResult;
	if (shotInfo.blackPocketed)
	{
        UE_LOG(LogTemp,Error, TEXT("%s: cueKnocked = %d, cuePocketed = %d"), *FString(__PRETTY_FUNCTION__), shotInfo.cueKnocked, shotInfo.cuePocketed)
		if (shotInfo.cueKnocked || shotInfo.cuePocketed)
        {
            shotInfo.shotResult = E_ShotResult::LOSE;
            currentPlayers.ps->SendImmediateMessage(E_MessageSeverity::PRIMARY,
                                    MESSAGE_8BALL_UNLUCKY_LOSS);
            currentPlayers.other_ps->SendImmediateMessage(E_MessageSeverity::PRIMARY,
                                                          MESSAGE_8BALL_UNLUCKY_LOSS_OPPONENT);
        }
		else
        {
			shotInfo.shotResult = E_ShotResult::WIN;
            
            currentPlayers.ps->SendImmediateMessage(E_MessageSeverity::PRIMARY,
                                                    MESSAGE_8BALL_LUCKY_WIN);
            currentPlayers.other_ps->SendImmediateMessage(E_MessageSeverity::PRIMARY,
                                                          MESSAGE_8BALL_LUCKY_WIN_OPPONENT);
        }
	}
	else if (!shotInfo.blackPocketed)
	{
        UE_LOG(LogTemp,Error, TEXT("%s: cueBallFirstOtherBallHit = %d, numRailTouchesAfterFirstHit = %d,  cueKnocked = %d, cuePocketed = %d, lowPocketed = %d, highPocketed = %d"),
               *FString(__PRETTY_FUNCTION__),
               shotInfo.cueBallFirstOtherBallHit,
               shotInfo.numRailTouchesAfterFirstHit,
               shotInfo.cueKnocked,
               shotInfo.cuePocketed,
               shotInfo.lowPocketed,
               shotInfo.highPocketed)
		if (shotInfo.cueBallFirstOtherBallHit == 0)
        {
            shotInfo.shotResult = E_ShotResult::FOUL;
            currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                                    MESSAGE_8BALL_BREAK_NO_TOUCH_ON_BREAK);
            currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                                          MESSAGE_8BALL_BREAK_NO_TOUCH_ON_BREAK_OPPONENT);
        }
        else if (shotInfo.numRailTouchesAfterFirstHit < 3)
        {
            shotInfo.shotResult = E_ShotResult::FOUL;
            currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                               MESSAGE_8BALL_BREAK_3_RAILS);
            currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                                     MESSAGE_8BALL_BREAK_3_RAILS_OPPONENT);
        }
		else
			if (shotInfo.cuePocketed || shotInfo.cueKnocked)
				shotInfo.shotResult = E_ShotResult::FOUL;
			else
				if (shotInfo.lowPocketed || shotInfo.highPocketed)
					shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
				else
					shotInfo.shotResult = E_ShotResult::SHOT_LOST;
	}
}

void APool8BallReferee::CheckRules_NotFirstShot()
{
    UE_LOG(LogTemp,Error, TEXT("%s: blackPocketed = %d"), *FString(__PRETTY_FUNCTION__), shotInfo.blackPocketed)
	if (shotInfo.blackPocketed)
		CheckRules_NotFirstShot_BlackPocketed();
	else
		CheckRules_NotFirstShot_BlackNotPocketed();
}

void APool8BallReferee::CheckRules_NotFirstShot_BlackPocketed()
{
	APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
	APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*>(pc->PlayerState);

    UE_LOG(LogTemp,Error, TEXT("%s: blackTargeted = %d, cueKnocked = %d, cuePocketed = %d"),
           *FString(__PRETTY_FUNCTION__),
           ps->nextTarget == E_Pool8BallNextTarget::BLACK_BALL,
           shotInfo.cueKnocked,
           shotInfo.cuePocketed)
	if (ps->nextTarget != E_Pool8BallNextTarget::BLACK_BALL ||
        shotInfo.cueKnocked ||
        shotInfo.cuePocketed)
        	shotInfo.shotResult = E_ShotResult::LOSE;
	else
		shotInfo.shotResult = E_ShotResult::WIN;
}

void APool8BallReferee::CheckRules_NotFirstShot_BlackNotPocketed()
{
    UE_LOG(LogTemp,Error, TEXT("%s: cueBallFirstOtherBallHit = %d, numRailTouchesAfterFirstHit = %d, openTable = %d"),
           *FString(__PRETTY_FUNCTION__),
           shotInfo.cueBallFirstOtherBallHit,
           shotInfo.numRailTouchesAfterFirstHit,
           shotInfo.openTable)
	if (shotInfo.cueBallFirstOtherBallHit == 0)
    {
        shotInfo.shotResult = E_ShotResult::FOUL;
        currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                           MESSAGE_8BALL_NO_TOUCH);
        currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                                 MESSAGE_8BALL_NO_TOUCH_OPPONENT);
    }
    else if (shotInfo.cuePocketed || shotInfo.cueKnocked)
    {
        shotInfo.shotResult = E_ShotResult::FOUL;
    }
	else if (shotInfo.numRailTouchesAfterFirstHit == 0 &&
             !shotInfo.lowPocketed && !shotInfo.highPocketed)
    {
        shotInfo.shotResult = E_ShotResult::FOUL;
        currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                           MESSAGE_8BALL_NO_RAIL);
        currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                                 MESSAGE_8BALL_NO_RAIL_OPPONENT);
    }
	else if (shotInfo.openTable)
		CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit();
    
	else if (!shotInfo.openTable)
		CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit();
}

void APool8BallReferee::CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit()
{
    UE_LOG(LogTemp,Error, TEXT("%s: cueKnocked = %d, cuePocketed = %d, lowPocketed = %d, highPocketed = %d, firstPocketedBall = %d"),
           *FString(__PRETTY_FUNCTION__),
           shotInfo.cueKnocked,
           shotInfo.cuePocketed,
           shotInfo.lowPocketed,
           shotInfo.highPocketed,
           shotInfo.firstPocketedBall)

    if (shotInfo.lowPocketed || shotInfo.highPocketed)
    {
        if (shotInfo.firstPocketedBall > 0 && shotInfo.firstPocketedBall < 8)
        {
            currentPlayers.ps->ChangeNextTarget(E_Pool8BallNextTarget::LOW_BALLS);
            currentPlayers.other_ps->ChangeNextTarget(E_Pool8BallNextTarget::HIGH_BALLS);
            shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
            currentPlayers.ps->RegisterMessage(E_MessageSeverity::PRIMARY,
                                               MESSAGE_8BALL_SOLID);
            currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::PRIMARY,
                                                     MESSAGE_8BALL_STRIPED);
            shotInfo.openTable = false;
        }
        else
        if (shotInfo.firstPocketedBall > 8 && shotInfo.firstPocketedBall < 16)
        {
            currentPlayers.ps->ChangeNextTarget(E_Pool8BallNextTarget::HIGH_BALLS);
            currentPlayers.other_ps->ChangeNextTarget(E_Pool8BallNextTarget::LOW_BALLS);
            shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
            currentPlayers.ps->RegisterMessage(E_MessageSeverity::PRIMARY,
                                               MESSAGE_8BALL_STRIPED);
            currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::PRIMARY,
                                                     MESSAGE_8BALL_SOLID);
            shotInfo.openTable = false;
        }
    }
    else
        if (!shotInfo.lowPocketed && !shotInfo.highPocketed)
            shotInfo.shotResult = E_ShotResult::SHOT_LOST;
}

void APool8BallReferee::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit()
{
    
    UE_LOG(LogTemp,Error, TEXT("%s: lowTargeted = %d, highTargeted = %d, blackTargeted = %d"),
           *FString(__PRETTY_FUNCTION__),
           currentPlayers.ps->nextTarget == E_Pool8BallNextTarget::LOW_BALLS,
           currentPlayers.ps->nextTarget == E_Pool8BallNextTarget::HIGH_BALLS,
           currentPlayers.ps->nextTarget == E_Pool8BallNextTarget::BLACK_BALL)

	if (currentPlayers.ps->nextTarget == E_Pool8BallNextTarget::LOW_BALLS)
	{
		CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted();
	}
	else if (currentPlayers.ps->nextTarget == E_Pool8BallNextTarget::HIGH_BALLS)
	{
		CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted();
	}
	else if (currentPlayers.ps->nextTarget == E_Pool8BallNextTarget::BLACK_BALL)
	{
		CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted();
	}
	else
	{
		ERROR_DEBUG_PRINT_FUNC_INFO("Invalid situation occurred")
	}
}

void APool8BallReferee::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted()
{
    
    UE_LOG(LogTemp,Error, TEXT("%s: cueBallFirstOtherBallHit = %d, lowPocketed = %d"),
           *FString(__PRETTY_FUNCTION__),
           shotInfo.cueBallFirstOtherBallHit,
           shotInfo.lowPocketed)
    
	if (!(shotInfo.cueBallFirstOtherBallHit > 0 && shotInfo.cueBallFirstOtherBallHit < 8))
    {
        shotInfo.shotResult = E_ShotResult::FOUL;
        currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                           MESSAGE_8BALL_SOLID_NO_CONTACT);
        currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                                 MESSAGE_8BALL_SOLID_NO_CONTACT_OPPONENT);
    }
	else if (shotInfo.lowPocketed)
		shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
	else
		shotInfo.shotResult = E_ShotResult::SHOT_LOST;
}

void APool8BallReferee::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted()
{
    UE_LOG(LogTemp,Error, TEXT("%s: cueBallFirstOtherBallHit = %d, highPocketed = %d"),
           *FString(__PRETTY_FUNCTION__),
           shotInfo.cueBallFirstOtherBallHit,
           shotInfo.highPocketed)
    
	if (!(shotInfo.cueBallFirstOtherBallHit > 8 && shotInfo.cueBallFirstOtherBallHit < 16))
    {
        shotInfo.shotResult = E_ShotResult::FOUL;
        currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                           MESSAGE_8BALL_STRIPED_NO_CONTACT);
        currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                                 MESSAGE_8BALL_STRIPED_NO_CONTACT_OPPONENT);
    }
	else if (shotInfo.highPocketed)
		shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
	else
		shotInfo.shotResult = E_ShotResult::SHOT_LOST;
}

void APool8BallReferee::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted()
{
    UE_LOG(LogTemp,Error, TEXT("%s: cueBallFirstOtherBallHit = %d"),
        *FString(__PRETTY_FUNCTION__),
        shotInfo.cueBallFirstOtherBallHit)
    
	if (shotInfo.cueBallFirstOtherBallHit != 8)
    {
        shotInfo.shotResult = E_ShotResult::FOUL;
        currentPlayers.ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                           MESSAGE_8BALL_BLACK_NO_CONTACT);
        currentPlayers.other_ps->RegisterMessage(E_MessageSeverity::SECONDARY,
                                                 MESSAGE_8BALL_BLACK_NO_CONTACT_OPPONENT);
    }
	else
		shotInfo.shotResult = E_ShotResult::SHOT_LOST;
}

bool APool8BallReferee::IsReadyToFinalize()
{
	return shotInfo.readyToFinalize;
}

void APool8BallReferee::CheckRules_Finalize()
{
    
    currentPlayers.ps->Server_DeployMessages();
    currentPlayers.other_ps->Server_DeployMessages();
    
    switch (shotInfo.shotResult)
    {
        case E_ShotResult::WIN:
            UE_LOG(LogTemp,Error, TEXT("%s: %s WIN"),
                   *FString(__PRETTY_FUNCTION__), *currentPlayers.ps->username)
            break;
        case E_ShotResult::LOSE:
            UE_LOG(LogTemp,Error, TEXT("%s: %s LOSE"),
                   *FString(__PRETTY_FUNCTION__), *currentPlayers.ps->username)
            break;
        case E_ShotResult::SHOT_RETAINED:
            UE_LOG(LogTemp,Error, TEXT("%s: %s SHOT_RETAINED"),
                   *FString(__PRETTY_FUNCTION__), *currentPlayers.ps->username)
            break;
        case E_ShotResult::SHOT_LOST:
            UE_LOG(LogTemp,Error, TEXT("%s: %s SHOT_LOST"),
                   *FString(__PRETTY_FUNCTION__), *currentPlayers.ps->username)
            break;
        case E_ShotResult::FOUL:
            UE_LOG(LogTemp,Error, TEXT("%s: %s FOUL"),
                   *FString(__PRETTY_FUNCTION__), *currentPlayers.ps->username)
            break;
    }

	int8 nextPlayer = -1;

	if (firstShot) firstShot = false;
	if (shotInfo.shotResult == E_ShotResult::WIN)
	{
		if (!bIsTeamMode)
		{
			win_user_id_ = currentPlayers.ps->user_id;
			lose_user_id_ = currentPlayers.other_ps->user_id;
                        currentPlayers.pc->ClientTravelToLevel(win_game_level, win_game_widget);
                        currentPlayers.other_pc->ClientTravelToLevel(lose_game_level, lose_game_widget);
            
                        if (currentPlayers.other_ps->shots == 0)
                            is_firm_break = true;
                        if (currentPlayers.other_ps->shots == 1)
                            is_firm_replay = true;
		}
		else
		{
			TeamTravelToLoseScreen(1 - shotInfo.currentPlayerNum);
			TeamTravelToWinScreen(shotInfo.currentPlayerNum);
		}
		GameResults();
		UE_LOG(LogTemp,Error,TEXT("Win"),nextPlayer)
		
	}
	else if (shotInfo.shotResult == E_ShotResult::LOSE)
	{
		if (!bIsTeamMode)
		{
			win_user_id_ = currentPlayers.other_ps->user_id;
			lose_user_id_ = currentPlayers.ps->user_id;
                        currentPlayers.other_pc->ClientTravelToLevel(win_game_level, win_game_widget);
                        currentPlayers.pc->ClientTravelToLevel(lose_game_level, lose_game_widget);
            
                        if (currentPlayers.ps->shots == 0)
                            is_firm_break = true;
                        if (currentPlayers.ps->shots == 1)
                            is_firm_replay = true;
		}
		else
		{
			TeamTravelToLoseScreen(shotInfo.currentPlayerNum);
			TeamTravelToWinScreen(1- shotInfo.currentPlayerNum);
		}
		GameResults();
		UE_LOG(LogTemp,Error,TEXT("Lose"),nextPlayer)
	}
	else if (shotInfo.shotResult == E_ShotResult::SHOT_RETAINED)
	{
		nextPlayer = shotInfo.currentPlayerNum;
        currentPlayers.ps->ballInHand = false;
        currentPlayers.other_ps->ballInHand = false;
	}
	else if (shotInfo.shotResult == E_ShotResult::SHOT_LOST)
	{
		nextPlayer = 1 - shotInfo.currentPlayerNum;
        currentPlayers.ps->ballInHand = false;
        currentPlayers.other_ps->ballInHand = false;
		UE_LOG(LogTemp,Error,TEXT("Shot Lost"),nextPlayer)
	}
	else if (shotInfo.shotResult == E_ShotResult::FOUL)
	{
		nextPlayer = 1 - shotInfo.currentPlayerNum;
        currentPlayers.ps->ballInHand = false;
        currentPlayers.other_ps->ballInHand = true;
		UE_LOG(LogTemp,Error,TEXT("Foul"),nextPlayer)
	}
    currentPlayers.ps->UpdatePocketed();
    currentPlayers.other_ps->UpdatePocketed();
    currentPlayers.ps->UpdateClient();
    currentPlayers.other_ps->UpdateClient();
	UE_LOG(LogTemp,Error,TEXT("PrevPlayer: %i, NextPlayer: %i"), shotInfo.currentPlayerNum, nextPlayer)
    
    shotInfo.readyToFinalize = false;
    
	if (nextPlayer != -1)
	{
		APool8BallGameMode* GM = Cast<APool8BallGameMode>(GetWorld()->GetAuthGameMode());

		if (!bIsTeamMode)
		{
			GM->NextShot(nextPlayer);
		}
		else
		{
			NextTeamShot(nextPlayer);
		}
	}
}

void APool8BallReferee::GameResults()
{
    Super::GameResults();
    if (backend_data)
        RequestConsolidateMatch(win_user_id_, lose_user_id_);
}

void APool8BallReferee::ProcessBase64Replay(const FString& replay)
{
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi) return;
    UBackend1v1Info* backend_data_1v1 = Cast<UBackend1v1Info> (backend_data);
    if (!backend_data_1v1) return;
    APool8BallGameMode* GM = Cast<APool8BallGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM || GM->bIsTeamMode)
        return;
    
    cool_json_t replay_request;
    replay_request["user1_id"] = backend_data_1v1->player_info[0].user_id;
    replay_request["user2_id"] = backend_data_1v1->player_info[1].user_id;
    replay_request["encoded_replay"] = replay;
    replay_request["room"] = backend_data_1v1->map;
    replay_request["referee"] = backend_data_1v1->game_mode;
    replay_request["game_type"] = backend_data_1v1->GetGameType();
    replay_request["info"] = backend_data_1v1->GetInfoBundleJson();
    
    auto callback = [this](cool_json_t json) -> void {
            UE_LOG(LogTemp, Warning, TEXT("Processing reponse replay/store_1v1_replay"));
            if (!json.contains("status") || json["status"] != 200)
            {
                UE_LOG(LogTemp, Error, TEXT("Bad status on replay/store_1v1_replay"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("All good replay/store_1v1_replay"));
            }
    };
    
    gi->MakeRequest("replay", "store_1v1_replay", replay_request, callback);
}

void APool8BallReferee::PlayerExited(FString exiting_user_id) {
    
    UBaseBackend1v1Info* backend_data_1v1 = Cast<UBaseBackend1v1Info> (backend_data);
    if (backend_data_1v1)
    {
		int lose_index = backend_data_1v1->player_info[1].user_id == exiting_user_id;
		win_user_id_ = backend_data_1v1->player_info[1 - lose_index].user_id;
		lose_user_id_ = backend_data_1v1->player_info[lose_index].user_id;
		GameResults();
    }
    UE_LOG(LogTemp, Warning, TEXT("Traveling win player to : %s."), *win_game_level.ToString());
	if (currentPlayers.ps->user_id != exiting_user_id)
        currentPlayers.pc->ClientTravelToLevel(win_game_level, win_game_widget);
	if (currentPlayers.other_ps->user_id != exiting_user_id)
        currentPlayers.other_pc->ClientTravelToLevel(win_game_level, win_game_widget);

}

void APool8BallReferee::RequestConsolidateMatch(FString win_user_id, FString lose_user_id)
{
	UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi) return;
    UBackend1v1Info* backend_data_1v1 = Cast<UBackend1v1Info> (backend_data);
    if (!backend_data_1v1) return;
	UE_LOG(LogTemp, Warning, TEXT("Start distributing funds."));
	cool_json_t consolidate_json;
	consolidate_json["amount"] = backend_data_1v1->amount;
	consolidate_json["match_id"] = backend_data_1v1->match_id;
	consolidate_json["win_user_id"] = CreateConsolidationJsonForPlayer(win_user_id);
	consolidate_json["lose_user_id"] = CreateConsolidationJsonForPlayer(lose_user_id);
    consolidate_json["firm_break"] = is_firm_break;
    consolidate_json["firm_replay"] = is_firm_replay;
    if (!consolidation_json_data.empty())
        consolidate_json.update(consolidation_json_data);

	UE_LOG(LogTemp, Warning, TEXT("Win player user id: %s."), *consolidate_json["win_user_id"]["user_id"].get<FString>());
	UE_LOG(LogTemp, Warning, TEXT("Lose player user id: %s."), *consolidate_json["lose_user_id"]["user_id"].get<FString>());

	auto succesful = [](cool_json_t json) {
		if (!json.contains("status") || json["status"].get<int>() != 200)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bad status on issue_match_result_from_reserved_funds."));
		}
		else
		{
            /*if (!json.contains("match"))
            {
                UE_LOG(LogTemp, Warning, TEXT("No match in api/consolidate_match response"));
            }
            else
            if (!json["match"].contains("status") ||
                json["match"]["status"].get<int>() != 200)
            {
                UE_LOG(LogTemp, Warning, TEXT("Bad status in api/consolidate_match[\"match\"]"));
            }
            else
			if (!json["match"].contains("reserved_ok") ||
                !json["match"]["reserved_ok"].get<bool>())
			{
                UE_LOG(LogTemp, Warning, TEXT("Bad reserved in api/consolidate_match[\"match\"]"));
			}
            else
            if (!json["match"].contains("completed") ||
                !json["match"]["completed"].get<bool>())
            {
                UE_LOG(LogTemp, Warning, TEXT("Bad completed in api/consolidate_match[\"match\"]"));
            }*/
		}
	};
	auto unsuccesful = []() {
		UE_LOG(LogTemp, Warning, TEXT("Request DistributeFunds wasn't succesful"));
	};
    
    cool_json_t event_json;
    event_json["event_name"] = consolidation_event_name;
    event_json["item"] = consolidate_json;

	gi->MakeRequestWithFail("eventmanager", "register_event", event_json, succesful, unsuccesful);
    
    //cool_json_t match_ended_json;
    //match_ended_json["match_id"] = backend_data_1v1->match_id;
    //gi->MakeRequest("core", "internal/match_has_ended", match_ended_json, [](cool_json_t) {UE_LOG(LogTemp, Warning, TEXT("core match has started responded"))});
}

