// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsEngine.h"
#include "ObjectGatherer.h"
#include "PoolGameStateBase.h"
#include "SefaPool/8Ball/BallInfo.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

BallPhysicsHelper::BallPhysicsHelper() :
    ball                  (nullptr),
    world                 (nullptr),
    objects               (nullptr),
    position              (0.0f),
    rotation              (),
    linearVelocity        (0.0f),
    angularVelocity       (0.0f),
    tableZ                (0.0f),
    active                (false),
    affected              (false),
    isMoving              (false),
    evaluateHit           (false),
    frameTime             (0.0f),
    movingAverageVelocity (0.0f),
    touchesCloth          (0.0f),
    caught                (false),
    caughtPocketNum       (-1),
    singleSound           (false),
    linearDamping         (0.0f),
    angularDamping        (0.0f)
{}

float BallPhysicsHelper::R()
{
    return ball->scale * 50.0f;
}

void BallPhysicsHelper::Populate(ABall* ball_, UWorld* world_, AObjectGatherer* objects_, float tableZ_)
{
    ball = ball_;
    world = world_;
    objects = objects_;
    tableZ = tableZ_;
}

UWorld* BallPhysicsHelper::GetWorld()
{
    return world;
}

void BallPhysicsHelper::Clear()
{
    //position = ball->mesh->GetComponentLocation();
    //rotation = ball->mesh->GetComponentRotation();
    //linearVelocity = ball->mesh->GetPhysicsLinearVelocity();
    //angularVelocity = ball->mesh->GetPhysicsAngularVelocityInRadians();
    
    float newAverageVelocity = (linearVelocity.Size() + 10.0f * angularVelocity.Size() * R())/2.0f;
    if (movingAverageVelocity < objects->settings->stopVelocity &&
        newAverageVelocity > objects->settings->stopVelocity)
        movingAverageVelocity = newAverageVelocity;
    else
        movingAverageVelocity = movingAverageVelocity * 0.9f + newAverageVelocity * 0.1f;
    
    if (movingAverageVelocity > objects->settings->stopVelocity * 2.0f)
    {
        // UE_LOG(LogTemp, Warning, TEXT("velocity remaining %d: %f"), ball->ballNum, movingAverageVelocity / objects->settings->stopVelocity)
    }
    
    if (movingAverageVelocity < objects->settings->stopVelocity * 2.0f)
    {
        linearVelocity  = FVector(0.0f);
        angularVelocity = FVector(0.0f);
        if (isMoving)
        {
            movingAverageVelocity = 0.0f;
            Stop();
        }
    }/*
    else
    if (linearVelocity.Size() < objects->settings->startDecelerationVelocity)
    {
        linearDamping = objects->settings->slowingLinearDamping;
        angularDamping = objects->settings->slowingAngularDamping;
    }*/
    else if (!isMoving)
    {
        StartMoving();
        linearDamping = objects->settings->defaultLinearDamping;
        angularDamping = objects->settings->defaultAngularDamping;
    }
    
    active = true;
    if (ball->Pocketed()) active = false;
    affected = false;
    //isMoving = ball->isMoving;
    evaluateHit = false;
    if (!active)
        caught = false;
}

