// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Shared/DatabaseActor.h"
#include "../Shared/PlayerVarificationGameMode.h"
#include "../Shared/TcpComunicationActor.h"
#include "../Shared/Ball.h"
#include "GameFramework/GameMode.h"
#include "Engine.h"
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include "GameFramework/GameModeBase.h"
#include "../Shared/PoolPlayerControllerBase.h"
#include "SefaPool/Server/GsAsyncQueueAllocator.h"
#include "LobbyPlayerState.h"

#include "AgonesComponent.h"

#include "LobbyGameMode.generated.h"

/**
 *
 */
UCLASS()
class SEFAPOOL_API ALobbyGameMode : public APlayerVarificationGameMode
{
	GENERATED_BODY()
private:
	std::map<std::pair<int, int>, std::queue<ALobbyPlayerState*>> players_queues;
	std::unordered_map<int, std::set<ALobbyPlayerState*>> DifficultyToPlayers;
    
    UPROPERTY()
    UGsAsyncQueueAllocator* gs_allocate;
    
public:
    
#if UE_SERVER
		UAgonesComponent* AgonesSDK;
#endif
    
	ALobbyGameMode();

	virtual void PreLogin(const FString& Options,
		const FString& Address,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer,
		ENetRole InRemoteRole,
		const FString& Portal,
		const FString& Options,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	void CheckEnoughBalance(ALobbyPlayerState* ps);

	void AddPlayer(ALobbyPlayerState* ps);
	void ConnectPlayersToGame(ALobbyPlayerState* ps1, ALobbyPlayerState* ps2, FString amount);
	void UpdatePlayersQueues();
	void ConnectWithAnotherStake(ALobbyPlayerState* ps);
    
    
    UFUNCTION()
    void AllocateAgones(const FGameServerResponse& Response);

    UFUNCTION()
    void AgonesSuccess(const FEmptyResponse& Response);

    UFUNCTION()
    void AgonesError(const FAgonesError& Response);
};
