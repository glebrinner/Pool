
#include "TournamentGameState.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "Net/UnrealNetwork.h"

FTournamentMatchUIInfo::FTournamentMatchUIInfo() :
    user1 (TEXT("")),
    is_username1 (),
    user2 (TEXT("")),
    is_username2 (),
    state (ETournamentMatchState::NO_STATE),
    winner (TEXT("")),
    is_username_winner ()
{}

void FTournamentMatchUIInfo::FillUsernames(const TMap<FString, FString>& userid_to_username)
{
    if (user1.Len() && !is_username1 && userid_to_username.Contains(user1))
    {
        user1 = userid_to_username[user1];
        is_username1 = true;
    }
    if (user2.Len() && !is_username2 && userid_to_username.Contains(user2))
    {
        user2 = userid_to_username[user2];
        is_username2 = true;
    }
    if (winner.Len() && !is_username_winner && userid_to_username.Contains(winner))
    {
        winner = userid_to_username[winner];
        is_username_winner = true;
    }
}


void FTournamentMatchUIInfo::AddPlayer(FString user, const TMap<FString, FString>& userid_to_username)
{
    if (!user1.Len())
    {
        if (userid_to_username.Contains(user))
        {
            user1 = userid_to_username[user];
            is_username1 = true;
        }
        else
        {
            user1 = user;
            is_username1 = false;
        }
    }
    else
    if (!user2.Len())
    {
        if (userid_to_username.Contains(user))
        {
            user2 = userid_to_username[user];
            is_username2 = true;
        }
        else
        {
            user2 = user;
            is_username2 = false;
        }
    }
    state = ETournamentMatchState::NOT_STARTED;
}

void FTournamentMatchUIInfo::HasStarted()
{
    state = ETournamentMatchState::RUNNING;
}

void FTournamentMatchUIInfo::HasWon(FString user, const TMap<FString, FString>& userid_to_username)
{
    state = ETournamentMatchState::FINISHED;
    winner = user;
    if (userid_to_username.Contains(winner))
    {
        winner = userid_to_username[winner];
        is_username_winner = true;
    }
    else
        is_username_winner = false;
}


void ATournamentGameState::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME( ATournamentGameState, tournament_info );
}

void ATournamentGameState::BeginPlay()
{
    Super::BeginPlay();
    FTournamentMatchUIInfo init;
    UPoolServerGameInstance* gi = Cast<UPoolServerGameInstance> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi) {
        UE_LOG(LogTemp, Error, TEXT("Game instance is nullptr"));
        return;
    }
    UBackendTournamentInfo* backend_data = dynamic_cast<UBackendTournamentInfo*>(gi->backend_data);
    if (!backend_data) {
        UE_LOG(LogTemp, Warning, TEXT("backend_data is nullptr, using default values."));
        tournament_info.Init(init, 7);
        return;
    }
    tournament_info.Init(init, backend_data->size - 1);
}


void ATournamentGameState::CheckUsernames(const TMap<FString, FString>& userid_to_username)
{
    for (auto& info : tournament_info)
        info.FillUsernames(userid_to_username);
}