void BallPhysicsHelper::CheckHit(float frameTime_)
{
    if (!active) return;
    frameTime = frameTime_;
    if (!isMoving)
    {
        evaluateHit = false;
        return;
    }
    
    FHitResult tableHit;

    FCollisionQueryParams queryParams;
    queryParams.bTraceComplex = true;
    for (int i = 0; i < objects->physicsEngine->BallCountInGame; i++)
        queryParams.AddIgnoredActor(objects->balls[i]);

    FCollisionResponseParams respParams;
    
    bool hasHit = GetWorld()->SweepSingleByChannel(tableHit,
                                                   position,
                                                   position + frameTime * linearVelocity,
                                                   FQuat::Identity,
                                                   ECollisionChannel::ECC_Visibility,
                                                   FCollisionShape::MakeSphere(R()),
                                                   queryParams,
                                                   respParams);
    
    if (dynamic_cast<ATable*>(tableHit.Actor.Get()))
        evaluateHit = true;
    else
    if (dynamic_cast<ATablePockets*>(tableHit.Actor.Get()))
    {
        //PRINTF((TEXT("Close to pocket %d"), ball->ballNum), Yellow)
        evaluateHit = false;
    }
    else
    if (dynamic_cast<ATableWalls*>(tableHit.Actor.Get()))
        evaluateHit = true;
    else
        evaluateHit = false;
    print = false;
}
void BallPhysicsHelper::ProcessGravity(float dt, FVector& targetPos)
{
    if (!active || !isMoving) return;
    
    float tableRestitution = 0.5f;
    const float clothTolerance = (1.0f / GAME_UNIT_IN_CM) / 100.0f;
    float G = FREE_FALL_ACCELERATION;
    
    if (objects->gi)
    {
        tableRestitution = objects->gi->data->settings->GetRestrictionBallCloth();
        G = objects->gi->data->settings->GetAccelerationOfGravity();
    }
    
    float lowestZ = tableZ;
    bool pocketed = false;
    FVector lateralAcceleration(0.0f);
    objects->tablePockets->GetZLevelForBallAndCheckIfPocketed(this, lowestZ, pocketed, lateralAcceleration);
    /*if (!ball->ballNum && linearVelocity.Z > 1e-5f)
        PRINTF((TEXT("Non zero Z velocity %f"), linearVelocity.Z), White)
    if (!ball->ballNum)
        PRINTF((TEXT("Z pos %e"), targetPos.Z - (lowestZ + clothTolerance / 2.0f)), White)*/
    if (pocketed)
    {
        ball->Pocketed() = true;
        Stop();
        caught = false;
        objects->sound->EmitSound_ClientOrServer(E_PoolSounds::DROP, position, linearVelocity.Size() / MAX_BALL_LINEAR_VELOCITY);
        ball->AfterPocketed();
        isMoving = false;
        active = false;
        return;
    }
    
    if (caught)
    {
        if (singleSound)
        {
            singleSound = true;
        }
        
    }
    else
    if (position.Z > tableZ + objects->physicsEngine->GetCushionHeight() && !objects->physicsEngine->OnTable(objects->physicsEngine->WorldToTableCoords(position)))
    {
        lowestZ = tableZ + objects->physicsEngine->GetCushionHeight();
        PRINTF((TEXT("CUSHION %f %f"), position.Z, lowestZ), White)
    }
    
    linearVelocity += G * lateralAcceleration * dt;
    //targetPos += G * lateralAcceleration * dt * dt;
    /*if (caught)
    {
        targetPos.Z = lowestZ;
        linearVelocity.Z = 0.0f;
        touchesCloth = true;
    }
    else*/
    if (/*position.Z > lowestZ + clothTolerance && */targetPos.Z < lowestZ)
    {
        //if (print)
        targetPos.Z = lowestZ + lowestZ - targetPos.Z;
        linearVelocity.Z *= -tableRestitution;
        touchesCloth = false;
    }
    else if ((targetPos.Z < lowestZ + clothTolerance &&
             targetPos.Z > lowestZ &&
             linearVelocity.Z < objects->settings->stopVelocity) ||
             position.Z < lowestZ)
    {
        linearVelocity.Z = 0.0f;
        targetPos.Z = lowestZ + clothTolerance / 2.0f;
        touchesCloth = true;
        if (!ball->ballNum)
            PRINTF((TEXT("Touching cloth")), White)
    }
    else if (position.Z > lowestZ + clothTolerance)
    {
        //PRINTF((TEXT("G++")), White)
        linearVelocity.Z += -G * dt;
        touchesCloth = false;
    }
    else
    {
        touchesCloth = true;
        linearVelocity.Z = 0.0f;
        targetPos.Z = lowestZ + clothTolerance / 2.0f;
    }
}

void BallPhysicsHelper::ProcessRotation(float dt)
{
    if (!active || !isMoving) return;
    if (caught)
        return;
    float ballClothSlidingResistance = 0.125f;
    float ballClothRollingResistance = 0.0075f;
    float angularDecelerationRate = 10.0f; //rad/s^2
    float slidingResistanceVelocityDeltaTrigger = 1.0f; //mm/s
    float inertiaMassDistribution = 2.5f; //mm/s
    
    if (objects->gi)
    {
        ballClothSlidingResistance = objects->gi->data->settings->GetSlipFrictionBallTable();
        ballClothRollingResistance = objects->gi->data->settings->GetRollingFriction();
        angularDecelerationRate = objects->gi->data->settings->GetVerticalAngleAngularDemping();
        slidingResistanceVelocityDeltaTrigger = objects->gi->data->settings->GetSlidingResistanceVelocityDeltaTrigger();
        inertiaMassDistribution = objects->gi->data->settings->GetCoefBallMassDistribution();
    }
    
    
    const float G = FREE_FALL_ACCELERATION;
    const float slidingResistanceAcc = G * ballClothSlidingResistance;
    const float rollingResistanceAcc = G * ballClothRollingResistance;
    const float R_ = R();
    FVector normalizedRotationAxis = angularVelocity;
    normalizedRotationAxis.Normalize();
    FQuat additionalRotation (normalizedRotationAxis, angularVelocity.Size() * dt);
    rotation = additionalRotation * rotation;
    if (touchesCloth)
    {
        FVector speedAtTouchPoint = linearVelocity +
            FVector::CrossProduct(angularVelocity, FVector (0.0f, 0.0f, -R_));
        speedAtTouchPoint.Z = 0.0f;
        if (speedAtTouchPoint.Size() > slidingResistanceVelocityDeltaTrigger) //sliding, 1 mm/s relative speed
        {
            FVector speedNormal = speedAtTouchPoint;
            speedNormal.Normalize();
            FVector dV = -speedNormal * slidingResistanceAcc * dt;
            if (dV.Size() > speedAtTouchPoint.Size())
            {
                dV = -speedAtTouchPoint;
            }
            linearVelocity += dV;
            FVector dOmega = FVector::CrossProduct(FVector(0.0f, 0.0f, -R_), dV);
            dOmega.Normalize();
            dOmega *= slidingResistanceAcc * inertiaMassDistribution * dt / R_;
            angularVelocity += dOmega;
        }
        if (true)
        {
            FVector speedNormal = linearVelocity;
            speedNormal.Z = 0.0f;
            speedNormal.Normalize();
            
            FVector dV = -speedNormal * rollingResistanceAcc * dt;
            if (dV.Size() > linearVelocity.Size())
            {
                dV = -linearVelocity;
            }
            linearVelocity += dV;
            FVector dOmega = FVector::CrossProduct(FVector(0.0f, 0.0f, -R_), dV);
            dOmega.Normalize();
            dOmega *= rollingResistanceAcc * inertiaMassDistribution * dt / R_;
            angularVelocity -= dOmega;
        }
        
        if (fabs (angularVelocity.Z) >=  angularDecelerationRate * dt)
            angularVelocity.Z -= ((angularVelocity.Z > 0.0f) ? 1.0f : -1.0f) * angularDecelerationRate * dt;
        else
            angularVelocity.Z = 0.0f;
        
    }
}

