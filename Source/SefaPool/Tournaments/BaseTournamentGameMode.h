// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SefaPool/PlayersVarification/VarificationGameMode.h"
#include "SefaPool/PlayersVarification/VarificationPlayerState.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "TournamentPlayerController.h"
#include "TournamentPlayerState.h"
#include "../Shared/PlayerVarificationGameMode.h"
#include "Tournament.h"
#if UE_SERVER
#include "RedisObject.h"
#endif

#include "SefaPool/Server/GsAsyncQueueAllocator.h"

//#if UE_SERVER
#include "AgonesComponent.h"
//#endif

#include "BaseTournamentGameMode.generated.h"


struct match
{
	bool player_has_entered = false;
	std::pair<APlayerController*, APlayerController*> pcs;
	int balls_left = -1;
	int games_left = -1;
	int match_id = -1;
	int stage = -1;
	//bool AddPlayer(APlayerController* pc) {
	//	if (pcs.size() == 2)
	//		return false;
	//	pcs.push_back(pc);
	//}
};

UCLASS()
class SEFAPOOL_API ABaseTournamentGameMode : public AVarificationGameMode//: public APlayerVarificationGameMode
{
	GENERATED_BODY()

public:


//#if UE_SERVER
	UAgonesComponent* AgonesSDK;
//#endif

	ABaseTournamentGameMode();

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

	virtual void BeginPlay() override;

	void UpdateListOfPlayers();

	void ShutdownServer();

    UFUNCTION()
    void ReadyAgones(const FGameServerResponse& Response);

    UFUNCTION()
    void AgonesSuccess(const FEmptyResponse& Response);

    UFUNCTION()
    void AgonesError(const FAgonesError& Response);

public:
	UPROPERTY()
	UTournament* tournament;
};
