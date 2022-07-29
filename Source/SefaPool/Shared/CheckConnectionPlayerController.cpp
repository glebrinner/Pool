// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckConnectionPlayerController.h"
#include "Engine/NetConnection.h"
#include "Engine/NetDriver.h"

void ACheckConnectionPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		for (auto connection : GetNetDriver()->ClientConnections)
		{
			if (connection->OwningActor == GetOwner())
			{
				connection_ = connection;
			}
		}
	}
}

void ACheckConnectionPlayerController::SetDelay(double delay)
{
	delay_ = delay;
}

double ACheckConnectionPlayerController::GetDelay() const
{
	return delay_;
}

//Closes connection of PlayerController
void ACheckConnectionPlayerController::CloseConnection()
{
	connection_->Close();
}

//Return True if 
bool ACheckConnectionPlayerController::IsClientAlive() const
{
	return connection_->LastTickTime - connection_->LastReceiveTime < delay_;
}