bool BallPhysicsHelper::BallToBandInteraction(FHitResult& hit, float dt, float timeBeforeHit)
{
    if (!active || !isMoving) return true;
    if (!caught)
        objects->shotinfo->AddToHitLog(ball->ballNum, SIDERAIL_HIT, hit.ImpactPoint, objects->shotinfo->GetTimeSinceShotBegan());


    FVector velocity = linearVelocity;
    FVector angular = angularVelocity;
    FVector normal = hit.ImpactNormal;


    if (position.Z > tableZ + objects->physicsEngine->GetCushionHeight() * 0.95f || 
        normal.Z / FMath::Sqrt(normal.X * normal.X + normal.Y * normal.Y) > 5)
    {
        if (!ball->ballNum)
            PRINTF((TEXT("Dropping")), White)
        return false;
    }
    
    normal.Z = 0.0f;
    normal.Normalize();
    float normalSign = FVector::DotProduct(velocity, normal) > 0.0f ? 1.0f : -1.0f;
    normal *= normalSign;
    
    FVector normalVelocity = velocity.ProjectOnToNormal(normal);
    FVector tangentVelocity = velocity - normalVelocity;
    
    FVector normalAngular = angular.ProjectOnToNormal(normal);
    FVector tangentAngular = angular - normalAngular;
    
    FVector r = hit.ImpactPoint - hit.Location;
    r.Normalize();
    r *= R();
    
    float normalVelocityLoss = 0.3f;
    float ballSurfaceVelocityLossCoeff = 2.32e-3f;
    float lostVelocityAtHitLocationToLinearTransfer = 0.36f;
    float cushionHeightProfile = 5.0f;
    if (objects->gi)
    {
        normalVelocityLoss = objects->gi->data->settings->GetNormalVelocityLossValue();
        ballSurfaceVelocityLossCoeff = objects->gi->data->settings->GetCoefBallSurfaceVelocityLoss();
        lostVelocityAtHitLocationToLinearTransfer = objects->gi->data->settings->GetCoefLossVelocityIntoLossLinearVelocity();
        cushionHeightProfile = objects->gi->data->settings->GetCoefSidewallProfile();
    }
    
    
    normalVelocity *= - (1.0f - normalVelocityLoss);
    //normalAngular = normalAngular;
    
    FVector ballSurfaceVelocityAtHitLocation = tangentVelocity + FVector::CrossProduct(tangentAngular, r);
    
    if (ballSurfaceVelocityAtHitLocation.Size() > 1e-5f)
    {
        /*float retainedVelocityAtHitLocation = 1.0f - FVector::DotProduct(velocity, normal) / velocity.Size();
        
            UE_LOG(LogTemp, Warning, TEXT("Targeting %f"), retainedVelocityAtHitLocation);
        FVector newVelocityAtHitLocation = ballSurfaceVelocityAtHitLocation;
        
        FVector dV = ballSurfaceVelocityAtHitLocation / 100.0f;
        FVector dW = tangentAngular;
        dW.Normalize();
        dW *= dV.Size() * 2.5f / R();
        const int limit = 100;
        int done = 0;
        float prev = 1.0f;
        while (newVelocityAtHitLocation.Size() > retainedVelocityAtHitLocation * ballSurfaceVelocityAtHitLocation.Size() && done < limit)
        {
            UE_LOG(LogTemp, Warning, TEXT("%f"), newVelocityAtHitLocation.Size() / ballSurfaceVelocityAtHitLocation.Size());
            tangentVelocity -= dV;
            tangentAngular -= dW;
            float newC = newVelocityAtHitLocation.Size() / ballSurfaceVelocityAtHitLocation.Size();;
            if (newC > prev) break;
            newVelocityAtHitLocation = tangentVelocity + FVector::CrossProduct(tangentAngular, r);
            prev = newC;
            done++;
        }*/
        
        float lostVelocityAtHitLocation = ballSurfaceVelocityLossCoeff / GAME_UNIT_IN_CM * normalVelocity.Size();
        float alpha = lostVelocityAtHitLocation;
        if (alpha > 1.0f) alpha = 1.0f;
        float beta = lostVelocityAtHitLocationToLinearTransfer;
        
        //FVector resultingBallSurfaceVelocityAtHitLocation = ballSurfaceVelocityAtHitLocation * alpha;
        FVector lostBallSurfaceVelocityToLinearTransfer = - ballSurfaceVelocityAtHitLocation * alpha * beta;
        
        lostBallSurfaceVelocityToLinearTransfer.Z /= cushionHeightProfile;
        
        tangentVelocity += lostBallSurfaceVelocityToLinearTransfer;
        
        FVector resultingVelocityAtHitLocation = ballSurfaceVelocityAtHitLocation * (1.0f - alpha);
        FVector resultingLinearizedAngularVelocityAtHitLocation = resultingVelocityAtHitLocation - tangentVelocity;
        
        tangentAngular = FVector::CrossProduct(r, resultingLinearizedAngularVelocityAtHitLocation) / r.SizeSquared();
        //tangentAngular *= beta;
    }
    
    velocity = tangentVelocity + normalVelocity;
    angular = normalAngular + tangentAngular;
    
    objects->sound->EmitSound_ClientOrServer(E_PoolSounds::CUSHION, hit.ImpactPoint, normalVelocity.Size() / MAX_BALL_LINEAR_VELOCITY);
    
    FVector targetPos = hit.Location + velocity * (dt - timeBeforeHit);
    
    /*FVector velocity = linearVelocity;
    FVector normal = hit.ImpactNormal;
    normal.Z = 0.0f;
    normal.Normalize();
    float normalSign = FVector::DotProduct(velocity, normal) > 0.0f ? 1.0f : -1.0f;
    normal *= normalSign;
    FVector normalVelocity = velocity.ProjectOnToNormal(normal);
    FVector tangentVelocity = velocity - normalVelocity;

    FVector targetTangentVelocity = tangentVelocity;
    FVector targetPos = hit.Location + velocity * (dt - timeBeforeHit);

    FVector angular = angularVelocity;
    FVector r = hit.ImpactPoint - hit.Location;
    r.Normalize();
    r *= R();
    
    //const float retainFunctionMix = 0.9f;
    const float normalVelocityCoeff = normalVelocity.Size() / MAX_BALL_LINEAR_VELOCITY;*/
    /*const float retainedVelocityAtHitLocation = (normalVelocityCoeff > 1.0f) ? 0.01f :
        retainFunctionMix          * 1.0f / (1.0f + 99.0f * normalVelocityCoeff) +
        (1.0f - retainFunctionMix) * (-0.99f * normalVelocityCoeff + 1.0f);*/
    /*float retainedVelocityAtHitLocation = FVector::DotProduct(velocity, normal) / velocity.Size();
    retainedVelocityAtHitLocation = 1.0f - FMath::Clamp(retainedVelocityAtHitLocation, 0.0f, 1.0f);
    const float velocityAtHitLocationToTangentTransmission = sqrt((1.0f - retainedVelocityAtHitLocation*retainedVelocityAtHitLocation) / 5.0f) / 2.0f;
    PRINTF((TEXT("Retained vel %f"), retainedVelocityAtHitLocation), White)

    FVector tangentVelocityAtHitLocation = tangentVelocity + FVector::CrossProduct(angular, r);
    
    FVector horizontalVelocityAtHitLocation = tangentVelocityAtHitLocation;
    horizontalVelocityAtHitLocation.Z = 0.0f;*/
    
    
    /*if (caught)
    {
        velocity = targetTangentVelocity - objects->settings->normalVelocityLoss * normalVelocity;
        position = targetPos;
        return;
    }*/
    
    /*targetTangentVelocity -=  horizontalVelocityAtHitLocation * (1.0f - retainedVelocityAtHitLocation) * velocityAtHitLocationToTangentTransmission;
    velocity = targetTangentVelocity - objects->settings->normalVelocityLoss * normalVelocity;*/
    //velocity = tangentVelocity - objects->settings->normalVelocityLoss * normalVelocity;
    /*FVector targetAngularVelocityInLinearSpace = -retainedVelocityAtHitLocation * horizontalVelocityAtHitLocation + FVector(0.0f, 0.0f, 1.0f) * tangentVelocityAtHitLocation.Z * retainedVelocityAtHitLocation;
    angular = FVector::CrossProduct(r, targetAngularVelocityInLinearSpace) / r.SizeSquared();
    angular += angularVelocity.ProjectOnToNormal(normal);*/
    //angular.Z *= retainedVelocityAtHitLocation;

    /*if (!caught)
        velocity.Z *= 0.1f;
    else*/
    if (caught)
    {
        velocity.X *= 0.25f;
        velocity.Y *= 0.25f;
        if (velocity.Z > 0.0f)
            velocity.Z *= 0.25f;
    }

    linearVelocity = velocity;
    angularVelocity = angular;
    affected = true;
    evaluateHit = false;
    
    if (caught && targetPos.Z > position.Z)
    {
        targetPos.Z = position.Z;
    }
    position = targetPos;
    CheckHit(frameTime);
    return true;
}


