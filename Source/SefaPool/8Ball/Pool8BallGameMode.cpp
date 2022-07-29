
#include "Pool8BallGameMode.h"

#include "../Shared/ObjectGatherer.h"
#include "Dom/JsonValue.h"
#include "../Shared/PoolGameStateBase.h"
#include "SefaPool/8Ball/Referee/MPRefereeBase.h"
#include "SefaPool/8Ball/Referee/Pool8BallReferee.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "SefaPool/Shared/MainSpectatorPawn.h"
#include "SefaPool/Shared/PoolSpectatorPlayerController.h"

#include "AgonesComponent.h"

CurrentShotDetails::CurrentShotDetails()
{
    Clear();
}

void CurrentShotDetails::Clear()
{
    lowPocketed = false;
    highPocketed = false;
    cuePocketed = false;
    cueKnocked = false;
    blackPocketed = false;
    foul = false;
    firstPocketedBall = -1;
    currentPlayerNum = -1;
    cueBallFirstOtherBallHit = 0;
    numRailTouchesAfterFirstHit = 0;
    shotResult = E_ShotResult::UNDETERMINED;
    readyToFinalize = false;
}


APool8BallGameMode::APool8BallGameMode() :
//#if UE_SERVER
    AgonesSDK(nullptr),
//#endif
    registeredPlayers(0),
    playingPcs(),
    readyPCs(),
    firstShot(true),
    shotInfo(),
    logged_out(),
    objects()
{
    GameStateClass = APoolGameStateBase::StaticClass();
    PlayerControllerClass = APool8BallPlayerController::StaticClass();
    PlayerStateClass = APool8BallPlayerState::StaticClass();
    DefaultPawnClass = APoolPlayer::StaticClass();
    HUDClass = APoolHUD::StaticClass();
    SpectatorClass = AMainSpectatorPawn::StaticClass();
#if UE_SERVER
    AgonesSDK = CreateDefaultSubobject<UAgonesComponent>(TEXT("AgonesSDK"));
    FNetworkVersion::IsNetworkCompatibleOverride.BindLambda([](uint32 LocalNetworkVersion, uint32 RemoteNetworkVersion)
    {
        return true;
    });

#endif
    PrimaryActorTick.bCanEverTick = true;
    bIsTeamMode = false;
    gameHasStarted = false;
}

void APool8BallGameMode::AllocateAgones(const FGameServerResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocating game server"));
    FAgonesErrorDelegate error_del;
    error_del.BindUFunction(this, "AgonesError");
    FAllocateDelegate success_del;
    success_del.BindUFunction(this, "AgonesSuccess");
    AgonesSDK->Allocate(success_del, error_del);
}

void APool8BallGameMode::AgonesSuccess(const FEmptyResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocation was succesful"));
}

void APool8BallGameMode::AgonesError(const FAgonesError& Response) {
    UE_LOG(LogTemp, Error, TEXT("%s"), *Response.ErrorMessage);
}

void APool8BallGameMode::BeginPlay()
{
    Super::BeginPlay();
#if UE_SERVER
    AgonesSDK->Allocate({}, {});
    FScriptDelegate agones_reply_delegat;
    agones_reply_delegat.BindUFunction(this, "AllocateAgones");
    AgonesSDK->ConnectedDelegate.AddUnique(agones_reply_delegat);
#endif
    GATHER_OBJECT_GATHERER(objects)
    UE_LOG(LogTemp,Error,TEXT("APool8BallGameMode::BeginPlay"))
    // EnableTeamMode();
    readyPCs[0] = readyPCs[1] = readyPCs[2] = readyPCs[3] = false;
    registeredPlayers = 0;
}

