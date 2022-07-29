
#include "SefaPoolSettings.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void USefaPoolSettings::InitDefaults()
{
    
#define SET_TO_DEFAULT(var) var = var##Info.default_;
#define SET_TO_DEFAULT_ENUM(var, type) var = static_cast<type> (var##Info.default_);
    
    SET_TO_DEFAULT(FocusCameraHorizontalSensitivity)
    SET_TO_DEFAULT(FocusCameraVerticalSensitivity)
    SET_TO_DEFAULT(ViewCameraHorizontalSensitivity)
    SET_TO_DEFAULT(FocusPinchSensitivity)
    SET_TO_DEFAULT(FocusJoystickSensitivity)
    SET_TO_DEFAULT(Joystick3DSensitivity)
    SET_TO_DEFAULT(BallInHandSensitivity)
    SET_TO_DEFAULT_ENUM(ShotStrength, EShotStrength)
    SET_TO_DEFAULT_ENUM(HandOrientation, EHandOrientation)
    SET_TO_DEFAULT(CueAutoReload)
    SET_TO_DEFAULT(AutoReloadChalk)
    SET_TO_DEFAULT(ShouldReplayGoodShots)
    SET_TO_DEFAULT(AutoTranslationBallInHand)
    SET_TO_DEFAULT(TapToAim2D)
    SET_TO_DEFAULT(ShouldShowPocketedBalls)
    SET_TO_DEFAULT(TapDelay)
    SET_TO_DEFAULT(DoublePinchTapSpeed)
    SET_TO_DEFAULT(HardMode)
    SET_TO_DEFAULT(PhysSubstepSize)
    SET_TO_DEFAULT(RollingFriction)
    SET_TO_DEFAULT(SlipFrictionBallTable)
    SET_TO_DEFAULT(SlipFrictionBallBall)
    SET_TO_DEFAULT(RestrictionBallCloth)
    SET_TO_DEFAULT(NormalVelocityLossValue)
    SET_TO_DEFAULT(AccelerationOfGravity)
    SET_TO_DEFAULT(VerticalAngleAngularDemping)
    SET_TO_DEFAULT(SlidingResistanceVelocityDeltaTrigger)
    SET_TO_DEFAULT(CoefBallMassDistribution)
    SET_TO_DEFAULT(CoefBallSurfaceVelocityLoss)
    SET_TO_DEFAULT(CoefLossVelocityIntoLossLinearVelocity)
    SET_TO_DEFAULT(CoefSidewallProfile)
    SET_TO_DEFAULT(DrawBallsShadows)
    
    
    SET_TO_DEFAULT_ENUM(CameraMoveMode, ECameraMoveMode)
    SET_TO_DEFAULT_ENUM(PreferredCameraMode, EPreferredCameraMode)
    SET_TO_DEFAULT(SpectatingCamPosition)
    SET_TO_DEFAULT(SpectatingCamHeight)
    SET_TO_DEFAULT(SpectatingCamFOV)
    SET_TO_DEFAULT(SpectatingCamCenteringHeight)
    
    // Графика камера
    /*bool */ //bUseCameraAnimation;
    /*float */ //MusicVolume;
    /*float */ //EffectSoundsVolume;
    /*float */ //GameSoundsVolume;
    /*EMenuOrientation */ //MenuOrientation;

    // Physics
    // //Контесты
    /*float */ //CenterofCircleCOF;
    /*float */ //COFRadius;
    /*int32 */ //CountOfBallsCOF;
    /*FVector2D */ //LineUpDefaultPos;
    /*int32 */ //FoulContractSP;
    
    // visualization
    /*float */ //FirstDataSendDelay;
    /*bool */ //bUseInterpolation;

    // Connectivity
    /*float */ //LobbyPingFrequency;
    /*float */ //WaitingPingDelay;
    /*FString */ //ReleaseBackendAddress;
    /*FString */ //PreReleaseBackendAddress;

    // PoolPlayer
    /*float */ //CameraViewStack;
    /*float */ //MaxAimDisplacementCoef;
    /*float */ //CameraLagSpeed;
    /*FVector */ //DefaultCameraPos;
    /*FVector */ //CueRelativeLocation;
    /*FVector */ //HitPointScale;
    /*float */ //SoftMinimumCueAngle;
    /*float */ //SoftMaxCueAngle;
    /*float */ //HardMaxCueAngle;
    /*float */ //AngleTriggerNonIntensiveCamera;
    /*float */ //CueLength;
    /*float */ //MaxAimVerticalAdjustment;
    /*ESideCamera */ //SideCameraMoving;
    /*float */ //CloseUpCameraZooming;
    /*float */ //MinDistFromTable;
    /*float */ //MaxDistFromTable;

    // MISC
    /*int32 */ //ChalkRetentionShotCount;
    /*int32 */ //BallCountInScene;
    /*float */ //TriggerPerpendicularCameraCatch;
    /*float */ //CameraToAimFrom3DAnimLength;
    /*float */ //CubicFocusAnimLength;
    /*FVector2D */ //PointToCameraMove;
    /*FVector2D */ //VerticalPartOfTheScreenOccupiedByTable;
    /*float */ //GameFlowAimDelay;
    /*float */ //OpponentPositionReplicateFrequency;
    /*float */ //ReadyMessageSendDelay;
    /*float */ //DelayToTravelToNextStage;
    /*float */ //DelayToChangeLevel;
    /*float */ //AngularShiftPocketCameraFromVertical;
    /*float */ //AngularShiftPocketCameraFromHorizontal;
    /*float */ //ShiftCentralPocketCameraFromVertical;
    /*float */ //ShiftCentralPocketCameraFromHorizontal;
    
#undef SET_TO_DEFAULT
#undef SET_TO_DEFAULT_ENUM


}