void BallPhysicsHelper::ProcessStep(float dt)
{
    if (!active || !isMoving) return;
    FVector targetPos = position + linearVelocity * dt;
    if (!ball->ballNum)
        PRINTF((TEXT("Velocity %s"), *linearVelocity.ToString()), White)
    
    ProcessGravity(dt, targetPos);
    ProcessRotation(dt);
    if (caught && targetPos.Z > position.Z)
    {
        targetPos.Z = position.Z;
    }
    if ((!evaluateHit && !caught) || ball->Pocketed())
    {
        position = targetPos;
        return;
    }
    if (caught)
    {
        position = targetPos;
        objects->tablePockets->EvaluateCaughtPostStep(this);
        return;
    }
    
    FHitResult hit;

    FCollisionQueryParams queryParams;
    queryParams.bTraceComplex = true;
    queryParams.AddIgnoredActor(ball);

    FCollisionResponseParams respParams;
    
    
    
    bool hasHit = GetWorld()->SweepSingleByChannel(hit,
        position,
        position + linearVelocity * frameTime,
        FQuat::Identity,
        ECollisionChannel::ECC_Visibility,
        FCollisionShape::MakeSphere(R()),
        queryParams,
        respParams);
    if (!hasHit || caught)
    {
        position = targetPos;
        return;
    }
    
    float timeBeforeHit = hit.Distance / linearVelocity.Size();
    
    if (dynamic_cast<ATable*>(hit.Actor.Get()))
    {
        if (!ball->CanCollideWithTable())
        {
            position = targetPos;
            return;
        }
        
        ATable* hitTable = dynamic_cast<ATable*>(hit.Actor.Get());
        
        //PRINTF((TEXT("Found table hit %f/%f"), 1000000.0f * timeBeforeHit, 1e6f * dt), White)
        if (timeBeforeHit < dt)
        {
            if (!BallToBandInteraction(hit, dt, timeBeforeHit))
                position = targetPos;
            return;
        }
    }
    else
    /*if (dynamic_cast<ATablePockets*>(hit.Actor.Get()))
    {
        active = false;
        if (timeBeforeHit < dt)
        {
            PRINTF((TEXT("Pocketed in physics %d"), ball->ballNum), Yellow)
            objects->sound->EmitDropSound(position, linearVelocity.Size() / MAX_BALL_LINEAR_VELOCITY);
            ball->Pocketed() = true;
            ball->AfterPocketed();
            isMoving = false;
            return;
        }
    }
    else*/
    if (dynamic_cast<ATableWalls*>(hit.Actor.Get()))
    {
        if (timeBeforeHit < dt)
        {
            ball->AfterKnocked();
            isMoving = false;
            active = false;
            return;
        }
    }
    
    position = targetPos;
    
}

