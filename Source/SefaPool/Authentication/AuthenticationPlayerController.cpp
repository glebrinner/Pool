// Fill out your copyright notice in the Description page of Project Settings.


#include "AuthenticationPlayerController.h"



void to_json(cool_json_t& json, const FRegistrationData& struct_)
{
    json["name"] = struct_.name;
    json["surname"] = struct_.surname;
    json["username"] = struct_.username;
    json["password"] = struct_.password;
    json["day"] = struct_.day;
    json["month"] = struct_.month;
    json["year"] = struct_.year;
    json["ref_code"] = struct_.ref_code;
}

void AAuthenticationPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

// Send request to API serivce with endpoint get_intro.
// After that initialize class object FEntryData::entry_data_ with response data.
void AAuthenticationPlayerController::RequestEntryDataForMainMenu(const FResponseDelegat& succesful, const FResponseDelegat& unsuccesful)
{
    USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        gi->RequestEntyDataForMainMenu(succesful, unsuccesful);
    }
}


void AAuthenticationPlayerController::RequestLogin(const FString &username,
                                                   const FString& password, const
                                                   FResponseDelegat& succesful, const
                                                   FResponseDelegat& unsuccesful)
{
    
    USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        gi->Login(username, password, succesful, unsuccesful);
    }
}


void AAuthenticationPlayerController::Offline()
{

	USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
	if (gi)
	{
		gi->PlayingOffline();
	}
}

void AAuthenticationPlayerController::RequestRegistration(const FRegistrationData& data, const FResponseDelegat& succesful, const FResponseDelegat& unsuccesful)
{
    cool_json_t json = data;
	auto func = [this, succesful, unsuccesful](FHttpRequestPtr request, FHttpResponsePtr response, bool success) -> void {
		if (!success)
		{
			PRINTF((TEXT("Problem with server. Request %s was unsuccesful"), TEXT("RequestRegistration")), Red);
			unsuccesful.Execute();
		}
		else
		{
            cool_json_t json_resp(response);
			if (!json_resp.is_null())
			{
				bool username_available = json_resp["username_available"].get<bool>();
				if (username_available)
				{
					succesful.Execute();
				}
				else
				{
					unsuccesful.Execute();
				}
			}
			else
			{
				PRINTF((TEXT("Problem with reading json. Recieved json is: %s"), *response->GetContentAsString()), Red);
				unsuccesful.Execute();
			}
		}
	};
    USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        gi->MakeRequestWithInitToken("auth", "registration", json, func);
    }
}

void AAuthenticationPlayerController::RequestCheckConnectionWithServer(const FResponseDelegat& succesful, const FResponseDelegat& unsuccesful)
{
    USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        gi->CheckConnectivity(succesful, unsuccesful);
    }
    
}

TArray<bool> AAuthenticationPlayerController::CheckRegistrationData(const FRegistrationData& data) const
{
	TArray<bool> is_correct = {0, 0, 0, 0, 0};

	if (!data.name.IsEmpty())
		is_correct[0] = 1;
	if (!data.surname.IsEmpty())
		is_correct[1] = 1;
	if (!data.username.IsEmpty())
		is_correct[2] = 1;
	if (!data.password.IsEmpty())
		is_correct[3] = 1;

	auto FStringToInt = [](const FString& s)
	{
		if (s.IsEmpty())
			return -1;

		int number = 0;
		for (char c : s) {
			if (isdigit(c))
				number = number * 10 + (c - '0');
			else
				return -1;
		}
		return number;
	};

	int day = FStringToInt(data.day);
	int month = FStringToInt(data.month);
	int year = FStringToInt(data.year);

	if (month >= 1 && month <= 12 && day >= 1 && day <= DayInMonth[month - 1] && year >= 1950 && year <= 2020)
	{
		is_correct[4] = 1;
	}

	return is_correct;
}
