
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolUser.generated.h"

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolUser : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    FString birthday;
    UPROPERTY(BlueprintReadOnly)
    FString first_name;
    UPROPERTY(BlueprintReadOnly)
    FString last_name;
    UPROPERTY(BlueprintReadOnly)
    FString username;
    UPROPERTY(BlueprintReadOnly)
    FString owned_ref_code;
    UPROPERTY(BlueprintReadOnly)
    FString attached_to_ref_code;
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    
    UFUNCTION(BlueprintCallable)
    void Logout();
    
    UFUNCTION(BlueprintCallable)
    void FullLogout();
    
    UFUNCTION(BlueprintCallable)
    void AttachRef(const FString& username_,
                   const FResponseDelegat& successful,
                   const FResponseDelegat& unsuccessful);
    
    UFUNCTION(BlueprintCallable)
    void GetRef(const FResponseDelegat& successful,
                const FResponseDelegat& unsuccessful);
};