void BallPhysicsHelper::ApplyToWorld()
{
    ball->SetActorLocation(position, false, nullptr, ETeleportType::TeleportPhysics);
    ball->mesh->SetWorldRotation(FRotator(rotation), false, nullptr, ETeleportType::TeleportPhysics);
    ball->UpdateShadows();
    //ball->PushVelocity(linearVelocity);
}

void BallPhysicsHelper::ApplyLinearVelocity(FVector vel)
{
    StartMoving();
    linearVelocity = vel;
    movingAverageVelocity = (linearVelocity.Size() + angularVelocity.Size() * R())/2.0f;
    objects->shotinfo->BallMoved(ball->ballNum);
}
void BallPhysicsHelper::ApplyAngularVelocity(FVector vel)
{
    StartMoving();
    angularVelocity = vel;
    movingAverageVelocity = (linearVelocity.Size() + angularVelocity.Size() * R())/2.0f;
}
void BallPhysicsHelper::PlaceAndStop(FVector pos, FRotator rot)
{
    position = pos;
    rotation = rot.Quaternion();
    isMoving = false;
    linearVelocity = FVector(0.0f);
    angularVelocity = FVector(0.0f);
    ApplyToWorld();
}

void BallPhysicsHelper::Stop()
{
    isMoving = false;
    linearVelocity  = FVector(0.0f);
    angularVelocity = FVector(0.0f);
    objects->shotinfo->BallStopped(ball->ballNum);
}

void BallPhysicsHelper::StartMoving()
{
    isMoving = true;
    objects->shotinfo->BallMoved(ball->ballNum);
}


// Sets default values
APhysicsEngine::APhysicsEngine() :
    objects               (nullptr),
    ballsInfo             (),
    ballR                 (3.0f),
    ballRestingOnTableZ   (0.0f),
    halfTableCueToDiamond (),
    MovedBallInx          (0)
{
	PrimaryActorTick.bCanEverTick = true;
}

void APhysicsEngine::InitializePhysicEngine()
{
    for (int i = 0; i < BallCountInGame; i ++)
        ballsInfo[i].Populate(objects->balls[i], GetWorld(), objects, ballRestingOnTableZ);
    objects->tablePockets->SetBaseZLevel(ballRestingOnTableZ - ballR);
    PrimaryActorTick.SetTickFunctionEnable(true);
    
    ballR = objects->balls[0]->scale * 50.0f;
    
    FVector defCue  = objects->table->GetCueBallDefaultPos();
    FVector defFoul = objects->table->GetFoulDefaultPos();
    ballRestingOnTableZ = (defCue.Z + defFoul.Z) / 2.0f +
                           ballR;
}

// Called when the game starts or when spawned
void APhysicsEngine::BeginPlay()
{
	Super::BeginPlay();
    GameState = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
    PrimaryActorTick.SetTickFunctionEnable(false);
    GATHER_OBJECT_GATHERER(objects)
    // for (int i = 0; i < BallCountInGame; i ++)
    //     ballsInfo.Add(BallPhysicsHelper());
    UE_LOG(LogTemp,Error,TEXT("Ball info set"))
    
    FVector defCue  = objects->table->GetCueBallDefaultPos();
    FVector defFoul = objects->table->GetFoulDefaultPos();
    ballR = objects->balls[0]->scale * 50.0f;
    ballRestingOnTableZ = (defCue.Z + defFoul.Z) / 2.0f +
                           ballR;
    
    halfTableCueToDiamond.X = defFoul.X - defCue.X;
    halfTableCueToDiamond.Y = defFoul.Y - defCue.Y;

    
}

void APhysicsEngine::PreTick(float frameTime)
{
    for (int i = 0; i < BallCountInGame; i++)
    {
        ballsInfo[i].Clear();
        ballsInfo[i].CheckHit(frameTime);
    }
}

