// Fill out your copyright notice in the Description page of Project Settings.


#include "Pool8BallPlayerController.h"
#include "Pool8BallGameMode.h"
#include "Pool8BallPlayerState.h"
#include "../Shared/ObjectGatherer.h"
#include "../Widgets/PoolMPUserBarWidget.h"
#include "SefaPool/Shared/PoolGameStateBase.h"

void APool8BallPlayerController::ServerPCReady_Implementation()
{
	if (!this || !objects->table)
		ERROR_DEBUG_PRINT_FUNC
		GetWorld()->GetAuthGameMode<APool8BallGameMode>()->PCReady(this);
}

void APool8BallPlayerController::CurrentFrameNextBallsToHit(TArray<int32>& arr)
{
	dynamic_cast<APool8BallPlayerState*>(PlayerState)->CurrentFrameNextBallsToHit(arr);
}

void APool8BallPlayerController::ActivateFoulIfNeeded()
{
    if (!PlayerState) return;
    APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*> (PlayerState);
    if (!ps) return;
    if (ps->ballInHand)
        objects->gameflow->SetBallInHand(true);
    else
        objects->gameflow->SetBallInHand(false);
}

void APool8BallPlayerController::UpdateUserBarWidget()
{
    bool ok = true;
    if (this && IsSpectator()) return;
    
    if (ok && !objects) ok = false;
    if (ok && !objects->hud) ok = false;
    if (ok && !objects->hud->userBarWidget) ok = false;
    UPoolMPUserBarWidget* wid = dynamic_cast<UPoolMPUserBarWidget*> (objects->hud->userBarWidget);
    if (ok && !wid) ok = false;
    if (!ok) return;
    
    if (ok && !PlayerState) ok = false;
    APool8BallPlayerState* this_ps = dynamic_cast<APool8BallPlayerState*> (PlayerState);
    if (ok && !this_ps) ok = false;
    
    if (!ok) return;
    
    APoolGameStateBase* gs = GetWorld()->GetGameState<APoolGameStateBase>();
    if (ok && !gs) ok = false;
    auto players = gs->PlayerArray;
    int i = 0;
    APool8BallPlayerState* that_ps = nullptr;
    while (ok && i < players.Num())
    {
        if (!dynamic_cast<APool8BallPlayerState*> (players[i])) i++;
        else if (players[i] == PlayerState) i++;
        else
        {
            that_ps = dynamic_cast<APool8BallPlayerState*> (players[i]);
            break;
        }
    }
    if (ok && !that_ps) ok = false;
    
    if (!ok) return;
    
    TArray<int32> thisNextBalls;
    this_ps->CurrentFrameNextBallsToHit(thisNextBalls);
    
    TArray<int32> thatNextBalls;
    that_ps->CurrentFrameNextBallsToHit(thatNextBalls);
    
    E_Pool8BallNextTarget thisNextTarget = this_ps->nextTarget;
    E_Pool8BallNextTarget thatNextTarget = that_ps->nextTarget;
    
    wid->UpdateUserTargetMode(thisNextTarget);
    wid->UpdateOpponentTargetMode(thatNextTarget);
    
    wid->UpdateUserTargetBalls(thisNextBalls);
    wid->UpdateOpponentTargetBalls(thatNextBalls);
}

APool8BallPlayerState* APool8BallPlayerController::GetOpponentPlayerState()
{
    bool ok = true;
    
    if (ok && !objects) ok = false;
    if (ok && !objects->hud) ok = false;
    if (ok && !objects->hud->userBarWidget) ok = false;
    UPoolMPUserBarWidget* wid = dynamic_cast<UPoolMPUserBarWidget*> (objects->hud->userBarWidget);
    if (ok && !wid) ok = false;
    if (!ok) return nullptr;
    
    if (ok && !PlayerState) ok = false;
    APool8BallPlayerState* this_ps = dynamic_cast<APool8BallPlayerState*> (PlayerState);
    if (ok && !this_ps) ok = false;
    
    if (!ok) return nullptr;
    
    APoolGameStateBase* gs = GetWorld()->GetGameState<APoolGameStateBase>();
    if (ok && !gs) ok = false;
    auto players = gs->PlayerArray;
    int i = 0;
    APool8BallPlayerState* that_ps = nullptr;
    while (ok && i < players.Num())
    {
        if (!dynamic_cast<APool8BallPlayerState*> (players[i])) i++;
        else if (players[i] == PlayerState) i++;
        else
        {
            that_ps = dynamic_cast<APool8BallPlayerState*> (players[i]);
            break;
        }
    }
    if (!ok && !that_ps) ok = false;
    
    if (!ok) return nullptr;
    return that_ps;
}

void APool8BallPlayerController::UploadOpponentInfo_Implementation(int cueNum,
                                                    FVector loc,
                                                    FRotator rot,
                                                    FVector cueLoc,
                                                    FRotator cueRot)
{
    if (!HasAuthority()) return;
    
    GetWorld()->GetAuthGameMode<APool8BallGameMode>()->BroadcastOpponentStatus(this,
                                                                               cueNum,
                                                                               loc,
                                                                               rot,
                                                                               cueLoc,
                                                                               cueRot);
    
}


void APool8BallPlayerController::UpdateOpponentStatus_Implementation(int cueNum,
                                                      FVector loc,
                                                      FRotator rot,
                                                      FVector cueLoc,
                                                      FRotator cueRot)
{
    if (!objects || !objects->opponent)
        return;
    objects->opponent->EnableAndUpdatePosition(cueNum, loc, rot, cueLoc, cueRot);
}


void APool8BallPlayerController::StopOpponentUpdate_Implementation()
{
    if (!objects || !objects->opponent)
        return;
    objects->opponent->Disable();
}

void APool8BallPlayerController::Client_UpdateActivePlayer_Implementation(uint8 ActivePlayer)
{
    UPoolMPUserBarWidget* Widget = Cast<UPoolMPUserBarWidget>(objects->hud->userBarWidget);
    if (Widget)
    {
        Widget->SetActivePlayer(ActivePlayer);
    }
    else
    {
        UE_LOG(LogTemp,Error,TEXT("Can't Set active widget invalid"))
    }
}

void APool8BallPlayerController::CueTimeHasEnded_Body()
{
    objects->gameflow->Spectating();
    playingPawn->Deactivate();
    nextState = GameFlowState_t::SPECTATING;
    GetWorldTimerManager().SetTimer(pendingStateChangeTimer,
                                    this,
                                    &APool8BallPlayerController::ProceedToNextStageIfVisualizationHasEnded,
                                    1.1f,
                                    true,
                                    0.5f);
}

void APool8BallPlayerController::UpdateClientBallPos_Implementation(const FBallsInfo& pos)
{
    if (!objects || !objects->ballinfo) return;
    objects->ballinfo->SingleBallPosUpdate(pos);
}


void APool8BallPlayerController::SetUsernames_Implementation(const FString& left, const FString& right)
{
    objects->hud->userBarWidget->UpdateUsername(left, false);
    objects->hud->userBarWidget->UpdateUsername(right, true);
}
