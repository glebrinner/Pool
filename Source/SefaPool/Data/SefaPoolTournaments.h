
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPool/Shared/PoolPlayerAimLine.h"
#include "SefaPoolTournaments.generated.h"


USTRUCT(BlueprintType)
struct FTournamentInfo {
    GENERATED_USTRUCT_BODY()

    //FString tourn_id;
    UPROPERTY(BlueprintReadOnly)
    EDifficultyLevels difficulty;
    UPROPERTY(BlueprintReadOnly)
    FString ip;
    UPROPERTY(BlueprintReadOnly)
    int port;
    
    bool filled = false;
};

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolTournaments : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    FTournamentInfo ti;
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);

    //TArray<FTournamentInfo> TournLobbiesInfo;

    UFUNCTION(BlueprintCallable)
    void ParticipateInFreeTournament(const FResponseDelegat& successful,
                                     const FResponseDelegat& unsuccessful);
};