void APhysicsEngine::Tick(float frameTime)
{
	Super::Tick(frameTime);
    PreTick(frameTime);
    
    //PRINTF((TEXT("Cue ball pos %s"), *WorldToTableCoords(GetBallLocation(0)).ToString()), White)
    
    if (!HasAuthority()) return;
    if (objects->ballinfo->bShouldGetInfo)
    {
        
        FBallsInfo BallsInfoInstance = FBallsInfo();
        for (int i = 0; i < BallCountInGame; i++)
        {
            BallsInfoInstance.BallPos[i] = ballsInfo[i].position;
            BallsInfoInstance.BallRotation[i] = ballsInfo[i].rotation.Rotator();
        }
        
        BallsInfoInstance.time = objects->shotinfo->GetTimeSinceShotBegan();
        BallsInfoInstance.game_time = GameState->GameTime;
        UE_LOG(LogTemp,Error,TEXT("save time %i"),GameState->GameTime)
        objects->ballinfo->BallsInfoArray.Add(BallsInfoInstance);
        
    }
    
    //const float subRadiusPerSubstep = 100.0f;
    //const int frameSubdivs = int(frameTime * subRadiusPerSubstep * maxSpeed / ballR) + 3;
    float substepSize = 1.0f;
    if (objects->gi)
        substepSize = objects->gi->data->settings->GetPhysSubstepSize();
    const int frameSubdivs = int(1000.0f * frameTime / substepSize) + 1;
    
    const float dt = frameTime / (frameSubdivs * 1.0f);
    for (int substep = 0; substep < frameSubdivs; substep++)
    {
        if (!objects->shotinfo->inShot) return;
        
        for (int i = 0; i < BallCountInGame; i ++)
            ballsInfo[i].ProcessStep(dt);
        
        for (int i = 0; i < BallCountInGame; i ++)
        {
            if (!ballsInfo[i].active) continue;
            for (int j = i + 1; j < BallCountInGame; j++)
            {
                if (!ballsInfo[i].active) continue;
                FVector i_to_j_vec = ballsInfo[j].position - ballsInfo[i].position;
                float dist = i_to_j_vec.Size();
                i_to_j_vec.Normalize();
                if (dist < 2.0f * ballR * 0.99f && (ballsInfo[i].isMoving || ballsInfo[j].isMoving))
                {
                    
                    
                    objects->shotinfo->AddToHitLog(i, j, ballsInfo[i].position + i_to_j_vec / 2.0f, objects->shotinfo->GetTimeSinceShotBegan());
                    
                    ballsInfo[i].affected = true;
                    ballsInfo[j].affected = true;
                    ballsInfo[i].StartMoving();
                    ballsInfo[j].StartMoving();
                    
                    FVector centers = i_to_j_vec;
                    FVector thisVel = ballsInfo[i].linearVelocity;
                    FVector thatVel = ballsInfo[j].linearVelocity;
                    FVector velRel  = thisVel - thatVel;
    
                    objects->sound->EmitSound_ClientOrServer(E_PoolSounds::BALL_TO_BALL,
                                                             ballsInfo[i].position + i_to_j_vec / 2.0f,
                                                             velRel.Size() / MAX_BALL_LINEAR_VELOCITY);
                    ballsInfo[i].position -= i_to_j_vec * (2.0f * ballR - dist) / 2.0f * 1.001f;
                    ballsInfo[j].position += i_to_j_vec * (2.0f * ballR - dist) / 2.0f * 1.001f;
                    if (FVector::DotProduct(velRel, centers) < 0.0f) continue;

                    FVector velNorm = FVector::DotProduct(velRel, centers) * centers;
                    FVector thisNewVelocity = thisVel - velNorm;
                    FVector thatNewVelocity = thatVel + velNorm;


                    /*FVector thisAngular = this->GetAngularVelocitySnap(true);
                    FVector thatAngular = that->GetAngularVelocitySnap(true);
                    FVector thisR = HitLocation - this->mesh->GetComponentLocation();
                    FVector thatR = HitLocation - that->mesh->GetComponentLocation();

                    FVector thisVelocityAtHitLocation = this->GetVelocitySnap(true) - this->GetVelocitySnap(true).ProjectOnToNormal(centers) + FVector::CrossProduct(thisAngular, thisR);
                    FVector thatVelocityAtHitLocation = that->GetVelocitySnap(true) - that->GetVelocitySnap(true).ProjectOnToNormal(centers) + FVector::CrossProduct(thatAngular, thatR);

                    FVector collisionPointsRelativeVelocity = thisVelocityAtHitLocation - thatVelocityAtHitLocation;
                    float relativeVelocityCoeff = 0.0f;
                    if (velRel.Size() < 50.0) relativeVelocityCoeff = 1.0f;
                    else relativeVelocityCoeff = 50.0 / velRel.Size();
                    float inducedThrowCoeff = 0.1f * relativeVelocityCoeff;
                    FVector inducedThrow = collisionPointsRelativeVelocity * inducedThrowCoeff;
                    thatNewVelocity += inducedThrow;
                    thisNewVelocity -= inducedThrow;*/
                    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FVector(relativeVelocityCoeff).ToString());
                    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, thatAngular.ToString());

                    /*float inducedSpinCoeff = 0.1f * relativeVelocityCoeff;
                    FVector thisInducedSpin = FVector::CrossProduct(thisR, collisionPointsRelativeVelocity) * inducedSpinCoeff / thisR.SizeSquared();
                    FVector thatInducedSpin = FVector::CrossProduct(thatR, collisionPointsRelativeVelocity) * inducedSpinCoeff / thatR.SizeSquared();
                    thisAngular -= thisInducedSpin;
                    thatAngular += thatInducedSpin;
                    this->mesh->SetPhysicsAngularVelocityInRadians(thisAngular);
                    that->mesh->SetPhysicsAngularVelocityInRadians(thatAngular);
                    this->NewSnapshot();
                    that->NewSnapshot();*/
                    ballsInfo[i].linearVelocity = thisNewVelocity;
                    ballsInfo[j].linearVelocity = thatNewVelocity;
                    /*ballsInfo[i].linearVelocity.Z *= 0.1f;
                    ballsInfo[j].linearVelocity.Z *= 0.1f;*/
                    
                    ballsInfo[i].CheckHit(frameTime);
                    ballsInfo[j].CheckHit(frameTime);
                }
            }
        }
    }
    
    for (int i = 0; i < BallCountInGame; i++)
    {
        if (/* !ballsInfo[i].affected ||*/ !ballsInfo[i].active)
            continue;
        ballsInfo[i].ApplyToWorld();
    }
}



