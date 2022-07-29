// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pool8BallReferee.h"
#if UE_SERVER
#include "RedisObject.h"
#endif
#include "TournBallReferee.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API ATournBallReferee : public APool8BallReferee
{
	GENERATED_BODY()
public:

	ATournBallReferee();

	void BeginPlay() override;

	virtual void GameResults() override;

};
