// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInputValidation.h"
#include "CueComponent.h"
#include "ObjectGatherer.h"
#include "SefaPool/Shared/PoolPlayerStateBase.h"
#include "SefaPool/Shared/PoolGameModeBase.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "SefaPool/Shared/PoolPlayerControllerBase.h"

// Sets default values
AUserInputValidation::AUserInputValidation() :
    objects(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AUserInputValidation::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(objects)
}


void AUserInputValidation::ValidateShot(APoolPlayerControllerBase* pc,
                  FVector target,
                  FVector2D offset,
                  float power,
                  bool usingAngle)
{
    const float maxPower = MAX_BALL_LINEAR_VELOCITY;
    const float minPower = MAX_BALL_LINEAR_VELOCITY / 20.0f;
    
    if (!HasAuthority()) return;
    
    if (!GetWorld()->GetAuthGameMode<APoolGameModeBase>()->CanPlayerShoot(pc)) return;
    
    
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (pc->PlayerState);
    FCueInfo ci;
    ci.ChangeCueNum(1);
    FCueInfo* actualCi = ps ? &(ps->cueInfo) : &ci;
    const float cuePower = actualCi->GetPower();
    const float cueEffect = actualCi->GetPower();
    
    
    PRINTF((TEXT("%s"), *offset.ToString()), Green)
    
    if (!usingAngle)
        target.Z = 0.0f;
    target.Normalize();
    // Checking cue - table angle
    //if (target.Z*target.Z > target.X*target.X + target.Y*target.Y) return;
    // Checking offset
    if (offset.SizeSquared() > 1.0f)
        offset.Normalize();
    
    FVector velocity;
    bool applyingAngular = false;
    FVector angularVelocity;
    
    FVector ballPos = objects->physicsEngine->GetBallLocation(0);
    
    
    
    if (!actualCi->IsChalked())
    {
        velocity = target;
        float actualShotPower = minPower + power * cuePower * (maxPower - minPower);
        velocity *= actualShotPower;
        
        applyingAngular = false;
    }
    else
    {
        //velocity = target;
        float actualCueSpeed = minPower + power * cuePower * (maxPower - minPower);
        FVector cueVelocity = target * actualCueSpeed;
        float cueZVelocity = cueVelocity.Z;
        float offsetSize = offset.SizeSquared();
        float cueThroughBallTravelHalfDistancePreSqrt = 1.0f - offsetSize;
        float cueThroughBallTravelHalfDistance = 0.0f;
        if (cueThroughBallTravelHalfDistancePreSqrt > 1e-5f)
            cueThroughBallTravelHalfDistance = sqrt(cueThroughBallTravelHalfDistancePreSqrt);
        
        
        applyingAngular = true;
        FVector aimPointOnPlane = ballPos - target * objects->physicsEngine->ballR;
        FVector sidewaysAimVec = -FVector::CrossProduct(target, FVector(0.0f, 0.0f, 1.0f));
        sidewaysAimVec.Normalize();
        FVector verticalAimVec = -FVector::CrossProduct(sidewaysAimVec, target);
        aimPointOnPlane += offset.X * sidewaysAimVec + offset.Y * verticalAimVec;
        
        
        
        float projectOnSphereStep = objects->physicsEngine->ballR*(1.0f - cueThroughBallTravelHalfDistance);
        FVector aimPointOnBall = aimPointOnPlane + target * projectOnSphereStep;
        
        float angularSizeAbs = actualCueSpeed / objects->physicsEngine->ballR;
        FVector contactToBallVec = ballPos - aimPointOnBall;
        contactToBallVec.Normalize();
        float verticalCatchAdjustment = FVector::CrossProduct(target, contactToBallVec).Size();
        angularSizeAbs *= verticalCatchAdjustment;
        FVector angularVec = -FVector::CrossProduct(target, aimPointOnBall - ballPos);
        angularVec.Normalize();
        float angularSize = angularSizeAbs * cueEffect;
        //float angularSize = 150.0f * offsetSize * power * cueEffect;
        angularVelocity = angularVec * angularSize;
        
        
        velocity = target * sqrt(actualCueSpeed * actualCueSpeed -
                        2.0f / 5.0f * angularSizeAbs * angularSizeAbs *
                        objects->physicsEngine->ballR * objects->physicsEngine->ballR);
        
        PRINTF((TEXT("Velocity lost on spin: %f, adj %f"), velocity.Size() / actualCueSpeed, verticalCatchAdjustment), Red)
        
        velocity.Z *= -1.0f;
        
        FVector projectionOnCue = velocity.ProjectOnTo(target);
        bool verticalCatch = false;
        if (FVector::DotProduct(target, velocity) < 0.0f)
        {
            PRINTF((TEXT("Vertical catching")), Green)
            velocity -= projectionOnCue;
            verticalCatch = true;
        }
        if (!verticalCatch)
        {
            angularVelocity /= verticalCatchAdjustment;
        }
        
        if (cueThroughBallTravelHalfDistance > 1e-5f)
        {
            PRINTF((TEXT("Trying deflection")), Green)
            FVector deflectionVec = FVector::CrossProduct(FVector::CrossProduct(target, FVector(0.0f, 0.0f, 1.0f)), FVector(0.0f, 0.0f, 1.0f));
            deflectionVec.Normalize();
            float cueBallDeflectionTravel = 1.0f - offsetSize;
            float deflectionSize = cueBallDeflectionTravel / cueThroughBallTravelHalfDistance * actualCueSpeed;
            
            FVector deflection = deflectionVec * deflectionSize;
            
            
            FVector projectionOnDeflection = velocity.ProjectOnTo(deflectionVec);
            
            if (projectionOnDeflection.Size() < deflectionSize)
            {
                PRINTF((TEXT("Deflecting %f"), (projectionOnDeflection.Size() - deflectionSize) / actualCueSpeed), Green)
                velocity += deflectionVec * (projectionOnDeflection.Size() - deflectionSize);
            }
            
        }
        
        
        
        
        velocity.Z *= -1.0f;
        actualCi->Unchalk();
    }
    
    PRINTF((TEXT("Shooting with velocity: %f"), velocity.Size()), Yellow)
    
    GetWorld()->GetAuthGameMode<APoolGameModeBase>()->StartingPhysicsSimulationForShot(pc);
    objects->shotinfo->ResetForNextShot();
    objects->physicsEngine->ApplyLinearVelocity(0, velocity);
    objects->sound->EmitSound_ClientOrServer(E_PoolSounds::CUE_HIT, ballPos, power);
    if (applyingAngular)
        objects->physicsEngine->ApplyAngularVelocity(0, angularVelocity);
    
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        UBaseBackendInfo* backend_data = Cast<UBaseBackendInfo>(gi->backend_data);
        FBackendPlayerInfo* player_info = backend_data->GetPlayerInfo(ps->user_id);
        if (player_info)
        {
            player_info->durability.ChangeConsumableDurability(EConsumableType::CUE, ps->cueInfo.GetCueNum(), -1);
            
            pc->UpdateShop(EConsumableType::CUE, ps->cueInfo.GetCueNum(), player_info->durability.GetDurability(EConsumableType::CUE, ps->cueInfo.GetCueNum()));
        }
    }
    else
    if (ps->GetItemDurability().ChangeConsumableDurability(EConsumableType::CUE, ps->cueInfo.GetCueNum(), -1))
        pc->UpdateShop(EConsumableType::CUE, ps->cueInfo.GetCueNum(), ps->GetItemDurability().GetDurability(EConsumableType::CUE, ps->cueInfo.GetCueNum()));
    
    
    ps->UpdateClient();
}