void APool8BallGameMode::SetSpecPawn()
{
    for (int32 i = 0; i < Spectators.Num(); ++i)
    {
        APoolPlayerControllerBase* PC = Cast<APoolPlayerControllerBase>(Spectators[i]);
        if (PC)
        {
            PC->ClientSetViewToTable();
            PC->SetIsSpectator();
            AMainSpectatorPawn* SPawn = GetWorld()->SpawnActor<AMainSpectatorPawn>(AMainSpectatorPawn::StaticClass());
            PC->SetPawn(SPawn);
        }
        // UE_LOG(LogTemp,Warning,TEXT("Spec pawn seted %s"),*SpecControllers[i]->GetPawn()->GetName())
    }
}


void APool8BallGameMode::PreLogin(const FString& Options,
                                  const FString& Address,
                                  const FUniqueNetIdRepl& UniqueId,
                                  FString& ErrorMessage)
{
    int32 Limit = bIsTeamMode ? 4 : 2;
    if (!this/* || registeredPlayers == Limit*/)
    {
        ErrorMessage = TEXT("Error occurred, login deprecated");
        // ERROR_DEBUG_PRINT_FUNC
    }
#if UE_SERVER
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi)
    {
        ErrorMessage = TEXT("Game instance not found");
    }
    /*FString user_id = UGameplayStatics::ParseOption(Options, "user_id");
    if (gi && !gi->backend_data->GetPlayerInfo(user_id))
    {
        ErrorMessage = TEXT("Incorrect user_id");
    }*/
#endif
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* APool8BallGameMode::Login(UPlayer* NewPlayer,
    ENetRole InRemoteRole,
    const FString& Portal,
    const FString& Options,
    const FUniqueNetIdRepl& UniqueId,
    FString& ErrorMessage)
{
    UE_LOG(LogTemp, Warning, TEXT("Login"));
    int32 PlayerCount = bIsTeamMode ? 4 : 2;
    if (!this)
        ERROR_DEBUG_PRINT_FUNC_RET(nullptr)
    APlayerController* pc = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
    Cast<APoolPlayerControllerBase>(pc)->Options_ = Options;
    
    return pc;
}

void APool8BallGameMode::PostLogin(APlayerController* NewPlayer)
{
    UE_LOG(LogTemp,Error,TEXT("APool8BallGameMode::PostLogin: %i"),registeredPlayers)
    if (Referee)
    {
        UE_LOG(LogTemp,Error,TEXT("REFEREE CLASS IS VALID"))
    }
    else
    {
        UE_LOG(LogTemp,Error,TEXT("REFEREE CLASS NOT VALID"))
    }
    
        // ERROR_DEBUG_PRINT_FUNC
    Super::PostLogin(NewPlayer);
    APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(NewPlayer);
    APool8BallPlayerState* ps = Cast<APool8BallPlayerState>(pc->PlayerState);
    int32 PlayerCount = bIsTeamMode ? 4 : 2;
    
#if UE_SERVER
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi)
        return;
    FString user_id = UGameplayStatics::ParseOption(pc->Options_, "user_id");
    if (!gi->backend_data->GetPlayerInfo(user_id))
#else
    if (!this || registeredPlayers >= PlayerCount)
