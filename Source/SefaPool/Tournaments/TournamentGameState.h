

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/GameStateBase.h"

#include "TournamentGameState.generated.h"

UENUM(BlueprintType)
enum class ETournamentMatchState : uint8
{
    NO_STATE           = 0 UMETA(DisplayName = "No state"),
    NOT_STARTED        = 1 UMETA(DisplayName = "Not started"),
    RUNNING            = 2 UMETA(DisplayName = "Running"),
    FINISHED           = 3 UMETA(DisplayName = "Finished")
};

USTRUCT(BlueprintType)
struct FTournamentMatchUIInfo
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(BlueprintReadOnly)
    FString user1;
    UPROPERTY(BlueprintReadOnly)
    bool is_username1;
    UPROPERTY(BlueprintReadOnly)
    FString user2;
    UPROPERTY(BlueprintReadOnly)
    bool is_username2;
    UPROPERTY(BlueprintReadOnly)
    ETournamentMatchState state;
    UPROPERTY(BlueprintReadOnly)
    FString winner;
    UPROPERTY(BlueprintReadOnly)
    bool is_username_winner;
    
    FTournamentMatchUIInfo();
    
    void FillUsernames(const TMap<FString, FString>& userid_to_username);
    void AddPlayer(FString user, const TMap<FString, FString>& userid_to_username);
    void HasStarted();
    void HasWon(FString user, const TMap<FString, FString>& userid_to_username);
};


UCLASS()
class SEFAPOOL_API ATournamentGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
    UPROPERTY(replicated, BlueprintReadWrite)
    TArray<FTournamentMatchUIInfo> tournament_info;
    
    virtual void BeginPlay() override;
    
    virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
    
    void CheckUsernames(const TMap<FString, FString>& userid_to_username);
};