/*
 void AUserInputValidation::ValidateShot(APoolPlayerControllerBase* pc,
                   FVector target,
                   FVector2D offset,
                   float power)
 {
     const float maxPower = MAX_BALL_LINEAR_VELOCITY;
     const float minPower = MAX_BALL_LINEAR_VELOCITY / 20.0f;
     
     if (!HasAuthority()) return;
     
     if (!GetWorld()->GetAuthGameMode<APoolGameModeBase>()->CanPlayerShoot(pc)) return;
     
     
     APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (pc->PlayerState);
     FCueInfo ci;
     ci.ChangeCueNum(1);
     FCueInfo* actualCi = ps ? &(ps->cueInfo) : &ci;
     const float cuePower = actualCi->GetPower();
     const float cueEffect = actualCi->GetPower();
     
     
     PRINTF((TEXT("%s"), *offset.ToString()), Green)
     
     target.Normalize();
     // Checking cue - table angle
     //if (target.Z*target.Z > target.X*target.X + target.Y*target.Y) return;
     // Checking offset
     if (offset.SizeSquared() > 1.0f)
         offset.Normalize();
     
     FVector ballPos = objects->physicsEngine->GetBallLocation(0);
     
     FVector velocity = target;
     float actualShotPower = minPower + power * cuePower * (maxPower - minPower);
     velocity *= actualShotPower;
     
     FVector angularVelocity;
     bool applyingAngular = false;
     
     if (actualCi->IsChalked()) // check for collisions
     {
         float offsetSize = offset.SizeSquared();
         offset.Normalize();
         offset /= 2.0f;
         
         PRINTF((TEXT("Chalked")), White)
         applyingAngular = true;
         FVector aimPointOnPlane = ballPos - target * objects->physicsEngine->ballR * 0.5f;
         FVector sidewaysAimVec = -FVector::CrossProduct(target, FVector(0.0f, 0.0f, 1.0f));
         sidewaysAimVec.Normalize();
         FVector verticalAimVec = -FVector::CrossProduct(sidewaysAimVec, target);
         aimPointOnPlane += offset.X * sidewaysAimVec + offset.Y * verticalAimVec;
         
         float projectOnSphereStep = objects->physicsEngine->ballR*(1.0f - 0.25f);
         FVector aimPointOnSphere = aimPointOnPlane + target * projectOnSphereStep;
         
         
         FVector angularVec = -FVector::CrossProduct(target, aimPointOnSphere - ballPos);
         angularVec.Normalize();
         float angularSize = 150.0f * offsetSize * power * cueEffect;
         angularVelocity = angularVec * angularSize;
         
         actualCi->Unchalk();
     }
    auto* ps_child = dynamic_cast<APool8BallPlayerState*>(ps);
    if (ps_child)
    {
        if (ps_child->GetItemDurability().ChangeCueDurability(actualCi->cueNum - 1, -1))
        {
            UE_LOG(LogTemp, Warning, TEXT("Current cue %d"), actualCi->cueNum);
            for (auto cue : ps_child->GetItemDurability().cue_durability)
            {
                UE_LOG(LogTemp, Warning, TEXT("%d"), cue);
            }
            ps->UpdateClient();
            FItemDurability transfer_item_durability = ps_child->GetItemDurability();
            ps_child->TransferItemDurabilityToGameInstance(transfer_item_durability);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Error when logging shot"));
        }
    }
     objects->shotinfo->ResetForNextShot();
     objects->physicsEngine->ApplyLinearVelocity(0, velocity);
     if (applyingAngular)
         objects->physicsEngine->ApplyAngularVelocity(0, angularVelocity);
 }*/
    
