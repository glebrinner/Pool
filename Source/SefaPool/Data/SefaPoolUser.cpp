
#include "SefaPoolUser.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void USefaPoolUser::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "user", 0);
}
    
void USefaPoolUser::Load()
{
    USefaPoolUser* LoadedGame = Cast<USefaPoolUser>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "user", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load user")), Yellow)
        birthday.Empty();
        first_name.Empty();
        last_name.Empty();
        username.Empty();
        attached_to_ref_code.Empty();
        owned_ref_code.Empty();
        attached_to_ref_code.Empty();
    }
    else
    {
        birthday = LoadedGame->birthday;
        first_name = LoadedGame->first_name;
        last_name = LoadedGame->last_name;
        username = LoadedGame->username;
        attached_to_ref_code = LoadedGame->attached_to_ref_code;
        owned_ref_code = LoadedGame->owned_ref_code;
        attached_to_ref_code = LoadedGame->attached_to_ref_code;
        PRINTF((TEXT("User successfully loaded")), Green)
    }
}
    
void USefaPoolUser::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                      const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/get_user_info"))
        {
            if (resp_json.contains("birthday") &&
                resp_json.contains("first_name") &&
                resp_json.contains("second_name") &&
                resp_json.contains("username") &&
                resp_json.contains("has_parent_ref_code"))
            {
                resp_json["birthday"].get_to(birthday);
                resp_json["first_name"].get_to(first_name);
                resp_json["second_name"].get_to(last_name);
                resp_json["username"].get_to(username);
                if (resp_json["has_parent_ref_code"].get<bool> ())
                    resp_json["parent_ref_code"].get_to(attached_to_ref_code);
                else
                    attached_to_ref_code.Empty();
                Save();
                if (successful) successful(this);
            }
            else
            {
                if (unsuccessful) unsuccessful(this);
                UE_LOG(LogTemp, Warning, TEXT("user/get_user_info did not contain birthday, first_name, second_name, username or has_parent_ref_code"))
            }
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("user", "get_user_info", cool_json_t(), callback);
}

void USefaPoolUser::Logout()
{
    if (!gi) return;
    gi->Logout();
}

void USefaPoolUser::FullLogout()
{
    if (!gi) return;
    gi->FullLogout();
}

void USefaPoolUser::AttachRef(const FString& username_,
               const FResponseDelegat& successful,
               const FResponseDelegat& unsuccessful)
{
    if (!gi)
    {
        unsuccessful.Execute();
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "referral/set_parent_ref"))
        {
            if (resp_json.contains("ref_code"))
            {
                resp_json["ref_code"].get_to(attached_to_ref_code);
                Save();
                successful.Execute();
            }
            else
            {
                unsuccessful.Execute();
                UE_LOG(LogTemp, Warning, TEXT("referrals/generate_ref_code did not contain ref_code"))
            }
        }
        else unsuccessful.Execute();
    };
    cool_json_t req;
    req["ref_user_info"] = username_;
    req["is_username"] = true;
    gi->MakeRequest("referrals", "set_parent_ref", req, callback);
}

void USefaPoolUser::GetRef(const FResponseDelegat& successful,
            const FResponseDelegat& unsuccessful)
{
    if (!gi)
    {
        unsuccessful.Execute();
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "referrals/generate_ref_code"))
        {
            if (resp_json.contains("ref_code"))
            {
                resp_json["ref_code"].get_to(owned_ref_code);
                Save();
                successful.Execute();
            }
            else
            {
                unsuccessful.Execute();
                UE_LOG(LogTemp, Warning, TEXT("referrals/generate_ref_code did not contain ref_code"))
            }
        }
        else unsuccessful.Execute();
    };
    
    gi->MakeRequest("referrals", "generate_ref_code", cool_json_t(), callback);
}
