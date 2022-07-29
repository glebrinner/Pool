
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolMessages.h"
#include "SefaPoolFriends.generated.h"

/*

UENUM(BlueprintType)
enum class EPlayerMenuState : uint8
{
    OFFLINE    = 0 UMETA(DisplayName = "Offline"),
    MAIN_MENU  = 1 UMETA(DisplayName = "Main menu navigation"),
    PLAYING    = 2 UMETA(DisplayName = "Playing"),
    SPECTATING = 3 UMETA(DisplayName = "Spectating"),
    IN_LOBBY   = 4 UMETA(DisplayName = "Lobby")
};
*/
USTRUCT(BlueprintType)
struct FPlayerFriend
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY(BlueprintReadOnly)
    FString username;
    UPROPERTY(BlueprintReadOnly)
    FString user_id;
    UPROPERTY(BlueprintReadOnly)
    bool online;
    UPROPERTY(BlueprintReadOnly)
    EPlayerMenuState state;
};

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolFriends : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    TArray<FPlayerFriend> friendsArray;
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    
    void AcceptFriendRequest(FString user_id);
    void IgnoreFriendRequest(FString user_id);
    
    void AcceptInvitation(FString invite_id);
    void RejectInvitation(FString invite_id);
    
    UFUNCTION(BlueprintCallable)
    void SendFriendRequest(FString user_id);
    UFUNCTION(BlueprintCallable)
    void SendInvitation(FString username,
                        EDifficultyLevels diff,
                        int amount,
                        const FResponseDelegat& invited,
                        const FResponseDelegat& offline,
                        const FResponseDelegat& not_found,
                        const FResponseDelegat& unsuccessful);

    UFUNCTION(BlueprintCallable)
    bool IsFriend(FString username);
    UFUNCTION(BlueprintCallable)
    TArray<FPlayerFriend> GetFriendsFiltered(FString filter, bool online);
};
