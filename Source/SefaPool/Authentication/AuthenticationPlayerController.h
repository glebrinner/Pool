// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../Shared/SharedDefs.h"
#include "../Shared/CueComponent.h"

#include "../Shared/SefaPoolGameInstance.h"
#include "../Shared/HTTP_GI.h"
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "../Widgets/PoolBaseShopWidget.h"
#include "AuthenticationPlayerController.generated.h"

static const int DayInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

USTRUCT(BlueprintType)
struct FRegistrationData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "RegistrationData")
		FString name;
	UPROPERTY(BlueprintReadWrite, Category = "RegistrationData")
		FString surname;
	UPROPERTY(BlueprintReadWrite, Category = "RegistrationData")
		FString username;
	UPROPERTY(BlueprintReadWrite, Category = "RegistrationData")
		FString password;
	UPROPERTY(BlueprintReadWrite, Category = "RegistrationData")
		FString day;
	UPROPERTY(BlueprintReadWrite, Category = "RegistrationData")
		FString month;
	UPROPERTY(BlueprintReadWrite, Category = "RegistrationData")
		FString year;
    UPROPERTY(BlueprintReadWrite, Category = "RegistrationData")
        FString ref_code;
};
void to_json(cool_json_t& json, const FRegistrationData& str);

UCLASS(Blueprintable)
class SEFAPOOL_API AAuthenticationPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	void BeginPlay();


	UFUNCTION(BlueprintCallable)
		TArray<bool> CheckRegistrationData(const FRegistrationData &data) const;

	UFUNCTION(BlueprintCallable, Category = "Requests")
		void RequestEntryDataForMainMenu(const FResponseDelegat &succesful, const FResponseDelegat& unsuccesful);

	UFUNCTION(BlueprintCallable, Category = "Requests")
		void RequestLogin(const FString& username, const FString& password, const FResponseDelegat& succesful, const FResponseDelegat& unsuccesful);
	
	UFUNCTION(BlueprintCallable, Category = "Requests")
		void Offline();

	UFUNCTION(BlueprintCallable, Category = "Requests")
		void RequestRegistration(const FRegistrationData& data, const FResponseDelegat& succesful, const FResponseDelegat& unsuccesful);

	UFUNCTION(BlueprintCallable, Category = "Requests")
		void RequestCheckConnectionWithServer(const FResponseDelegat& succesful, const FResponseDelegat& unsuccesful);

	// Defenition finding in AuthenticationPlayerControllerBP
	// If player has a save file call RequestEntyDataForMainMenu else show LoginWB
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerManagement")
		void ProcessEnteredPlayer();

	// Defenition finding in AuthenticationPlayerControllerBP
	// Call RequestLogin and process request
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerManagement")
		void ProcessLogin(const FString& username, const FString& password);

	// Definition finding in AuthenticationPlayerControllerBP
	// Call RequestRegistration and process request
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerManagement")
		void ProcessRegistration(const FRegistrationData& data);
private:
};
