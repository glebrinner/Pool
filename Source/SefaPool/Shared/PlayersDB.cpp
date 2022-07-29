// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayersDB.h"

PlayersDB::PlayersDB()
{

}

PlayersDB::~PlayersDB()
{
}

void PlayersDB::EditDB()
{
}

void PlayersDB::GetChanges()
{
}

void PlayersDB::read_from_bstring(uint8* bstring)
{
	uint8 query = bstring[0];
	uint32 player_id = static_cast<uint32>(bstring[1] << 24 | bstring[2] << 16 | bstring[3] << 8 | bstring[4]);
	std::string access_token(reinterpret_cast<char*>(bstring) + 5);

	switch (query)
	{
	case 0:
		players[access_token] = player_id;
		break;
	case 1:
		players.erase(access_token);
		break;
	default:
		break;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Database:"));
	UE_LOG(LogTemp, Warning, TEXT("======================"));
	for (auto const& pair : players) {
		FString tmp(pair.first.c_str());
		UE_LOG(LogTemp, Warning, TEXT("%s: %d"), *tmp, pair.second);
	}
	UE_LOG(LogTemp, Warning, TEXT("======================"));
	
}