void APhysicsEngine::ApplyLinearVelocity(int num, FVector vel)
{
    FVector newVel = vel;
    //newVel.Z = 0.0f;
    ballsInfo[num].ApplyLinearVelocity(newVel);
}
void APhysicsEngine::ApplyAngularVelocity(int num, FVector vel)
{
    ballsInfo[num].ApplyAngularVelocity(vel);
}
bool APhysicsEngine::BallIsMoving(int num)
{
    return ballsInfo[num].isMoving;
}

void APhysicsEngine::MoveBallAndStop(int num, FVector pos, FRotator rot)
{
    if (!ballsInfo[num].ball) return;
    ballsInfo[num].PlaceAndStop(pos, rot);
    objects->balls[num]->mesh->SetCollisionProfileName(TEXT("BallPreset"));
    ballsInfo[num].ball->UpdateShadows();
}


FVector APhysicsEngine::GetBallLocation(int num)
{
    return ballsInfo[num].position;
}
FRotator APhysicsEngine::GetBallRotation(int num)
{
    return ballsInfo[num].rotation.Rotator();
}

// Движение шара
void APhysicsEngine::ValidateCueBallLinearMovement(FVector target)
{
   
    
    auto Ruleset = GameState->GetRulesetFunction();
    
    float dl = GAME_UNIT_IN_CM / 10.0f;
    
    FVector prevPos = GetBallLocation(MovedBallInx);
    
    FHitResult hit;

    FCollisionQueryParams queryParams;
    queryParams.bTraceComplex = true;
    queryParams.AddIgnoredActor(ballsInfo[MovedBallInx].ball);

    FCollisionResponseParams respParams;
    
    bool hasHit = GetWorld()->SweepSingleByChannel(hit,
                                                   prevPos,
                                                   target,
                                                   FQuat::Identity,
                                                   ECollisionChannel::ECC_Visibility,
                                                   FCollisionShape::MakeSphere(ballR),
                                                   queryParams,
                                                   respParams);
    if (hasHit) target = hit.Location;
    FVector dl_vec = prevPos - target;
    dl_vec.Normalize();
    dl_vec *= dl;
    bool intersection = true;
    const int max_steps = (prevPos - target).Size() / dl;
    int step = 0;
    
    while (intersection == true && step < max_steps)
    {
    //     // между шарами
    intersection = false;
        // ломает движение шаров c помощью касаний
    //     for (int i = 1; i < BallCountInGame; i++)
    //     {
    //         float distance = (target - ballsInfo[i].position).Size();
    //         if (distance < 2.0f * ballR * 0.99f)
    //         {
    //             intersection = true;
    //             target += dl_vec;
    //             step ++;
    //         }
    //     }
    // луза
    bool pocket = objects->tablePockets->IntersectingPocket(target);
    if (pocket)
    {
        intersection = true;
        target += dl_vec;
        step ++;
    }
        
    // границы
    if (Ruleset && !Ruleset(WorldToTableCoords(target))&& MovedBallInx ==0) 
    {
        intersection = true;
        target += dl_vec;
        step ++;
    }
    }
    if (step == max_steps)
        target = prevPos;
    
    
    FQuat targetRot = ballsInfo[MovedBallInx].rotation;
    
    FVector move = target - prevPos;
    move.Z = 0.0f;
    if (move.SizeSquared() > 1e-5f) {
        float angle = move.Size() / ballR;
        move.Normalize();
        FVector rotAxis = -FVector::CrossProduct(move, FVector(0.0f, 0.0f, 1.0f));
        FQuat addRot(rotAxis, angle);
        targetRot = addRot * targetRot;
    }
    target.Z = ballRestingOnTableZ;
    MoveBallAndStop(MovedBallInx, target, targetRot.Rotator());
    
}