#endif
    {
        // NewPlayer->StartSpectatingOnly();
        // NewPlayer->GetPawnOrSpectator()->Destroy();
        // APoolPlayerControllerBase* PC = Cast<APoolPlayerControllerBase>(NewPlayer);
        // if (PC)
        // {
        //     PC->ClientSetViewToTable();
        // }
        // APawn* SpecPawn = GetWorld()->SpawnActor<APawn>(AMainSpectatorPawn::StaticClass());
        // NewPlayer->SetPawn(nullptr);
        // // NewPlayer->Possess(SpecPawn);
        UE_LOG(LogTemp,Error,TEXT("Spectator joined"))
        APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
        if (GS) GS->SpectatorCount++;
        FTimerHandle TimerHandle_Test;
        Spectators.Add(Cast<APool8BallPlayerController>(NewPlayer));
        pc->SetIsSpectator();
        pc->ForceNetUpdate();
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_Test,this,&APool8BallGameMode::SetSpecPawn,0.2f);
        // UE_LOG(LogTemp, Error, TEXT("Created spectator class %s"), *GetNameSafe(NewPlayer->GetPawn()))
    }
    else
    {
        APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
        if (GS)
        {
            switch (registeredPlayers)
            {
                case 0:
                    GS->FirstPlayerPS = Cast<APool8BallPlayerState>(NewPlayer->PlayerState);
                    GS->ForceNetUpdate();
                    break;
                case 1:
                    GS->SecondPlayerPS = Cast<APool8BallPlayerState>(NewPlayer->PlayerState);
                    GS->ForceNetUpdate();
                    break;
                default:
                    break;
            }
        }
    }
    #if UE_SERVER
    ps->user_id = (UGameplayStatics::ParseOption(pc->Options_, "user_id"));
    #endif
    if (Referee)
    {
        Referee->playingPcs[registeredPlayers] = playingPcs[registeredPlayers] = pc;
        UE_LOG(LogTemp,Error,TEXT("login valid"))
    }else UE_LOG(LogTemp,Error,TEXT("Referee post login invalid"))
    if (!playingPcs[registeredPlayers])
        ERROR_DEBUG_PRINT_FUNC_INFO("Wrong player controller type received")
    else
        registeredPlayers++;
    UE_LOG(LogTemp,Error,TEXT("registeredPlayers: %i"),registeredPlayers)
}