void USefaPoolSettings::Save()
{
    if(UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "settings", 0))
        PRINTF((TEXT("Settings successfully saved")), Green)
    else
        PRINTF((TEXT("Could not save settings")), Green)
    //Push();
}
    
void USefaPoolSettings::Load()
{
    USefaPoolSettings* LoadedGame = Cast<USefaPoolSettings>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "settings", 0));
    InitDefaults();
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load settings, defaulting")), Yellow)
        Pull();
    }
    else
    {
        FocusCameraHorizontalSensitivity = LoadedGame->FocusCameraHorizontalSensitivity;
        FocusCameraVerticalSensitivity = LoadedGame->FocusCameraVerticalSensitivity;
        ViewCameraHorizontalSensitivity = LoadedGame->ViewCameraHorizontalSensitivity;
        FocusPinchSensitivity = LoadedGame->FocusPinchSensitivity;
        FocusJoystickSensitivity = LoadedGame->FocusJoystickSensitivity;
        Joystick3DSensitivity = LoadedGame->Joystick3DSensitivity;
        BallInHandSensitivity = LoadedGame->BallInHandSensitivity;
        ShotStrength = LoadedGame->ShotStrength;
        HandOrientation  = LoadedGame->HandOrientation;
        CueAutoReload = LoadedGame->CueAutoReload;
        AutoReloadChalk = LoadedGame->AutoReloadChalk;
        ShouldReplayGoodShots = LoadedGame->ShouldReplayGoodShots;
        AutoTranslationBallInHand = LoadedGame->AutoTranslationBallInHand;
        TapToAim2D = LoadedGame->TapToAim2D;
        ShouldShowPocketedBalls = LoadedGame->ShouldShowPocketedBalls;
        TapDelay = LoadedGame->TapDelay;
        DoublePinchTapSpeed = LoadedGame->DoublePinchTapSpeed;
        HardMode = LoadedGame->HardMode;
        PhysSubstepSize = LoadedGame->PhysSubstepSize;
        RollingFriction = LoadedGame->RollingFriction;
        SlipFrictionBallTable = LoadedGame->SlipFrictionBallTable;
        SlipFrictionBallBall = LoadedGame->SlipFrictionBallBall;
        RestrictionBallCloth = LoadedGame->RestrictionBallCloth;
        NormalVelocityLossValue = LoadedGame->NormalVelocityLossValue;
        AccelerationOfGravity = LoadedGame->AccelerationOfGravity;
        VerticalAngleAngularDemping = LoadedGame->VerticalAngleAngularDemping;
        SlidingResistanceVelocityDeltaTrigger = LoadedGame->SlidingResistanceVelocityDeltaTrigger;
        CoefBallMassDistribution = LoadedGame->CoefBallMassDistribution;
        CoefBallSurfaceVelocityLoss = LoadedGame->CoefBallSurfaceVelocityLoss;
        CoefLossVelocityIntoLossLinearVelocity = LoadedGame->CoefLossVelocityIntoLossLinearVelocity;
        CoefSidewallProfile = LoadedGame->CoefSidewallProfile;
        DrawBallsShadows = LoadedGame->DrawBallsShadows;
        CameraMoveMode = LoadedGame->CameraMoveMode;
        UseCameraAnimation = LoadedGame->UseCameraAnimation;
        PreferredCameraMode = LoadedGame->PreferredCameraMode;
        MusicVolume = LoadedGame->MusicVolume;
        EffectSoundsVolume = LoadedGame->EffectSoundsVolume;
        GameSoundsVolume = LoadedGame->GameSoundsVolume;
        MenuOrientation = LoadedGame->MenuOrientation;
        Pull();
        PRINTF((TEXT("Settings successfully loaded")), Green)
    }
}
    
