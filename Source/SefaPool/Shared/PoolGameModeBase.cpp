// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolGameModeBase.h"
#include "SefaPool/8Ball/Referee/RefereeBase.h"


void APoolGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Referee = GetWorld()->SpawnActor<ARefereeBase>(Referee_Class);
}

void APoolGameModeBase::PCReady(class APoolPlayerControllerBase* player)
{}
void APoolGameModeBase::ProcessBallPocketed(int num, bool first)
{}	
void APoolGameModeBase::ProcessBallKnockedOff(int num)
{}
void APoolGameModeBase::ProcessHitLogAndFinalize(const TArray<int>& hitLog1,
                                      const TArray<int>& hitLog2)
{}

void APoolGameModeBase::OnPlayerShotEvent(uint8 PlayerID)
{
}

bool APoolGameModeBase::CanPlayerShoot (APoolPlayerControllerBase* player)
{
    return true;
}

void APoolGameModeBase::StartingPhysicsSimulationForShot (APoolPlayerControllerBase* player)
{}

TSharedPtr<FJsonObject> APoolGameModeBase::CreateConsolidateJson(APoolPlayerStateBase* ps)
{
    TSharedPtr<FJsonObject> json = MakeShareable(new FJsonObject);
    json->SetStringField("user_id", ps->user_id);
    TArray<TSharedPtr < FJsonValue >> cues, chalks;
    const FItemDurability& consumables = ps->GetItemDurability();
    UE_LOG(LogTemp, Warning, TEXT("Create subjson for cues"));
    for (int i = 1; i < PREDEFINED_CUES_N + 1; ++i)
    {
        if (consumables.cue_durabilities[i] != -1)
        {
            TSharedPtr<FJsonObject> cue_json = MakeShareable(new FJsonObject);
            cue_json->SetStringField("cue_id", FString::FromInt(i));
            cue_json->SetStringField("recharged", FString::FromInt(consumables.cue_recharges[i]));
            cue_json->SetStringField("used", FString::FromInt(consumables.cue_usages[i]));
            TSharedRef<FJsonValueObject> JsonValue = MakeShareable(new FJsonValueObject(cue_json));
            cues.Add(JsonValue);
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Create subjson for chalk"));
    for (int i = 1; i < PREDEFINED_CHALKS_N + 1; ++i)
    {
        if (consumables.chalk_durabilities[i] != -1)
        {
            TSharedPtr<FJsonObject> chalk_json = MakeShareable(new FJsonObject);
            chalk_json->SetStringField("chalk_id", FString::FromInt(i));
            chalk_json->SetStringField("recharged", FString::FromInt(consumables.chalk_recharges[i]));
            chalk_json->SetStringField("used", FString::FromInt(consumables.chalk_usages[i]));
            TSharedRef<FJsonValueObject> JsonValue = MakeShareable(new FJsonValueObject(chalk_json));
            cues.Add(JsonValue);
        }
    }
    json->SetArrayField("cues", cues);
    return json;
}

void APoolGameModeBase::LevelReady()
{}
