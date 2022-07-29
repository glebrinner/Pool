// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseActor.h"
#include "SefaPool/Server/PoolServerGameInstance.h"

// Sets default values
ADatabaseActor::ADatabaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADatabaseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADatabaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (status)
	{
	case Status::STANDART: {
		int i = 0;
		while (i < queries_for_tick && (!add_queue.empty() || !del_queue.empty()))
		{
			auto it = add_queue.begin();
			for (int j = 0; it != add_queue.end() && j < priority && i < queries_for_tick; ++j, ++i) {
				database[*it] = true;
				++it;
				add_queue.erase(std::prev(it));
			}

			it = del_queue.begin();
			for (int j = 0; it != del_queue.end() && j < priority && i < queries_for_tick; ++j, ++i)
			{
				database.erase(*it);
				++it;
				del_queue.erase(std::prev(it));
			}
		}
		break;
	}
	case Status::INVALIDATION:
		for (int i = 0; i < invalidations_for_tick && current_it != database.end(); ++i)
		{
			current_it->second = false;
			current_it = std::next(current_it);
		}
		if (current_it == database.end())
		{
			status = Status::VERIFICATION;
		}
		break;
	case Status::VERIFICATION:
 		for (int i = 0; i < queries_for_tick && !verification_queue.empty(); ++i) {
			database[verification_queue.front()] = true;
			verification_queue.pop();
			verification_database_size += 1;
		}
		if (!is_verification && verification_queue.empty()) {
			status = Status::END_VERIFICATION;
			current_it = database.begin();
		}
		break;
	case Status::END_VERIFICATION:
		{
			//UE_LOG(LogTemp, Warning, TEXT("Current database is incorrect"));
			for (int i = 0; i < invalidations_for_tick && current_it != database.end(); ++i)
			{
				auto next_it = std::next(current_it);
				if (!current_it->second) {
					database.erase(current_it);
				}
				current_it = next_it;
			}
			if (current_it == database.end())
			{
				status = Status::STANDART;
			}
			for (auto pair : database)
			{
				UE_LOG(LogTemp, Warning, TEXT("user_id: %d, access token: %s"), pair.first.first, *FString(pair.first.second.c_str()));
			}
		}
		break;
	}
}

void ADatabaseActor::handle_query(uint8* bstring, int size)
{
	uint8 query = bstring[0];
	UE_LOG(LogTemp, Warning, TEXT("========================PRINT QUERY========================"));
	for (int i = 0; i < size; ++i)
	{
		printf("%c", bstring[i]);
	}
	printf("\n");
	UE_LOG(LogTemp, Warning, TEXT("========================END PRINT QUERY========================"));
	switch (query)
	{
	case 0: // Add to the database
		if (size > 6)
		{
			uint32 player_id = *(uint32*)(bstring + 1);
			std::string access_token(reinterpret_cast<char*>(bstring) + 5);
			UE_LOG(LogTemp, Warning, TEXT("Add user_id: %d, access_token: %s...%s"), player_id,
																					 std::string(access_token.begin(), access_token.begin() + 4).c_str(),
																					 std::string(access_token.end() - 5, access_token.end()).c_str());
			add_queue.insert({ player_id, access_token });
		}
		break;
	case 1: // delete from database
		if (size > 6)
		{
			UE_LOG(LogTemp, Warning, TEXT("Del from database"));
			uint32 player_id = *(uint32*)(bstring + 1);
			std::string access_token(reinterpret_cast<char*>(bstring) + 5);
			del_queue.insert({ player_id, access_token });
		}
		break;
	case 2: // varify player
		if (size > 6)
		{
			is_different = false;
			verification_database_size = 0;

			UE_LOG(LogTemp, Warning, TEXT("Start invalidation"));
			current_it = database.begin();

			status = Status::INVALIDATION;

			UE_LOG(LogTemp, Warning, TEXT("Verify players"));
			uint8 query_num = *(uint32*)(bstring + 1);
			uint16 index = 5;

			for (uint8 i = 0; i < query_num; ++i) {
				uint32 player_id = *(uint32*)(bstring + index);
				std::string access_token(reinterpret_cast<char*>(bstring) + index + 4);
				index += access_token.size() + 1 + 4;
				FString tmp(access_token.c_str());
				UE_LOG(LogTemp, Warning, TEXT("verify: %d %s"), player_id, *tmp);
				verification_queue.push({ player_id, access_token });
			}

			UE_LOG(LogTemp, Warning, TEXT("End verifying"));
			is_verification = false;
		}
		break;
	case 5:
		UE_LOG(LogTemp, Warning, TEXT("Database:"));
		UE_LOG(LogTemp, Warning, TEXT("======================"));
		for (auto const& pair : database) {
			FString tmp(pair.first.second.c_str());
			UE_LOG(LogTemp, Warning, TEXT("%d %s"), pair.first.first, *tmp);
		}
		UE_LOG(LogTemp, Warning, TEXT("======================"));
		break;
	case 6:
		SendVerify();
	default:
		break;
	}
}

bool ADatabaseActor::find(FString access_token, FString player_id) const
{
	std::pair<uint32, std::string> key = { FCString::Atoi(*player_id), std::string(TCHAR_TO_UTF8(*access_token)) };
	return database.find(key) != database.end() || add_queue.find(key) != add_queue.end();
}

#include <string>

void ADatabaseActor::SendVerify() const 
{
    
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    
    if (srv_gi)
    {
        cool_json_t json;
        
		json["hashes"] = cool_json_t::array();
		for (const auto& pair : database) {
			uint64 player_id = pair.first.first;
			std::string access_token = pair.first.second;
			uint64 hash_id, hash_token;
			hash_id = MurmurHash64A((void*)&player_id, sizeof(uint64), 1);
			hash_token = MurmurHash64A((void*)&access_token, access_token.size(), 1);

			json["hashes"].push_back(hash_id ^ hash_token);
		}
		json["n"] = database.size();

        auto callback = [](cool_json_t json) -> void {};
        
        srv_gi->MakeRequest("lobbyrelay", "verify_req", json, callback);
    }
}