void USefaPoolSettings::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                          const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    auto callback = [this, successful, unsuccessful](cool_json_t json) -> void
    {
        UE_LOG(LogTemp, Warning, TEXT("Request user/get_user_settings was succesful"));
        if (!json.contains("status") || json["status"] != 200)
        {
            UE_LOG(LogTemp, Warning, TEXT("Bad status on user/get_user_settings."));
            if (unsuccessful) unsuccessful(this);
        }
        else
        {
            InitDefaults();
            if (!json.contains("settings")) return;
#define GetAny_SettingIfAvailableFromJson(sett, type)\
            if (json["settings"].contains(#sett)) \
                sett = json["settings"][#sett].get<type>();
#define GetEnumSettingIfAvailableFromJson(sett, type)\
            if (json["settings"].contains(#sett)) \
                sett = static_cast<type>(json["settings"][#sett].get<int>());
            
            
            UE_LOG(LogTemp, Warning, TEXT("    Pulled settings: %s"), *(FString(json.dump().c_str())))
            
            GetAny_SettingIfAvailableFromJson(FocusCameraHorizontalSensitivity, float);
            GetAny_SettingIfAvailableFromJson(FocusCameraVerticalSensitivity, float);
            GetAny_SettingIfAvailableFromJson(ViewCameraHorizontalSensitivity, float);
            GetAny_SettingIfAvailableFromJson(FocusPinchSensitivity, float);
            GetAny_SettingIfAvailableFromJson(FocusJoystickSensitivity, float);
            GetAny_SettingIfAvailableFromJson(Joystick3DSensitivity, float);
            GetAny_SettingIfAvailableFromJson(BallInHandSensitivity, float);
            GetEnumSettingIfAvailableFromJson(ShotStrength, EShotStrength);
            GetEnumSettingIfAvailableFromJson(HandOrientation, EHandOrientation);
            GetAny_SettingIfAvailableFromJson(CueAutoReload, bool);
            GetAny_SettingIfAvailableFromJson(AutoReloadChalk, bool);
            GetAny_SettingIfAvailableFromJson(ShouldReplayGoodShots, bool);
            GetAny_SettingIfAvailableFromJson(AutoTranslationBallInHand, bool);
            GetAny_SettingIfAvailableFromJson(TapToAim2D, bool);
            GetAny_SettingIfAvailableFromJson(ShouldShowPocketedBalls, bool);
            GetAny_SettingIfAvailableFromJson(TapDelay, float);
            GetAny_SettingIfAvailableFromJson(DoublePinchTapSpeed, float);
            GetAny_SettingIfAvailableFromJson(HardMode, bool);
            GetAny_SettingIfAvailableFromJson(PhysSubstepSize, float);
            GetAny_SettingIfAvailableFromJson(RollingFriction, float);
            GetAny_SettingIfAvailableFromJson(SlipFrictionBallTable, float);
            GetAny_SettingIfAvailableFromJson(SlipFrictionBallBall, float);
            GetAny_SettingIfAvailableFromJson(RestrictionBallCloth, float);
            GetAny_SettingIfAvailableFromJson(NormalVelocityLossValue, float);
            GetAny_SettingIfAvailableFromJson(AccelerationOfGravity, float);
            GetAny_SettingIfAvailableFromJson(VerticalAngleAngularDemping, float);
            GetAny_SettingIfAvailableFromJson(SlidingResistanceVelocityDeltaTrigger, float);
            GetAny_SettingIfAvailableFromJson(CoefBallMassDistribution, float);
            GetAny_SettingIfAvailableFromJson(CoefBallSurfaceVelocityLoss, float);
            GetAny_SettingIfAvailableFromJson(CoefLossVelocityIntoLossLinearVelocity, float);
            GetAny_SettingIfAvailableFromJson(CoefSidewallProfile, float);
            GetAny_SettingIfAvailableFromJson(DrawBallsShadows, bool);
            GetEnumSettingIfAvailableFromJson(CameraMoveMode, ECameraMoveMode);
            GetAny_SettingIfAvailableFromJson(UseCameraAnimation, bool);
            GetEnumSettingIfAvailableFromJson(PreferredCameraMode, EPreferredCameraMode);
            GetAny_SettingIfAvailableFromJson(MusicVolume, float);
            GetAny_SettingIfAvailableFromJson(EffectSoundsVolume, float);
            GetAny_SettingIfAvailableFromJson(GameSoundsVolume, float);
            GetEnumSettingIfAvailableFromJson(MenuOrientation, EMenuOrientation);
#undef GetSettingIfAvailableFromJson
#undef GetEnumSettingIfAvailableFromJson
            if (successful) successful(this);
        }
    };
    
    gi->MakeRequest("user", "get_user_settings", cool_json_t(), callback);
}

void USefaPoolSettings::Push()
{
    if (!gi) return;
    cool_json_t settings_json;
    //UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    settings_json["FocusCameraHorizontalSensitivity"] = FocusCameraHorizontalSensitivity;
    settings_json["FocusCameraVerticalSensitivity"] = FocusCameraVerticalSensitivity;
    settings_json["ViewCameraHorizontalSensitivity"] = ViewCameraHorizontalSensitivity;
    settings_json["FocusPinchSensitivity"] = FocusPinchSensitivity;
    settings_json["FocusJoystickSensitivity"] = FocusJoystickSensitivity;
    settings_json["Joystick3DSensitivity"] = Joystick3DSensitivity;
    settings_json["BallInHandSensitivity"] = BallInHandSensitivity;
    settings_json["ShotStrength"] = ShotStrength;
    settings_json["HandOrientation"] = HandOrientation;
    settings_json["CueAutoReload"] = CueAutoReload;
    settings_json["AutoReloadChalk"] = AutoReloadChalk;
    settings_json["ShouldReplayGoodShots"] = ShouldReplayGoodShots;
    settings_json["AutoTranslationBallInHand"] = AutoTranslationBallInHand;
    settings_json["2DTapToAim"] = TapToAim2D;
    settings_json["TapDelay"] = TapDelay;
    settings_json["DoublePinchTapSpeed"] = DoublePinchTapSpeed;
    settings_json["HardMode"] = HardMode;
    settings_json["PhysSubstepSize"] = PhysSubstepSize;
    settings_json["RollingFriction"] = RollingFriction;
    settings_json["SlipFrictionBallTable"] = SlipFrictionBallTable;
    settings_json["SlipFrictionBallBall"] = SlipFrictionBallBall;
    settings_json["RestrictionBallCloth"] = RestrictionBallCloth;
    settings_json["NormalVelocityLossValue"] = NormalVelocityLossValue;
    settings_json["AccelerationOfGravity"] = AccelerationOfGravity;
    settings_json["VerticalAngleAngularDemping"] = VerticalAngleAngularDemping;
    settings_json["SlidingResistanceVelocityDeltaTrigger"] = SlidingResistanceVelocityDeltaTrigger;
    settings_json["CoefBallMassDistribution"] = CoefBallMassDistribution;
    settings_json["CoefBallSurfaceVelocityLoss"] = CoefBallSurfaceVelocityLoss;
    settings_json["CoefLossVelocityIntoLossLinearVelocity"] = CoefLossVelocityIntoLossLinearVelocity;
    settings_json["CoefSidewallProfile"] = CoefSidewallProfile;
    settings_json["DrawBallsShadows"] = DrawBallsShadows;
    settings_json["CameraMoveMode"] = CameraMoveMode;
    settings_json["UseCameraAnimation"] = UseCameraAnimation;
    settings_json["PreferredCameraMode"] = PreferredCameraMode;
    settings_json["MusicVolume"] = MusicVolume;
    settings_json["GameSoundsVolume"] = GameSoundsVolume;
    settings_json["MenuOrientation"] = MenuOrientation;
    
    cool_json_t req_json;
    req_json["settings"] = settings_json;
    auto callback = [this](cool_json_t json) -> void {
        if (!json.contains("status") || json["status"] != 200)
        {
            UE_LOG(LogTemp, Error, TEXT("Bad status on user/set_user_settings."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("All good user/set_user_settings."));
        }
    };
    gi->MakeRequest("user", "set_user_settings", req_json, callback);
}


void USefaPoolSettings::SetFocusCameraHorizontalSensitivity(float NewValue)
{
    FocusCameraHorizontalSensitivity = NewValue;
    if (OnFocusCameraHorizontalSensitivityChanged.IsBound())
    {
        OnFocusCameraHorizontalSensitivityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetFocusCameraVerticalSensitivity(float NewValue)
{
    FocusCameraVerticalSensitivity = NewValue;
    if (OnFocusCameraVerticalSensitivityChanged.IsBound())
    {
        OnFocusCameraVerticalSensitivityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetViewCameraHorizontalSensitivity(float NewValue)
{
    ViewCameraHorizontalSensitivity = NewValue;
    if (OnViewCameraHorizontalSensitivityChanged.IsBound())
    {
        OnViewCameraHorizontalSensitivityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetFocusPinchSensitivity(float NewValue)
{
    FocusPinchSensitivity = NewValue;
    if (OnFocusPinchSensitivityChanged.IsBound())
    {
        OnFocusPinchSensitivityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetFocusJoystickSensitivity(float NewValue)
{
    FocusJoystickSensitivity = NewValue;
    if (OnFocusJoystickSensitivityChanged.IsBound())
    {
        OnFocusJoystickSensitivityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetJoystick3DSensitivity(float NewValue)
{
    Joystick3DSensitivity = NewValue;
    if (OnJoystick3DSensitivityChanged.IsBound())
    {
        OnJoystick3DSensitivityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetBallInHandSensitivity(float NewValue)
{
    BallInHandSensitivity = NewValue;
    if (OnBallInHandSensitivityChanged.IsBound())
    {
        OnBallInHandSensitivityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetShotStrength(EShotStrength NewValue)
{
    UE_LOG(LogTemp, Warning, TEXT("Changing value in %s"), *FString(__PRETTY_FUNCTION__))
    ShotStrength = NewValue;
    if (OnShotStrengthChanged.IsBound())
    {
        OnShotStrengthChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetShotStrength_int(int32 NewValue)
{
    SetShotStrength(static_cast<EShotStrength> (NewValue));
}

void USefaPoolSettings::SetHandOrientation(EHandOrientation NewValue)
{
    UE_LOG(LogTemp, Warning, TEXT("Changing value in %s"), *FString(__PRETTY_FUNCTION__))
    HandOrientation = NewValue;
    if (OnHandOrientationChanged.IsBound())
    {
        OnHandOrientationChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetHandOrientation_int(int32 NewValue)
{
    SetHandOrientation(static_cast<EHandOrientation> (NewValue));
}

void USefaPoolSettings::SetCueAutoReload(bool NewValue)
{
    CueAutoReload = NewValue;
    if (OnCueAutoReloadChanged.IsBound())
    {
        OnCueAutoReloadChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetAutoReloadChalk(bool NewValue)
{
    AutoReloadChalk = NewValue;
    if (OnAutoReloadChalkChanged.IsBound())
    {
        OnAutoReloadChalkChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetShouldReplayGoodShots(bool NewValue)
{
    ShouldReplayGoodShots = NewValue;
    if (OnShouldReplayGoodShotsChanged.IsBound())
    {
        OnShouldReplayGoodShotsChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetAutoTranslationBallInHand(bool NewValue)
{
    AutoTranslationBallInHand = NewValue;
    if (OnAutoTranslationBallInHandChanged.IsBound())
    {
        OnAutoTranslationBallInHandChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetTapToAim2D(bool NewValue)
{
    TapToAim2D = NewValue;
    if (On2DTapToAimChanged.IsBound())
    {
        On2DTapToAimChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetShouldShowPocketedBalls(bool NewValue)
{
    ShouldShowPocketedBalls = NewValue;
    if (OnShouldShowPocketedBallsChanged.IsBound())
    {
        OnShouldShowPocketedBallsChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetTapDelay(float NewValue)
{
    TapDelay = NewValue;
    if (OnTapDelayChanged.IsBound())
    {
        OnTapDelayChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetDoublePinchTapSpeed(float NewValue)
{
    DoublePinchTapSpeed = NewValue;
    if (OnDoublePinchTapSpeedChanged.IsBound())
    {
        OnDoublePinchTapSpeedChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

#define SET_TO_DEFAULT(var) Set##var(var##Info.default_);
#define SET_TO_DEFAULT_ENUM(var, type) Set##var(static_cast<type> (var##Info.default_));


void USefaPoolSettings::Reset()
{
    SET_TO_DEFAULT(FocusCameraHorizontalSensitivity)
    SET_TO_DEFAULT(FocusCameraVerticalSensitivity)
    SET_TO_DEFAULT(ViewCameraHorizontalSensitivity)
    SET_TO_DEFAULT(FocusPinchSensitivity)
    SET_TO_DEFAULT(FocusJoystickSensitivity)
    SET_TO_DEFAULT(Joystick3DSensitivity)
    SET_TO_DEFAULT(BallInHandSensitivity)
    SET_TO_DEFAULT_ENUM(ShotStrength, EShotStrength)
    SET_TO_DEFAULT_ENUM(HandOrientation, EHandOrientation)
    SET_TO_DEFAULT(CueAutoReload)
    SET_TO_DEFAULT(AutoReloadChalk)
    SET_TO_DEFAULT(ShouldReplayGoodShots)
    SET_TO_DEFAULT(AutoTranslationBallInHand)
    SET_TO_DEFAULT(TapToAim2D)
    SET_TO_DEFAULT(ShouldShowPocketedBalls)
    SET_TO_DEFAULT(TapDelay)
    SET_TO_DEFAULT(DoublePinchTapSpeed)
    SET_TO_DEFAULT(HardMode)
    SET_TO_DEFAULT(PhysSubstepSize)
    SET_TO_DEFAULT(RollingFriction)
    SET_TO_DEFAULT(SlipFrictionBallTable)
    SET_TO_DEFAULT(SlipFrictionBallBall)
    SET_TO_DEFAULT(RestrictionBallCloth)
    SET_TO_DEFAULT(NormalVelocityLossValue)
    SET_TO_DEFAULT(AccelerationOfGravity)
    SET_TO_DEFAULT(VerticalAngleAngularDemping)
    SET_TO_DEFAULT(SlidingResistanceVelocityDeltaTrigger)
    SET_TO_DEFAULT(CoefBallMassDistribution)
    SET_TO_DEFAULT(CoefBallSurfaceVelocityLoss)
    SET_TO_DEFAULT(CoefLossVelocityIntoLossLinearVelocity)
    SET_TO_DEFAULT(CoefSidewallProfile)
    SET_TO_DEFAULT(DrawBallsShadows)
    SET_TO_DEFAULT_ENUM(CameraMoveMode, ECameraMoveMode)
    SET_TO_DEFAULT_ENUM(PreferredCameraMode, EPreferredCameraMode)
    
    Save();
}

void USefaPoolSettings::ResetPhysics()
{
    SET_TO_DEFAULT(PhysSubstepSize)
    SET_TO_DEFAULT(RollingFriction)
    SET_TO_DEFAULT(SlipFrictionBallTable)
    SET_TO_DEFAULT(SlipFrictionBallBall)
    SET_TO_DEFAULT(RestrictionBallCloth)
    SET_TO_DEFAULT(NormalVelocityLossValue)
    SET_TO_DEFAULT(AccelerationOfGravity)
    SET_TO_DEFAULT(VerticalAngleAngularDemping)
    SET_TO_DEFAULT(SlidingResistanceVelocityDeltaTrigger)
    SET_TO_DEFAULT(CoefBallMassDistribution)
    SET_TO_DEFAULT(CoefBallSurfaceVelocityLoss)
    SET_TO_DEFAULT(CoefLossVelocityIntoLossLinearVelocity)
    SET_TO_DEFAULT(CoefSidewallProfile)
    
    Save();
}

#undef SET_TO_DEFAULT
#undef SET_TO_DEFAULT_ENUM

void USefaPoolSettings::SetHardMode(bool NewValue)
{
    HardMode = NewValue;
    if (OnHardModeChanged.IsBound())
    {
        OnHardModeChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetPhysSubstepSize(float NewValue)
{
    PhysSubstepSize = NewValue;
    if (OnPhysSubstepSizeChanged.IsBound())
    {
        OnPhysSubstepSizeChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetRollingFriction(float NewValue)
{
    RollingFriction = NewValue;
    if (OnRollingFrictionChanged.IsBound())
    {
        OnRollingFrictionChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSlipFrictionBallTable(float NewValue)
{
    SlipFrictionBallTable = NewValue;
    if (OnSlipFrictionBallTableChanged.IsBound())
    {
        OnSlipFrictionBallTableChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSlipFrictionBallBall(float NewValue)
{
    SlipFrictionBallBall = NewValue;
    if (OnSlipFrictionBallBallChanged.IsBound())
    {
        OnSlipFrictionBallBallChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetRestrictionBallCloth(float NewValue)
{
    RestrictionBallCloth = NewValue;
    if (OnRestrictionBallClothChanged.IsBound())
    {
        OnRestrictionBallClothChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetNormalVelocityLossValue(float NewValue)
{
    NormalVelocityLossValue = NewValue;
    if (OnNormalVelocityLossValueChanged.IsBound())
    {
        OnNormalVelocityLossValueChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetAccelerationOfGravity(float NewValue)
{
    AccelerationOfGravity = NewValue;
    if (OnAccelerationOfGravityChanged.IsBound())
    {
        OnAccelerationOfGravityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetVerticalAngleAngularDemping(float NewValue)
{
    VerticalAngleAngularDemping = NewValue;
    if (OnVerticalAngleAngularDempingChanged.IsBound())
    {
        OnVerticalAngleAngularDempingChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSlidingResistanceVelocityDeltaTrigger(float NewValue)
{
    SlidingResistanceVelocityDeltaTrigger = NewValue;
    if (OnSlidingResistanceVelocityDeltaTriggerChanged.IsBound())
    {
        OnSlidingResistanceVelocityDeltaTriggerChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCoefBallMassDistribution(float NewValue)
{
    CoefBallMassDistribution = NewValue;
    if (OnCoefBallMassDistributionChanged.IsBound())
    {
        OnCoefBallMassDistributionChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCoefBallSurfaceVelocityLoss(float NewValue)
{
    CoefBallSurfaceVelocityLoss = NewValue;
    if (OnCoefBallSurfaceVelocityLossChanged.IsBound())
    {
        OnCoefBallSurfaceVelocityLossChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCoefLossVelocityIntoLossLinearVelocity(float NewValue)
{
    CoefLossVelocityIntoLossLinearVelocity = NewValue;
    if (OnCoefLossVelocityIntoLossLinearVelocityChanged.IsBound())
    {
        OnCoefLossVelocityIntoLossLinearVelocityChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCoefSidewallProfile(float NewValue)
{
    CoefSidewallProfile = NewValue;
    if (OnCoefSidewallProfileChanged.IsBound())
    {
        OnCoefSidewallProfileChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetDrawBallsShadows(bool NewValue)
{
    DrawBallsShadows = NewValue;
    if (OnDrawBallsShadowsChanged.IsBound())
    {
        OnDrawBallsShadowsChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCameraMoveMode(ECameraMoveMode NewValue)
{
    UE_LOG(LogTemp, Warning, TEXT("Changing value in %s"), *FString(__PRETTY_FUNCTION__))
    CameraMoveMode = NewValue;
    if (OnCameraMoveModeChanged.IsBound())
    {
        OnCameraMoveModeChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCameraMoveMode_int(int32 NewValue)
{
    SetCameraMoveMode(static_cast<ECameraMoveMode> (NewValue));
}

void USefaPoolSettings::SetUseCameraAnimation(bool NewValue)
{
    UseCameraAnimation = NewValue;
    if (OnUseCameraAnimationChanged.IsBound())
    {
        OnUseCameraAnimationChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetPreferredCameraMode(EPreferredCameraMode NewValue)
{
    UE_LOG(LogTemp, Warning, TEXT("Changing value in %s"), *FString(__PRETTY_FUNCTION__))
    PreferredCameraMode = NewValue;
    if (OnPreferredCameraModeChanged.IsBound())
    {
        OnPreferredCameraModeChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetPreferredCameraMode_int(int32 NewValue)
{
    SetPreferredCameraMode(static_cast<EPreferredCameraMode> (NewValue));
}

void USefaPoolSettings::SetSpectatingCamPosition(float NewValue)
{
    SpectatingCamPosition = NewValue;
    if (SpectatingCamPositionChanged.IsBound())
    {
        SpectatingCamPositionChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSpectatingCamHeight(float NewValue)
{
    SpectatingCamHeight = NewValue;
    if (SpectatingCamHeightChanged.IsBound())
    {
        SpectatingCamHeightChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSpectatingCamFOV(float NewValue)
{
    SpectatingCamFOV = NewValue;
    if (SpectatingCamFOVChanged.IsBound())
    {
        SpectatingCamFOVChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSpectatingCamCenteringHeight(float NewValue)
{
    SpectatingCamCenteringHeight = NewValue;
    if (SpectatingCamCenteringHeightChanged.IsBound())
    {
        SpectatingCamCenteringHeightChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetMusicVolume(float NewValue)
{
    MusicVolume = NewValue;
    if (OnMusicVolumeChanged.IsBound())
    {
        OnMusicVolumeChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetEffectSoundsVolume(float NewValue)
{
    EffectSoundsVolume = NewValue;
    if (OnEffectSoundsVolumeChanged.IsBound())
    {
        OnEffectSoundsVolumeChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetGameSoundsVolume(float NewValue)
{
    GameSoundsVolume = NewValue;
    if (OnGameSoundsVolumeChanged.IsBound())
    {
        OnGameSoundsVolumeChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetMenuOrientation(EMenuOrientation NewValue)
{
    UE_LOG(LogTemp, Warning, TEXT("Changing value in %s"), *FString(__PRETTY_FUNCTION__))
    MenuOrientation = NewValue;
    if (OnMenuOrientationChanged.IsBound())
    {
        OnMenuOrientationChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetMenuOrientation_int(int32 NewValue)
{
    SetMenuOrientation(static_cast<EMenuOrientation> (NewValue));
}

void USefaPoolSettings::SetCenterofCircleCOF(float NewValue)
{
    CenterofCircleCOF = NewValue;
    if (OnCenterofCircleCofChanged.IsBound())
    {
        OnCenterofCircleCofChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCOFRadius(float NewValue)
{
    COFRadius = NewValue;
    if (OnCofRadiusChanged.IsBound())
    {
        OnCofRadiusChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCountOfBallsCOF(int32 NewValue)
{
    CountOfBallsCOF = NewValue;
    if (OnCountOfBallsCofChanged.IsBound())
    {
        OnCountOfBallsCofChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetLineUpDefaultPos(FVector2D NewValue)
{
    LineUpDefaultPos = NewValue;
    if (OnLineUpDefaultPosChanged.IsBound())
    {
        OnLineUpDefaultPosChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetFoulContractSP(int32 NewValue)
{
    FoulContractSP = NewValue;
    if (OnFoulContractSPChanged.IsBound())
    {
        OnFoulContractSPChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetFirstDataSendDelay(float NewValue)
{
    FirstDataSendDelay = NewValue;
    if (OnFirstDataSendDelayChanged.IsBound())
    {
        OnFirstDataSendDelayChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetUseInterpolation(bool NewValue)
{
    bUseInterpolation = NewValue;
    if (OnUseInterpolationChanged.IsBound())
    {
        OnUseInterpolationChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetLobbyPingFrequency(float NewValue)
{
    LobbyPingFrequency = NewValue;
    if (OnLobbyPingFrequencyChanged.IsBound())
    {
        OnLobbyPingFrequencyChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetWaitingPingDelay(float NewValue)
{
    WaitingPingDelay = NewValue;
    if (OnWaitingPingDelayChanged.IsBound())
    {
        OnWaitingPingDelayChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetReleaseBackendAddress(FString NewValue)
{
    ReleaseBackendAddress = NewValue;
    if (OnReleaseBackendAddressChanged.IsBound())
    {
        OnReleaseBackendAddressChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetPreReleaseBackendAdress(FString NewValue)
{
    PreReleaseBackendAddress = NewValue;
    if (OnPreReleaseBackendAddressChanged.IsBound())
    {
        OnPreReleaseBackendAddressChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCameraViewStack(float NewValue)
{
    CameraViewStack = NewValue;
    if (OnCameraViewStackChanged.IsBound())
    {
        OnCameraViewStackChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetMaxAimDisplacementCoef(float NewValue)
{
    MaxAimDisplacementCoef = NewValue;
    if (OnMaxAimDisplacementCoefChanged.IsBound())
    {
        OnMaxAimDisplacementCoefChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCameraLagSpeed(float NewValue)
{
    CameraLagSpeed = NewValue;
    if (OnCameraLagSpeedChanged.IsBound())
    {
        OnCameraLagSpeedChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetDefaultCameraPos(FVector NewValue)
{
    DefaultCameraPos = NewValue;
    if (OnDefaultCameraPosChanged.IsBound())
    {
        OnDefaultCameraPosChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCueRelativeLocation(FVector NewValue)
{
    CueRelativeLocation = NewValue;
    if (OnCueRelativeLocationChanged.IsBound())
    {
        OnCueRelativeLocationChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetHitPointScale(FVector NewValue)
{
    HitPointScale = NewValue;
    if (OnHitPointScaleChanged.IsBound())
    {
        OnHitPointScaleChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSoftMinimumCueAngle(float NewValue)
{
    SoftMinimumCueAngle = NewValue;
    if (OnSoftMinimumCueAngleChanged.IsBound())
    {
        OnSoftMinimumCueAngleChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSoftMaxCueAngle(float NewValue)
{
    SoftMaxCueAngle = NewValue;
    if (OnSoftMaxCueAngleChanged.IsBound())
    {
        OnSoftMaxCueAngleChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetHardMaxCueAngle(float NewValue)
{
    HardMaxCueAngle = NewValue;
    if (OnHardMaxCueAngleChanged.IsBound())
    {
        OnHardMaxCueAngleChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetAngleTriggerNonIntensiveCamera(float NewValue)
{
    AngleTriggerNonIntensiveCamera = NewValue;
    if (OnAngleTriggerNonIntensiveCameraChanged.IsBound())
    {
        OnAngleTriggerNonIntensiveCameraChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCueLength(float NewValue)
{
    CueLength = NewValue;
    if (OnCueLengthChanged.IsBound())
    {
        OnCueLengthChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetMaxAimVerticalAdjustment(float NewValue)
{
    MaxAimVerticalAdjustment = NewValue;
    if (OnMaxAimVerticalAdjustmentChanged.IsBound())
    {
        OnMaxAimVerticalAdjustmentChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSideCameraMoving(ESideCamera NewValue)
{
    UE_LOG(LogTemp, Warning, TEXT("Changing value in %s"), *FString(__PRETTY_FUNCTION__))
    SideCameraMoving = NewValue;
    if (OnSideCameraMovingChanged.IsBound())
    {
        OnSideCameraMovingChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetSideCameraMoving_int(int32 NewValue)
{
    SetSideCameraMoving(static_cast<ESideCamera> (NewValue));
}

void USefaPoolSettings::SetCloseUpCameraZooming(float NewValue)
{
    CloseUpCameraZooming = NewValue;
    if (OnCloseUpCameraZoomingChanged.IsBound())
    {
        OnCloseUpCameraZoomingChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetMinDistFromTable(float NewValue)
{
    MinDistFromTable = NewValue;
    if (OnMinDistFromTableChanged.IsBound())
    {
        OnMinDistFromTableChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetMaxDistFromTable(float NewValue)
{
    MaxDistFromTable = NewValue;
    if (OnMaxDistFromTableChanged.IsBound())
    {
        OnMaxDistFromTableChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetChalkRetentionShotCount(int32 NewValue)
{
    ChalkRetentionShotCount = NewValue;
    if (OnChalkRetentionShotCountChanged.IsBound())
    {
        OnChalkRetentionShotCountChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetBallCountInScene(int32 NewValue)
{
    BallCountInScene = NewValue;
    if ( OnBallCountInSceneChanged.IsBound())
    {
        OnBallCountInSceneChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetTriggerPerpendicularCameraCatch(float NewValue)
{
    TriggerPerpendicularCameraCatch = NewValue;
    if (OnTriggerPerpendicularCameraCatchChanged.IsBound())
    {
        OnTriggerPerpendicularCameraCatchChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCameraToAimFrom3DAnimLength(float NewValue)
{
    CameraToAimFrom3DAnimLength = NewValue;
    if (OnCameraToAimFrom3DAnimLengthChanged.IsBound())
    {
        OnCameraToAimFrom3DAnimLengthChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetCubicFocusAnimLength(float NewValue)
{
    CubicFocusAnimLength = NewValue;
    if (OnCubicFocusAnimLengthChanged.IsBound())
    {
        OnCubicFocusAnimLengthChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetPointToCameraMove(FVector2D NewValue)
{
    PointToCameraMove = NewValue;
    if (OnPointToCameraMoveChanged.IsBound())
    {
        OnPointToCameraMoveChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetVerticalPartOfTheScreenOccupiedByTable(FVector2D NewValue)
{
    VerticalPartOfTheScreenOccupiedByTable = NewValue;
    if (OnVerticalPartOfTheScreenOccupiedByTableChanged.IsBound())
    {
        OnVerticalPartOfTheScreenOccupiedByTableChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetGameFlowAimDelay(float NewValue)
{
    GameFlowAimDelay = NewValue;
    if (OnGameFlowAimDelayChanged.IsBound())
    {
        OnGameFlowAimDelayChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetOpponentPositionReplicateFrequency(float NewValue)
{
    OpponentPositionReplicateFrequency = NewValue;
    if (OnOpponentPositionReplicateFrequencyChanged.IsBound())
    {
        OnOpponentPositionReplicateFrequencyChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetReadyMessageSendDelay(float NewValue)
{
    ReadyMessageSendDelay = NewValue;
    if (OnReadyMessageSendDelayChanged.IsBound())
    {
        OnReadyMessageSendDelayChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetDelayToTravelToNextStage(float NewValue)
{
    DelayToTravelToNextStage = NewValue;
    if (OnDelayToTravelToNextStageChanged.IsBound())
    {
        OnDelayToTravelToNextStageChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetDelayToChangeLevel(float NewValue)
{
    DelayToChangeLevel = NewValue;
    if (OnDelayToChangeLevelChanged.IsBound())
    {
        OnDelayToChangeLevelChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetAngularShiftPocketCameraFromVertical(float NewValue)
{
    AngularShiftPocketCameraFromVertical = NewValue;
    if (OnAngularShiftPocketCameraFromVerticalChanged.IsBound())
    {
        OnAngularShiftPocketCameraFromVerticalChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetAngularShiftPocketCameraFromHorizontal(float NewValue)
{
    AngularShiftPocketCameraFromHorizontal = NewValue;
    if (OnAngularShiftPocketCameraFromHorizontalChanged.IsBound())
    {
        OnAngularShiftPocketCameraFromHorizontalChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetShiftCentralPocketCameraFromVertical(float NewValue)
{
    ShiftCentralPocketCameraFromVertical = NewValue;
    if (OnShiftCentralPocketCameraFromVerticalChanged.IsBound())
    {
        OnShiftCentralPocketCameraFromVerticalChanged.Broadcast(NewValue);
    }
    SaveConfig();
}

void USefaPoolSettings::SetShiftCentralPocketCameraFromHorizontal(float NewValue)
{
    ShiftCentralPocketCameraFromHorizontal = NewValue;
    if (OnShiftCentralPocketCameraFromHorizontalChanged.IsBound())
    {
        OnShiftCentralPocketCameraFromHorizontalChanged.Broadcast(NewValue);
    }
    SaveConfig();
}