void APool8BallGameMode::Logout(AController* Exiting)
{
    UE_LOG(LogTemp, Warning, TEXT("Player Logout"));
    if (!this || registeredPlayers == 0)
        ERROR_DEBUG_PRINT_FUNC;
    auto pc_exiting = Cast<APool8BallPlayerController>(Exiting);
    if (pc_exiting != playingPcs[0] &&
        pc_exiting != playingPcs[1])
    {
        int32 Idx = Spectators.Remove(Cast<APool8BallPlayerController>(Exiting));
        if (Idx)
        {
            APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
            if (GS) GS->SpectatorCount--;
            Super::Logout(Exiting);
            return;
        }
    }
    if (registeredPlayers >= 2)
    {
        APool8BallPlayerController* other_pc = (pc_exiting == (playingPcs[0])) ? playingPcs[1] : playingPcs[0];
        // If the game is still going, tell the other pc that the second one left
        // If not, ignore


        if (dynamic_cast<AMPRefereeBase*>(Referee)->shotInfo.shotResult != E_ShotResult::LOSE &&
            dynamic_cast<AMPRefereeBase*>(Referee)->shotInfo.shotResult != E_ShotResult::WIN)
        {
            if (!IsValid(pc_exiting) ||
                !IsValid(other_pc))
            {
                UE_LOG(LogTemp, Warning, TEXT("Player controller is already not valid."))
            }
            else
            {
                APoolPlayerStateBase* other_ps = Cast<APoolPlayerStateBase>(other_pc->PlayerState);
                UPoolServerGameInstance* gi = Cast<UPoolServerGameInstance>(
                    (UGameplayStatics::GetGameInstance(GetWorld())));
                if (gi)
                {
                    UBaseBackend1v1Info* backend_data = Cast<UBaseBackend1v1Info>(gi->backend_data);
                    FString exiting_user_id = backend_data->player_info[1].user_id == other_ps->user_id
                                                  ? backend_data->player_info[0].user_id
                                                  : backend_data->player_info[1].user_id;
                    if (Cast<APool8BallReferee>(Referee))
                        Cast<APool8BallReferee>(Referee)->PlayerExited(exiting_user_id);
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("GM::Logout Referee wasn't valid."));
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Game is already consolidated."))
        }
    }
#if UE_SERVER
    //AgonesSDK->Shutdown({}, {});
    //FGenericPlatformMisc::RequestExit(false);
    FTimerHandle timer;
    GetWorld()->GetTimerManager().SetTimer(timer, this, &APool8BallGameMode::ExitServer, 90, false);
#endif
    Super::Logout(Exiting);
}

void APool8BallGameMode::ExitServer() {
#if UE_SERVER
    UE_LOG(LogTemp, Error, TEXT("Shutting down"))
        AgonesSDK->Shutdown({}, {});
    FGenericPlatformMisc::RequestExit(false);
#endif
}

void APool8BallGameMode::PCReady(APoolPlayerControllerBase* playerBase)
{
    UE_LOG(LogTemp,Error,TEXT("PC READY CALLED BY %s"),*playerBase->GetName())
    
    APool8BallPlayerController* player = dynamic_cast<APool8BallPlayerController*> (playerBase);
    
    if (!this || !player)
        ERROR_DEBUG_PRINT_FUNC;
    if (player == playingPcs[0])
    {
        UE_LOG(LogTemp, Error, TEXT("Find player 0"));
        readyPCs[0] = true;
    }
    if (player == playingPcs[1])
    {
        UE_LOG(LogTemp, Error, TEXT("Find player 1"));
        readyPCs[1] = true;
    }
    if (player == playingPcs[2])
        readyPCs[2] = true;
    if (player == playingPcs[3])
        readyPCs[3] = true;

    APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
    if (gs)
        gs->objects->shotinfo->SetNetworked(true);

    player->ClientSetBallManagerSettings(Referee->FirstBallIndex, Referee->BallCountInGame);
    APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*> (player->PlayerState);
    
    if (player->IsSpectator())
    {
        objects->ballinfo->UpdateBallPosOnSpecificClient(player);
        APool8BallPlayerController* sp_pc = Cast<APool8BallPlayerController> (player);
        
        if (gameHasStarted && sp_pc)
            sp_pc->SetUsernames(Cast<APool8BallPlayerState> (playingPcs[0]->PlayerState)->username,
                                Cast<APool8BallPlayerState> (playingPcs[1]->PlayerState)->username);
        return;
    }
    
//#if UE_SERVER
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (ps && gi && !player->IsSpectator())
    {
        UBaseBackend1v1Info* b1v1i = dynamic_cast<UBaseBackend1v1Info*> (gi->backend_data);
        if (b1v1i)
        {
            UE_LOG(LogTemp, Warning, TEXT("APool8BallGameMode::PCReady player with UserId %s"), *ps->user_id);

            FBackendPlayerInfo* pi_ = b1v1i->GetPlayerInfo(ps->user_id);
            if (!pi_)
            {
                UE_LOG(LogTemp, Error, TEXT("APool8BallGameMode::PCReady UserId %s not found (1v1)"), *ps->user_id);
            }
            TArray<int32> cue_durabilities, chalk_durabilities;

            EDifficultyLevels diff = b1v1i->difficulty;
            if (diff == EDifficultyLevels::DIFFICULTY_LEVEL_NONE)
                diff = EDifficultyLevels::DIFFICULTY_LEVEL_6;

            cue_durabilities = pi_->durability.cue_durabilities;
            chalk_durabilities = pi_->durability.chalk_durabilities;
            ps->username = pi_->username;
            ps->InitClient(cue_durabilities,
                chalk_durabilities,
                pi_->cur_cue,
                pi_->cur_chalk,
                diff);
            ps->ForceNetUpdate();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("APool8BallGameMode::PCReady Couldn't cast backend_info"));
            UE_LOG(LogTemp, Error, TEXT("APool8BallGameMode::PCReady Couldn't cast backend_info"));
        }


    }
    else if (ps && !player->IsSpectator()) // Testing locally through PIE
    {
        auto GameInstance = Cast<USefaPoolGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        TArray<int32> cue_durabilities, chalk_durabilities;
        int current_cue = 1, current_chalk = 1;
        if (GameInstance && GameInstance->data->inventory->cue_durabilities.Num() != 0)
        {
            cue_durabilities = GameInstance->data->inventory->cue_durabilities;
            current_chalk = GameInstance->data->inventory->current_chalk;
            PRINTF((TEXT("Entry data found")), White)
        }
        else
        {
            cue_durabilities.Init(200, PREDEFINED_CUES_N + 1);
            cue_durabilities[0] = -1;
            current_cue = 20;
            PRINTF((TEXT("Entry data not found")), White)
        }

        if (GameInstance && GameInstance->data->inventory->chalk_durabilities.Num() != 0)
        {
            chalk_durabilities = GameInstance->data->inventory->chalk_durabilities;
            current_chalk = GameInstance->data->inventory->current_chalk;
        }
        else
        {
            chalk_durabilities.Init(-1, PREDEFINED_CHALKS_N + 1);
            chalk_durabilities[1] = 200; chalk_durabilities[2] = 200; chalk_durabilities[3] = 200; chalk_durabilities[4] = 200;
            current_chalk = 2;
        }
        ps->username = TEXT("HeyThere") + FString::FromInt(FMath::RandRange(1, 100));
        ps->InitClient(cue_durabilities,
                       chalk_durabilities,
                       current_cue,
                       current_chalk,
                       EDifficultyLevels::DIFFICULTY_LEVEL_NONE);
        UE_LOG(LogTemp,Error,TEXT("generated username %s, for %s"),*ps->username,*ps->GetName())
    }
    bool bCanStartGame = true;
    int32 ItLenght = bIsTeamMode ? 4 : 2;
    for (int32 i = 0; i < ItLenght; ++i)
    {
        if (readyPCs[i])
        {
            continue;
        }
        bCanStartGame = false;
        break;
    }
    if (bCanStartGame) StartGame();
    UE_LOG(LogTemp, Error, TEXT("PC READY ON POOL8 GM"))
}

