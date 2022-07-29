// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/RefereeBase.h"
#include "MPRefereeBase.generated.h"




UCLASS()
class SEFAPOOL_API AMPRefereeBase : public ARefereeBase
{
	GENERATED_BODY()
public:	
	AMPRefereeBase();
	
	struct CurrentShotDetails shotInfo;
	FString win_user_id_;
	FString lose_user_id_;
	int win_pc_idx = -1;

	TArray<uint8> TargetableBalls;
protected:
	void BeginPlay() override;

	TArray<APoolPlayerControllerBase*> FirstTeam;
	TArray<APoolPlayerControllerBase*> SecondTeam;

	uint8 FirstTeamCurrentPlayerNum;
	uint8 SecondTeamCurrentPlayerNum;

	APoolPlayerControllerBase* TeamPCs[4];

	bool bIsTeamMode;

public:	
    virtual void OnPlayTimeEnd() override;
	virtual void ProcessBallPocketed(int num, bool first) override;
	virtual void ProcessBallKnockedOff(int num) override;
	virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
	virtual void NextTeamShot(uint8 team);

	virtual bool CanPlayerShoot(APoolPlayerControllerBase* player) override;
	virtual void NextShot(uint8 pc_num) override;
	void OnGameStart() override;
	void CheckRules() override;
	void CheckRules_FirstShot() override;
	void CheckRules_NotFirstShot() override;
	void CheckRules_NotFirstShot_BlackPocketed() override;
	void CheckRules_NotFirstShot_BlackNotPocketed() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted() override;
	bool IsReadyToFinalize() override;
	void CheckRules_Finalize() override;
	void RegisterMessage(E_MessageSeverity MessageSeverity, uint16 MessageID, bool ToCurrentPlayer = true, bool ShouldDeployMessage = false);
	void DeployMessages(bool ForBothPlayers = false);
	void TeamTravelToLoseScreen(uint8 TeamID);
	void TeamTravelToWinScreen(uint8 TeamID);

	virtual void SetWinIdx(APoolPlayerControllerBase* pc_winner);

	void SetTeamMode(bool NewState) { bIsTeamMode = NewState;}
};