void APhysicsEngine::MoveCueBallIfPositionValid(FVector pos)
{
    
    UE_LOG(LogTemp, Warning, TEXT("MoveCueBallIfPositionValid"))
    APoolGameStateBase* gs = GetWorld()->GetGameState<APoolGameStateBase>();
    if (!gs) ERROR_DEBUG_PRINT_FUNC_INFO("Game state not found")
    
    auto Ruleset = gs->GetRulesetFunction();
    FVector defCue  = gs->GetCueBallDefaultLocationOnTable();

    FVector prevPos = GetBallLocation(MovedBallInx);
    
    /*FHitResult hit;

    FCollisionQueryParams queryParams;
    queryParams.bTraceComplex = true;
    queryParams.AddIgnoredActor(ballsInfo[MovedBallInx].ball);

    FCollisionResponseParams respParams;
    
    bool hasHit = GetWorld()->SweepSingleByChannel(hit,
                                                   defCue + FVector(0.0f, 0.0f, ballR),
                                                   pos,
                                                   FQuat::Identity,
                                                   ECollisionChannel::ECC_Visibility,
                                                   FCollisionShape::MakeSphere(ballR),
                                                   queryParams,
                                                   respParams);
    if (hasHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot move ball: hit %s"), *hit.Actor->GetHumanReadableName())
        return;
    }*/
    for (int i = 1; i < BallCountInGame; i++)
    {
        float distance = (pos - ballsInfo[i].position).Size();
        if (distance < 2.0f * ballR * 0.99f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot move ball: another ball (%d) is intersecting"), i)
            return;
        }
    }
    
    bool pocket = objects->tablePockets->IntersectingPocket(pos);
    if (pocket)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot move ball: pocket is intersecting"))
        return;
    }
    if (Ruleset && !Ruleset(WorldToTableCoords(pos)))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot move ball: rules are broken"))
        return;
    }
    
    MoveBallAndStop(MovedBallInx, pos, GetBallRotation(MovedBallInx));
}

FVector2D APhysicsEngine::WorldToTableCoords(FVector target)
{
    FVector longRail3D (halfTableCueToDiamond, 0.0f);
    FVector shortRail3D = FVector::CrossProduct(longRail3D, FVector(0.0f, 0.0f, 1.0f));
    shortRail3D.Normalize();
    shortRail3D *= longRail3D.Size();
    FVector defCue  = objects->table->GetCueBallDefaultPos();
    FVector coordStart3D = defCue - longRail3D / 2.0f - shortRail3D / 2.0f;
    
    FVector2D longRail(longRail3D.X, longRail3D.Y);
    FVector2D shortRail(shortRail3D.X, shortRail3D.Y);
    
    
    FVector posOnTable3D = target - coordStart3D;
    FVector2D posOnTable(posOnTable3D.X, posOnTable3D.Y);
    
    float coordShortRail = FVector2D::DotProduct(posOnTable, shortRail) / shortRail.SizeSquared();
    float coordLongRail  = FVector2D::DotProduct(posOnTable, longRail) / longRail.SizeSquared();
    posOnTable.X = coordShortRail;
    posOnTable.Y = coordLongRail;
    return posOnTable;
}

FVector APhysicsEngine::TableCoordsToWorld(FVector2D pos)
{
    FVector longRail3D (halfTableCueToDiamond, 0.0f);
    FVector shortRail3D = FVector::CrossProduct(longRail3D, FVector(0.0f, 0.0f, 1.0f));
    shortRail3D.Normalize();
    shortRail3D *= longRail3D.Size();
    FVector defCue  = objects->table->GetCueBallDefaultPos();
    FVector coordStart3D = defCue - longRail3D / 2.0f - shortRail3D / 2.0f;
    
    FVector posWorld = coordStart3D + pos.X * shortRail3D + pos.Y*longRail3D;
    posWorld.Z = ballRestingOnTableZ;
    return posWorld;
}

FVector APhysicsEngine::GetTableCenter()
{
    return objects->table->GetCueBallDefaultPos() + FVector(halfTableCueToDiamond / 2.0f, 0.0f);
}

float APhysicsEngine::GetTableSize()
{
    return halfTableCueToDiamond.Size() * 2.0f;
}

float APhysicsEngine::GetCushionWidth()
{
    if (!objects || !objects->table)
        return 0.0f;
    FVector d = objects->table->cushionWidth1->GetComponentLocation() -
                objects->table->cushionWidth2->GetComponentLocation();
    return d.Size();
}
float APhysicsEngine::GetCushionHeight()
{
    if (!objects || !objects->table)
        return 0.0f;
    float h =  objects->table->cushionHeight->GetComponentLocation().Z - GetTableCenter().Z;
    return h;
}

bool APhysicsEngine::OnTable(FVector2D coords)
{
    return coords.X > 0.0f && coords.X < 1.0f && coords.Y > 0.0f && coords.Y < 2.0f;
}

bool APhysicsEngine::IsPosFree(FVector2D coords)
{
    FVector pos3D (TableCoordsToWorld(coords));
    for (int i = 1; i < BallCountInGame; i++)
    {
        FVector d = pos3D - ballsInfo[i].position;
        d.Z = 0.0f;
        float distance = d.Size();
        if (distance < 2.0f * ballR * 0.99f)
        {
            return false;
        }
    }
    return true;
}

bool APhysicsEngine::IsPosFreeFromCueBall(FVector2D coords)
{
    FVector d (TableCoordsToWorld(coords) - ballsInfo[0].position);
    float distance = d.Size();
    if (distance < 2.0f * ballR * 0.99f)
    {
        return false;
    }
    return true;
}

void APhysicsEngine::UpdateBallInfo(TArray<ABall*> BallInfoToSet)
{
    for (uint8 i= 0;i<52;++i)
    {
        ballsInfo[i].ball = nullptr;
    }
    for (uint8 i =0 ;i< BallInfoToSet.Num();++i)
    {
        ballsInfo[i].ball = BallInfoToSet[i];
    }
}
