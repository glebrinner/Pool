// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Networking.h"
#include <string>
#include <unordered_map>
#include "../Shared/SharedDefs.h"
#include "Kismet/KismetStringLibrary.h"
//#include "Interfaces/IPv4/IPv4Address.h"

class SEFAPOOL_API PlayersDB
{
public:
	PlayersDB();
	~PlayersDB();
	void read_from_bstring(uint8* bstring);
	void EditDB();
	void GetChanges();

private:
	std::unordered_map<std::string, uint32> players;
	//FIPv4Endpoint Endpoint;
};
