// Fill out your copyright notice in the Description page of Project Settings.


#include "VarificationPlayerController.h"

void AVarificationPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && GetNetDriver())
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

void AVarificationPlayerController::SetDelay(double delay)
{
	delay_ = delay;
}

double AVarificationPlayerController::GetDelay() const
{
	return delay_;
}

void AVarificationPlayerController::CloseConnection()
{
	connection_->Close();
}

bool AVarificationPlayerController::IsClientAlive() const
{
	return connection_->LastTickTime - connection_->LastReceiveTime < delay_;
}

void AVarificationPlayerController::InitPlayerStateData() {
	AVarificationPlayerState* ps = Cast<AVarificationPlayerState>(PlayerState);
	if (ps) {
		ps->user_id = UGameplayStatics::ParseOption(Options, "user_id");
	}
	else
		UE_LOG(LogTemp, Error, TEXT("PlayerState is nulltpr in AVarificationPlayerController."));
}