void AUserInputValidation::ValidateChalkApplication(APoolPlayerControllerBase* pc)
{
    if (!HasAuthority()) return;
    if (!pc || !pc->PlayerState) return;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (pc->PlayerState);
    if (!ps) return;
    ps->cueInfo.ChalkUp();
    // TODO create variable which content current chalk
    //UE_LOG(LogTemp, Warning, TEXT("CHALK NUM %d"), ps->cueInfo.GetChalkNum());
    
    
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        UBaseBackendInfo* backend_data = Cast<UBaseBackendInfo>(gi->backend_data);
        FBackendPlayerInfo* player_info = backend_data->GetPlayerInfo(ps->user_id);
        if (player_info)
        {
            player_info->durability.ChangeConsumableDurability(EConsumableType::CUE, ps->cueInfo.GetCueNum(), -1);
            player_info->durability.ChangeConsumableDurability(EConsumableType::CHALK, ps->cueInfo.GetChalkNum(), -1);
            pc->UpdateShop(EConsumableType::CHALK, ps->cueInfo.GetChalkNum(), player_info->durability.GetDurability(EConsumableType::CHALK, ps->cueInfo.GetChalkNum()));
        }
    }
    else
    if (ps->GetItemDurability().ChangeConsumableDurability(EConsumableType::CHALK, ps->cueInfo.GetCueNum(), -1))
        pc->UpdateShop(EConsumableType::CHALK, ps->cueInfo.GetChalkNum(), ps->GetItemDurability().GetDurability(EConsumableType::CHALK, ps->cueInfo.GetChalkNum()));
        
    ps->UpdateClient();
    //PRINTF((TEXT("Chalking %d"), ps->GetItemDurability().GetDurability(EConsumableType::CHALK, ps->cueInfo.GetChalkNum())), White)
}


void AUserInputValidation::ValidateCueChange(APoolPlayerControllerBase* pc, int num)
{
    if (!HasAuthority()) return;
    if (!pc || !pc->PlayerState) return;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (pc->PlayerState);
    if (!ps) return;
    ps->SelectCue(num);
    ps->UpdateClient();
}

void AUserInputValidation::ValidateChalkChange(APoolPlayerControllerBase* pc, int num)
{
    if (!HasAuthority()) return;
    if (!pc || !pc->PlayerState) return;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (pc->PlayerState);
    if (!ps) return;
    ps->SelectChalk(num);
    ps->UpdateClient();
}

void AUserInputValidation::ValidateBallInHandNewCueBallPos(APoolPlayerControllerBase* pc, FVector pos)
{
    UE_LOG(LogTemp, Warning, TEXT("ValidateBallInHandNewCueBallPos"))
    if (!pc)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot move ball: no PC"))
        return;
    }
    if (!GetWorld()->GetAuthGameMode<APoolGameModeBase>()->CanPlayerShoot(pc))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot move ball: player cannot shoot"))
        return;
    }
    if (!Cast<APoolPlayerStateBase>(pc->PlayerState) ||
        !Cast<APoolPlayerStateBase>(pc->PlayerState)->ballInHand)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot move ball: no ball in hand"))
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("starting MoveCueBallIfPositionValid"))
    objects->physicsEngine->MoveCueBallIfPositionValid(pos);
}

void AUserInputValidation::ValidateConsumableRecharge(APoolPlayerControllerBase* pc, EConsumableType type, int num)
{
    if (!HasAuthority()) return;
    if (!pc || !pc->PlayerState) return;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (pc->PlayerState);
    if (!ps) return;
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        UBaseBackendInfo* backend_data = Cast<UBaseBackendInfo>(gi->backend_data);
        FBackendPlayerInfo* player_info = backend_data->GetPlayerInfo(ps->user_id);
        if (player_info)
        {
            player_info->durability.RechargeConsumable(type, num);
            pc->UpdateShop(type, num, player_info->durability.GetDurability(type, num));
        }
    }
    else
    if (ps->GetItemDurability().RechargeConsumable(type, num))
        pc->UpdateShop(type, num, ps->GetItemDurability().GetDurability(type, num));
    
    ps->UpdateClient();
}
