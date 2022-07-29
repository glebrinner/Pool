
#pragma once

#include "SefaPoolDataBlock.h"
#include <vector>
#include "SefaPool/Shared/HTTP_GI.h"
#include "SefaPoolMessages.generated.h"

UENUM(BlueprintType)
enum class ERemoteMessageType : uint8
{
    NO_MESSAGE                          = 0 UMETA(DisplayName = "No message"),
    FRIEND_REQUEST_TRANSIENT            = 1 UMETA(DisplayName = "Friend request"),
    FRIEND_RESPONSE_TRANSIENT           = 2 UMETA(DisplayName = "Friend request response"),
    CONTEST_WIN_PERSISTENT              = 3 UMETA(DisplayName = "Contest win"),
    INVITE_TO_GAME_TRANSIENT            = 4 UMETA(DisplayName = "Invited to game"),
    START_FRIENDLY_GAME_TRANSIENT       = 5 UMETA(DisplayName = "Friendly game starting"),
    CHAMPIONSHIP_WIN_PERSISTENT         = 6 UMETA(DisplayName = "Championship win"),
    FREE_TOURNAMENT_WIN_TRANSIENT       = 7 UMETA(DisplayName = "Tournament win"),
    MESSAGE_NUM  
};

UENUM(BlueprintType)
enum class EPlayerMenuState : uint8
{
    OFFLINE    = 0 UMETA(DisplayName = "Offline"),
    MAIN_MENU  = 1 UMETA(DisplayName = "Main menu navigation"),
    PLAYING    = 2 UMETA(DisplayName = "Playing"),
    SPECTATING = 3 UMETA(DisplayName = "Spectating"),
    IN_LOBBY   = 4 UMETA(DisplayName = "Lobby")
};

void from_json(const cool_json_t& json, ERemoteMessageType& mt);
void to_json(cool_json_t& json, const EPlayerMenuState& pms);

USTRUCT(BlueprintType)
struct FRemoteMessageInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString messageJson;
    UPROPERTY(BlueprintReadOnly)
    FString shortDescription;
    UPROPERTY()
    bool read;
};

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolMessages : public USefaPoolDataBlock
{
    GENERATED_BODY()

    void RegisterMessage(cool_json_t message);
public:
    
    virtual void Init(USefaPoolGameInstance* gi_);
    
    std::vector<cool_json_t> activeMessages;
    FTimerHandle pingTimer;

    UPROPERTY()
        TMap<int, FRemoteMessageInfo> messages;
    UPROPERTY()
        int largestMessageId = 0;


    UFUNCTION(BlueprintCallable)
    void MessageRead(int id);

    UFUNCTION(BlueprintCallable)
    void MessageDismissed(int id);
    

    
    //bool pingOk = true;
    
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);

    FString JsonToShortMessageDescription(cool_json_t json);
    
    UFUNCTION()
    void SendPingOnTimer();

    UFUNCTION(BlueprintCallable)
    FRemoteMessageInfo GetLatestMessage();
};