void APool8BallGameMode::StartGame()
{
    gameHasStarted = true;
    UPoolServerGameInstance* srv_gi = Cast<UPoolServerGameInstance>(GetWorld()->GetGameInstance());
    if (srv_gi)
    {
        UBackend1v1Info* b1v1i = dynamic_cast<UBackend1v1Info*> (srv_gi->backend_data);
        if (b1v1i)
        {
            cool_json_t req_json;
            req_json["match_id"] = b1v1i->match_id;
            srv_gi->MakeRequest("core", "internal/match_has_started", req_json, [](cool_json_t) {UE_LOG(LogTemp, Warning, TEXT("core match has started responded"))});
        }
    }
    
    playingPcs[0]->SetUsernames(Cast<APool8BallPlayerState> (playingPcs[0]->PlayerState)->username,
                                Cast<APool8BallPlayerState> (playingPcs[1]->PlayerState)->username);
    playingPcs[1]->SetUsernames(Cast<APool8BallPlayerState> (playingPcs[1]->PlayerState)->username,
                                Cast<APool8BallPlayerState> (playingPcs[0]->PlayerState)->username);
    
    for (auto spect : Spectators)
    {
        APool8BallPlayerController* sp_pc = Cast<APool8BallPlayerController> (spect);
        if (sp_pc)
            sp_pc->SetUsernames(Cast<APool8BallPlayerState> (playingPcs[0]->PlayerState)->username,
                                Cast<APool8BallPlayerState> (playingPcs[1]->PlayerState)->username);
    }
    
    uint8 first_pc_num = Referee->GetFirstPCPlayedNum();
    USefaPoolGameInstance* GI = Cast<USefaPoolGameInstance>(GetWorld()->GetGameInstance());
    if (GI)
    {
        GI->widgetManager->HideWidget("Loading");
    }
    APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*>(playingPcs[first_pc_num]->PlayerState);
    ps->ballInHand = true;
    ps->UpdateClient();
    firstShot = true;
    Referee->ShuffleAndArrangeBalls();
    Referee->firstShot = true;
    Referee->OnGameStart();
    // Referee->shotInfo.openTable = true;
    NextShot(first_pc_num);

    APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
    if (GS)
    {
        GS->ExecuteGameTime();
    }
}

