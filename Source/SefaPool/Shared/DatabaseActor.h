// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MurmurHash2.h"
#include <unordered_map>
#include <queue>
#include <functional>
#include <set>
#include <string>
#include "Dom/JsonObject.h"
#include "DatabaseActor.generated.h"

struct hash_pair {
	template <class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& p) const
	{
		uint16 hash_computed = 0;
		auto hash1 = std::hash<T1>{}(p.first);
		auto hash2 = std::hash<T2>{}(p.second);
		for (int i = 0; i < sizeof(size_t); i += 2)
			hash_computed ^= (uint16)(((hash1 >> 8 * i) & 0xFFFF) ^ ((hash2 >> 8 * i) & 0xFFFF));
		return hash_computed;
	}
};

using database_t = std::unordered_map<std::pair<uint32, std::string>, bool, hash_pair>;

enum class Status
{
	STANDART,
	INVALIDATION,
	VERIFICATION,
	END_VERIFICATION
};

static const int queries_for_tick = 10;
static const int priority = 2; // number to manage add_query and del_query lies in [1, queries_for_tick]
static const int invalidations_for_tick = 10;

UCLASS()
class SEFAPOOL_API ADatabaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADatabaseActor();
	void handle_query(uint8* bstring, int size);
	bool find(FString access_token, FString player_id) const;
	void SendVerify() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	database_t database;
	std::set < std::pair<uint32, std::string>> add_queue;
	std::set < std::pair<uint32, std::string>> del_queue;
	std::queue < std::pair<uint32, std::string>> verification_queue;
	database_t::iterator current_it;
	bool is_verification = true;
	bool is_different;
	size_t verification_database_size;
	Status status = Status::STANDART;

};