void APool8BallGameMode::EnableTeamMode()
{
    UE_LOG(LogTemp,Error,TEXT("Process enable team mode"))
    bIsTeamMode = true;
    AMPRefereeBase* MPReferee = Cast<AMPRefereeBase>(Referee);
    if (MPReferee)
    {
        MPReferee->SetTeamMode(true);
        UE_LOG(LogTemp, Error, TEXT("Referee team mode set"))
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Can't set team mode on referee, referee is nullptr"))
    }
}

void APool8BallGameMode::NextShot(uint8 pc_num)
{
    objects->ballinfo->UpdateBallPosOnClients();
    AMPRefereeBase* MPReferee = Cast<AMPRefereeBase>(Referee);
    APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
    if (GS)
    {
        GS->Multicast_UpdateUserBarWidget();
    }
    if (MPReferee)
    {
        MPReferee->shotInfo.Clear();
        if (bIsTeamMode)
        {
            if (pc_num % 2 == 0)
            {
                MPReferee->shotInfo.currentPlayerNum = 0;
            }
            else
            {
                MPReferee->shotInfo.currentPlayerNum = 1;
            }
        }
        else
        {
            MPReferee->shotInfo.currentPlayerNum = pc_num;
        }
    }
    else UE_LOG(LogTemp, Error, TEXT("NEXT SHOT REFEREE NOT VALID"))
        int32 PlayerCount = bIsTeamMode ? 4 : 2;
    Referee->NextShot(pc_num);
    for (int32 i = 0; i < PlayerCount; ++i)
    {
        if (i == pc_num)
        {
            continue;
        }
        playingPcs[i]->ClientSetViewToTable();
    }
    playingPcs[pc_num]->ClientStartPlay();
    APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(playingPcs[pc_num]->PlayerState);
    if (PS)
    {
        PS->Client_ForceUpdateTargetableBalls();
    }else UE_LOG(LogTemp,Error,TEXT("PS not valid"))
    CurrentActivePlayerNum = pc_num;
    SetActivePlayerForSpectator(CurrentActivePlayerNum);
}

void APool8BallGameMode::ReturnSpectatorToMenu()
{
    for (int32 i = 0; i < Spectators.Num(); ++i)
    {
        Spectators[i]->ClientTravelToLevel("Loading", "Authentication");
    }
}


void APool8BallGameMode::SetActivePlayerForSpectator(uint8 ActivePlayerNum)
{
    for (int32 i = 0; i < Spectators.Num();++i)
    {
        Spectators[i]->Client_UpdateActivePlayer(ActivePlayerNum);
    }
    UE_LOG(LogTemp,Error,TEXT("Set active spectator count %i"),Spectators.Num())
}


void APool8BallGameMode::Tick(float dt)
{
    //UE_LOG(LogTemp, Warning, TEXT("FPS: %f"), 1.0f / dt)
    if (Referee->IsReadyToFinalize())
    {
        Referee->CheckRules();

        Referee->CheckRules_Finalize();
    }
}

void APool8BallGameMode::BroadcastOpponentStatus(APool8BallPlayerController* pc,
    int cueNum,
    FVector loc,
    FRotator rot,
    FVector cueLoc,
    FRotator cueRot)
{
    if (!CanPlayerShoot(pc)) {
        return;
    }
    int32 PlayerCount = bIsTeamMode ? 4 : 2;
    for (int32 i = 0; i < PlayerCount; ++i)
    {
        if (playingPcs[i] == pc)
        {
            continue;
        }
        playingPcs[i]->UpdateOpponentStatus(cueNum, loc, rot, cueLoc, cueRot);
    }
    for (int32 i = 0; i < Spectators.Num(); ++i)
    {
        Spectators[i]->UpdateOpponentStatus(cueNum,loc,rot,cueLoc,cueRot);
    }
}

void APool8BallGameMode::StartingPhysicsSimulationForShot(APoolPlayerControllerBase* pc)
{
    if (!CanPlayerShoot(pc)) return;

    APool8BallPlayerController* other_pc = playingPcs[0];
    if (other_pc == pc)
        other_pc = playingPcs[1];

    if (!other_pc) return;

    other_pc->StopOpponentUpdate();
}
