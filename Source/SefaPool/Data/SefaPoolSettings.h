
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolSettings.generated.h"


UENUM()
enum class EShotStrength : uint8
{
    Exponential,
    Linear,
    Logarithmic
};

UENUM()
enum class EHandOrientation : uint8
{
    LeftHanded,
    RightHanded
};


UENUM()
enum class ECameraMoveMode : uint8
{
    FullMode,
    OnlyPockets,
    None
};

UENUM()
enum class EPreferredCameraMode : uint8
{
    Cam2D,
    Cam3D
};

UENUM()
enum class EMenuOrientation : uint8
{
    Portrait,
    Landscape,
    Left,
    Right
};

UENUM()
enum class ESideCamera : uint8
{
    Along,
    Perpendicular
};

// GAME DELEGATES


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusCameraHorizontalSensitivityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusCameraVerticalSensitivityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewCameraHorizontalSensitivityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusPinchSensitivityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusJoystickSensitivityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoystick3DSensitivityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallInHandSensitivityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShotStrengthChanged,EShotStrength,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandOrientationChanged,EHandOrientation,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCueAutoReloadChanged,bool,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutoReloadChalkChanged,bool,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShouldReplayGoodShotsChanged,bool,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutoTranslationBallInHandChanged,bool,Newvalue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOn2DTapToAimChanged, bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShouldShowPocketedBallsChanged,bool,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTapDelayChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoublePinchTapSpeedChanged,float,NewValue);

// Training only
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHardModeChanged,bool,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhysSubstepSizeChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRollingFrictionChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlipFrictionBallTableChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlipFrictionBallBallChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRestrictionBallClothChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNormalVelocityLossValueChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAccelerationOfGravityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVerticalAngleAngularDempingChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlidingResistanceVelocityDeltaTriggerChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoefBallMassDistributionChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoefBallSurfaceVelocityLossChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoefLossVelocityIntoLossLinearVelocityChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoefSidewallProfileChanged,float,NewValue);

// graphic, sound, general
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrawBallsShadowsChanged,bool,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraMoveModeChanged,ECameraMoveMode,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseCameraAnimationChanged,bool,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreferredCameraModeChanged,EPreferredCameraMode,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpectatingCamPositionChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpectatingCamHeightChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpectatingCamFOVChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpectatingCamCenteringHeightChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicVolumeChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectSoundsVolumeChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSoundsVolumeChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuOrientationChanged,EMenuOrientation,NewValue);
///////////////////////////////////////////////////////////////
// Contest
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCenterofCircleCOFChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCOFRadiusChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountOfBallsCOFChanged,int32,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLineUpDefaultPosChanged,FVector2D,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFoulContractSPChanged,int32,NewValue);

// Visualization
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFirstDataSendDelayChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseInterpolationChanged,bool,NewValue);

// Connectivity
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyPingFrequencyChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaitingPingDelayChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReleaseBackendAddressChanged,FString,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreReleaseBackendAddressChanged,FString,NewValue);

// Pool Player
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraViewStackChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxAimDisplacementCoefChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraLagSpeedChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDefaultCameraPosChanged,FVector,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCueRelativeLocationChanged,FVector,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitPointScaleChanged,FVector,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoftMinimumCueAngleChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoftMaxCueAngleChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHardMaxCueAngleChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAngleTriggerNonIntensiveCameraChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCueLengthChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxAimVerticalAdjustmentChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSideCameraMovingChanged,ESideCamera,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloseUpCameraZoomingChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinDistFromTableChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxDistFromTableChanged,float,NewValue);


    

// MISC
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChalkRetentionShotCountChanged,int32,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallCountInSceneChanged,int32,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerPerpendicularCameraCatchChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraToAimFrom3DAnimLengthChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCubicFocusAnimLengthChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointToCameraMoveChanged,FVector2D,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVerticalPartOfTheScreenOccupiedByTableChanged,FVector2D,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameFlowAimDelayChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpponentPositionReplicateFrequencyChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyMessageSendDelayChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDelayToTravelToNextStageChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDelayToChangeLevelChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAngularShiftPocketCameraFromVerticalChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAngularShiftPocketCameraFromHorizontalChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShiftCentralPocketCameraFromVerticalChanged,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShiftCentralPocketCameraFromHorizontalChanged,float,NewValue);

USTRUCT(BlueprintType)
struct FFloatSettingInfo
{
    UPROPERTY(BlueprintReadOnly)
    FString category;
    UPROPERTY(BlueprintReadOnly)
    float default_;
    GENERATED_USTRUCT_BODY()
    UPROPERTY(BlueprintReadOnly)
    FString name;
    UPROPERTY(BlueprintReadOnly)
    FString description;
    UPROPERTY(BlueprintReadOnly)
    float min;
    UPROPERTY(BlueprintReadOnly)
    float max;
    UPROPERTY(BlueprintReadOnly)
    float step;
};

USTRUCT(BlueprintType)
struct FEnumSettingInfo
{
    UPROPERTY(BlueprintReadOnly)
    FString category;
    UPROPERTY(BlueprintReadOnly)
    int default_;
    GENERATED_USTRUCT_BODY()
    UPROPERTY(BlueprintReadOnly)
    FString name;
    UPROPERTY(BlueprintReadOnly)
    FString description;
    UPROPERTY(BlueprintReadOnly)
    TArray<FString> values;
};

USTRUCT(BlueprintType)
struct FBoolSettingInfo
{
    UPROPERTY(BlueprintReadOnly)
    FString category;
    UPROPERTY(BlueprintReadOnly)
    bool default_;
    GENERATED_USTRUCT_BODY()
    UPROPERTY(BlueprintReadOnly)
    FString name;
    UPROPERTY(BlueprintReadOnly)
    FString description;
};

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolSettings : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    
    UFUNCTION(BlueprintCallable)
    void Push();
    
    void InitDefaults();
    
protected:
    // GAME SETTINGS
    // Чувствительность прицельной камеры
    UPROPERTY(EditAnywhere, Category = Game)
    float FocusCameraHorizontalSensitivity;
public:
    FFloatSettingInfo FocusCameraHorizontalSensitivityInfo = {
        TEXT("Input"),
        1.0f,
        TEXT("Horizontal aim"),
        TEXT("Modifies how sensitive \nthe horizontal aim movement is"),
        0.1f,
        10.0f,
        0.05f
    };
protected:

    UPROPERTY(EditAnywhere, Category = Game)
    float FocusCameraVerticalSensitivity;
public:
    FFloatSettingInfo FocusCameraVerticalSensitivityInfo = {
        TEXT("Input"),
        1.0f,
        TEXT("Vertical aim"),
        TEXT("Modifies how sensitive \nthe vertical aim movement is"),
        0.1f,
        10.0f,
        0.05f
    };
protected:

    // Чувствительность зрительной камеры
    UPROPERTY(EditAnywhere, Category = Game)
    float ViewCameraHorizontalSensitivity;
public:
    FFloatSettingInfo ViewCameraHorizontalSensitivityInfo = {
        TEXT("Input"),
        1.0f,
        TEXT("Horizontal view"),
        TEXT("Modifies how sensitive \nthe horizontal camera movement is\nwhen not aiming"),
        0.1f,
        10.0f,
        0.05f
    };
protected:

    // Чувствительность прицельного pinch
    UPROPERTY(EditAnywhere,Category = Game)
    float FocusPinchSensitivity;
public:
    FFloatSettingInfo FocusPinchSensitivityInfo = {
        TEXT("Input"),
        1.0f,
        TEXT("Aim pinch sensivity"),
        TEXT("Modifies how sensitive \nthe aiming pinch-to-zoom is"),
        0.2f,
        5.0f,
        0.1f
    };
protected:

    // Чувствительность прицельного джойстика
    UPROPERTY(EditAnywhere,Category = Game)
    float FocusJoystickSensitivity;
public:
    FFloatSettingInfo FocusJoystickSensitivityInfo = {
        TEXT("Input"),
        1.0f,
        TEXT("Aim joystick sensitivity"),
        TEXT("Modifies how sensitive \nthe effect joystick is"),
        0.2f,
        5.0f,
        0.1f
    };
protected:

    // Чувствительность 3D джойстика
    UPROPERTY(EditAnywhere,Category = Game)
    float Joystick3DSensitivity;
public:
    FFloatSettingInfo Joystick3DSensitivityInfo = {
        TEXT("Input"),
        1.0f,
        TEXT("3D Joystick sensitivity"),
        TEXT("Modifies how sensitive the 3D joystick is"),
        0.2f,
        5.0f,
        0.1f
    };
protected:

    // Чувствительность ball in hand
    UPROPERTY(EditAnywhere,Category = Game)
    float BallInHandSensitivity;
public:
    FFloatSettingInfo BallInHandSensitivityInfo = {
        TEXT("Input"),
        1.0f,
        TEXT("Ball in hand sensitivity"),
        TEXT("Modifies how sensitive \nthe ball in hand movement is"),
        0.2f,
        5.0f,
        0.1f
    };
protected:

    // Кривая чувствительность силы удара
    UPROPERTY(EditAnywhere,Category=Game)
    EShotStrength ShotStrength;
public:
    FEnumSettingInfo ShotStrengthInfo = {
        TEXT("Input"),
        static_cast<int>(EShotStrength::Linear),
        TEXT("Power bar behavior"),
        TEXT("Modifies the behavior of the power bar\nExponential = 25% power at 50% power bar\nLinear = 50% power at 50% power bar\nLogarithmic = 75% power at 50% power bar"),
        {"Exponential", "Linear", "Logarithmic"}
    };
protected:

    // левша-правша
    UPROPERTY(EditAnywhere,Category=Game)
    EHandOrientation HandOrientation;
public:
    FEnumSettingInfo HandOrientationInfo = {
        TEXT("Input"),
        static_cast<int>(EHandOrientation::RightHanded),
        TEXT("Hand orientation"),
        TEXT("Changes the in-game HUD orientation \nbetween right and left handed"),
        {"Left Handed", "Right Handed"}
    };
protected:

    UPROPERTY(EditAnywhere,Category=Game)
    bool CueAutoReload;
public:
    FBoolSettingInfo CueAutoReloadInfo = {
        TEXT("Game"),
        true,
        TEXT("Cue auto reload"),
        TEXT("Should the cue be auto-recharged")
    };
protected:

    UPROPERTY(EditAnywhere,Category=Game)
    bool AutoReloadChalk;
public:
    FBoolSettingInfo AutoReloadChalkInfo = {
        TEXT("Game"),
        true,
        TEXT("Chalk auto reload"),
        TEXT("Should the chalk be auto-recharged")
    };
protected:

    // Replay хороших ударов
    UPROPERTY(EditAnywhere,Category=Game)
    bool ShouldReplayGoodShots;
public:
    FBoolSettingInfo ShouldReplayGoodShotsInfo = {
        TEXT("Game"),
        false,
        TEXT("Replay good shots"),
        TEXT("Should good shots be replayed")
    };
protected:

    // Авто переход в ball in hand
    UPROPERTY(EditAnywhere,Category=Game)
    bool AutoTranslationBallInHand;
public:
    FBoolSettingInfo AutoTranslationBallInHandInfo = {
        TEXT("Game"),
        false,
        TEXT("Automatic ball in hand"),
        TEXT("Should ball in hand \nbe triggered automatically")
    };
protected:

    // 2D aim to aim off on
    UPROPERTY(EditAnywhere,Category=Game)
    bool TapToAim2D;
public:
    FBoolSettingInfo TapToAim2DInfo = {
        TEXT("Input"),
        true,
        TEXT("2D tap to aim"),
        TEXT("By enabling this you can aim \nby tapping the screen in 2D")
    };
protected:

    // 8ball показывать оставшиеся или забитые шары
    UPROPERTY(EditAnywhere,Category=Game)
    bool ShouldShowPocketedBalls;
public:
    FBoolSettingInfo ShouldShowPocketedBallsInfo = {
        TEXT("Game"),
        true,
        TEXT("Should show pocketed balls"),
        TEXT("Enable to show pocketed balls in the HUD")
    };
protected:

    // Длительность тапа
    UPROPERTY(EditAnywhere,Category=Game)
    float TapDelay;
public:
    FFloatSettingInfo TapDelayInfo = {
        TEXT("Input"),
        0.2f,
        TEXT("Tap delay"),
        TEXT("How long a screen touch should \ncan be to be considered a tap"),
        0.1f,
        1.0f,
        0.01f
    };
protected:

    // Скорость дабл пинч тапа
    UPROPERTY(EditAnywhere,Category=Game)
    float DoublePinchTapSpeed;
public:
    FFloatSettingInfo DoublePinchTapSpeedInfo = {
        TEXT("Input"),
        0.2f,
        TEXT("Two finger double tap speed"),
        TEXT("How big a delay is allowed \nbetween the two finger double taps"),
        0.1f,
        1.0f,
        0.01f
    };

public:
    UFUNCTION(BlueprintCallable)
    float GetFocusCameraHorizontalSensitivity() const { return FocusCameraHorizontalSensitivity;}

    UFUNCTION(BlueprintCallable)
    void SetFocusCameraHorizontalSensitivity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnFocusCameraHorizontalSensitivityChanged OnFocusCameraHorizontalSensitivityChanged;

    UFUNCTION(BlueprintCallable)
    float GetFocusCameraVerticalSensitivity() const { return FocusCameraVerticalSensitivity;}

    UFUNCTION(BlueprintCallable)
    void SetFocusCameraVerticalSensitivity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnFocusCameraVerticalSensitivityChanged OnFocusCameraVerticalSensitivityChanged;

    UFUNCTION(BlueprintCallable)
    float GetViewCameraHorizontalSensitivity() const { return ViewCameraHorizontalSensitivity;}

    UFUNCTION(BlueprintCallable)
    void SetViewCameraHorizontalSensitivity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnViewCameraHorizontalSensitivityChanged OnViewCameraHorizontalSensitivityChanged;

    UFUNCTION(BlueprintCallable)
    float GetFocusPinchSensitivity() const { return FocusPinchSensitivity;}

    UFUNCTION(BlueprintCallable)
    void SetFocusPinchSensitivity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnFocusPinchSensitivityChanged OnFocusPinchSensitivityChanged;

    UFUNCTION(BlueprintCallable)
    float GetFocusJoystickSensitivity() const { return FocusJoystickSensitivity; }

    
    void SetFocusJoystickSensitivity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnFocusJoystickSensitivityChanged OnFocusJoystickSensitivityChanged;

    UFUNCTION(BlueprintCallable)
    float GetJoystick3DSensitivity() const{ return Joystick3DSensitivity;}

    UFUNCTION(BlueprintCallable)
    void SetJoystick3DSensitivity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnJoystick3DSensitivityChanged OnJoystick3DSensitivityChanged;

    UFUNCTION(BlueprintCallable)
    float GetBallInHandSensitivity() const { return BallInHandSensitivity;}

    UFUNCTION(BlueprintCallable)
    void SetBallInHandSensitivity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnBallInHandSensitivityChanged OnBallInHandSensitivityChanged;

    UFUNCTION(BlueprintCallable)
    EShotStrength GetShotStrength() const { return ShotStrength; }
    UFUNCTION()
    int32 GetShotStrength_int() const { return static_cast<int32> (ShotStrength); }

    UFUNCTION(BlueprintCallable)
    void SetShotStrength(EShotStrength NewValue);
    UFUNCTION()
    void SetShotStrength_int(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnShotStrengthChanged OnShotStrengthChanged;

    UFUNCTION(BlueprintCallable)
    EHandOrientation GetHandOrientation() const  { return HandOrientation; }
    UFUNCTION()
    int32 GetHandOrientation_int() const { return static_cast<int32> (HandOrientation); }

    UFUNCTION(BlueprintCallable)
    void SetHandOrientation(EHandOrientation NewValue);
    UFUNCTION()
    void SetHandOrientation_int(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnHandOrientationChanged OnHandOrientationChanged;

    UFUNCTION(BlueprintCallable)
    bool GetCueAutoReload() { return CueAutoReload;}

    UFUNCTION(BlueprintCallable)
    void SetCueAutoReload(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCueAutoReloadChanged OnCueAutoReloadChanged;

    UFUNCTION(BlueprintCallable)
    bool GetAutoReloadChalk() const { return AutoReloadChalk; }

    UFUNCTION(BlueprintCallable)
    void SetAutoReloadChalk(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnAutoReloadChalkChanged OnAutoReloadChalkChanged;

    UFUNCTION(BlueprintCallable)
    bool GetShouldReplayGoodShots() const { return ShouldReplayGoodShots; }

    UFUNCTION(BlueprintCallable)
    void SetShouldReplayGoodShots(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnShouldReplayGoodShotsChanged OnShouldReplayGoodShotsChanged;

    UFUNCTION(BlueprintCallable)
    bool GetAutoTranslationBallInHand() const { return AutoTranslationBallInHand; }

    UFUNCTION(BlueprintCallable)
    void SetAutoTranslationBallInHand(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnAutoTranslationBallInHandChanged OnAutoTranslationBallInHandChanged;

    UFUNCTION(BlueprintCallable)
    bool GetTapToAim2D() const { return TapToAim2D;}

    UFUNCTION(BlueprintCallable)
    void SetTapToAim2D(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOn2DTapToAimChanged On2DTapToAimChanged;
    
    UFUNCTION(BlueprintCallable)
    bool GetShouldShowPocketedBalls() const { return ShouldShowPocketedBalls; }

    UFUNCTION(BlueprintCallable)
    void SetShouldShowPocketedBalls(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnShouldShowPocketedBallsChanged OnShouldShowPocketedBallsChanged;

    UFUNCTION(BlueprintCallable)
    float GetTapDelay() const  { return TapDelay; }

    UFUNCTION(BlueprintCallable)
    void SetTapDelay(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnTapDelayChanged OnTapDelayChanged;

    UFUNCTION(BlueprintCallable)
    float GetDoublePinchTapSpeed() const { return DoublePinchTapSpeed;}

    UFUNCTION(BlueprintCallable)
    void SetDoublePinchTapSpeed(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnDoublePinchTapSpeedChanged OnDoublePinchTapSpeedChanged;

protected:
    // TRAINING ONLY настройки сбрасываются после выхода из тренировки

    // Hard mode вкл/выкл (только тренировка, влияет на подсчет угла кия)
    UPROPERTY(EditAnywhere,Category= Training)
    bool HardMode;
public:
    FBoolSettingInfo HardModeInfo = {
        TEXT("Game"),
        false,
        TEXT("Hard mode"),
        TEXT("In Hard Mode cue elevation is always enabled\nThis means that each shot \nwill deviate from the straight trajectory\nmore when the cue is angled more\nBy changing the cue angle \nyou have a Hard Mode shot automatically")
    };
protected:

    // Размер физического сабстепа в мс
    UPROPERTY(EditAnywhere,Category= Training)
    float PhysSubstepSize;
public:
    FFloatSettingInfo PhysSubstepSizeInfo = {
        TEXT("Physics"),
        1.0f,
        TEXT("Substep size"),
        TEXT("How big should a computational step \nof the physics engine be in ms\nChoose less for better performance, \nmore for better accuracy"),
        0.1f,
        10.0f,
        0.1f
    };
protected:

    // Трение качения шар стол
    UPROPERTY(EditAnywhere,Category= Training)
    float RollingFriction;
public:
    FFloatSettingInfo RollingFrictionInfo = {
        TEXT("Physics"),
        0.01f,
        TEXT("Rolling"),
        TEXT("Rolling friction between \nthe balls and the table\nWith higher rolling friction the balls \ndo not go as far after stopping sliding"),
        0.0001f,
        0.2f,
        0.0001f
    };
protected:

    // • Трение скольжения шар стол
    UPROPERTY(EditAnywhere,Category= Training)
    float SlipFrictionBallTable;
public:
    FFloatSettingInfo SlipFrictionBallTableInfo = {
        TEXT("Physics"),
        0.2f,
        TEXT("Sliding"),
        TEXT("Sliding friction between \nthe balls and the table\nWith higher sliding friction the balls \ndo slide less and start rolling sooner"),
        0.001f,
        0.6f,
        0.001f
    };
protected:

    // • Трение (скольжения) шар-шар
    UPROPERTY(EditAnywhere,Category= Training)
    float SlipFrictionBallBall;
public:
    FFloatSettingInfo SlipFrictionBallBallInfo = {
        TEXT("Physics"),
        0.001f,
        TEXT("Sliding ball-ball"),
        TEXT("Sliding friction between balls\nHigher sliding ball to ball friction \nmeans spin between balls is transferred \nmore effectively"),
        0.0001f,
        0.1f,
        0.0001f
    };
protected:

    // • Restitution шар-сукно
    UPROPERTY(EditAnywhere,Category= Training)
    float RestrictionBallCloth;
public:
    FFloatSettingInfo RestrictionBallClothInfo = {
        TEXT("Physics"),
        0.5f,
        TEXT("Table restitution"),
        TEXT("How much vertical velocity \nshould the balls retain \nafter hitting the table"),
        0.01f,
        0.99f,
        0.01f
    };
protected:

    // • Normal velocity loss шар боковина
    UPROPERTY(EditAnywhere,Category= Training)
    float NormalVelocityLossValue;
public:
    FFloatSettingInfo NormalVelocityLossValueInfo = {
        TEXT("Physics"),
        0.2f,
        TEXT("Normal velocity loss"),
        TEXT("How much perpendicular velocity \nshould the balls lose after hitting \nthe cushions"),
        0.01f,
        0.99f,
        0.01f
    };
protected:

    // • Ускорение свободного падения
    UPROPERTY(EditAnywhere,Category= Training)
    float AccelerationOfGravity;
public:
    FFloatSettingInfo AccelerationOfGravityInfo = {
        TEXT("Physics"),
        FREE_FALL_ACCELERATION,
        TEXT("Acceleration of gravity"),
        TEXT("Free falling acceleration\nHigher acceleration means \nbetter grip with the table"),
        FREE_FALL_ACCELERATION/11.0f,
        FREE_FALL_ACCELERATION*11.0f,
        (FREE_FALL_ACCELERATION*11.0f - FREE_FALL_ACCELERATION/11.0f) / 100.0f
    };
protected:

    // • Угловой демпинг вдоль вертикальной оси
    UPROPERTY(EditAnywhere,Category= Training)
    float VerticalAngleAngularDemping;
public:
    FFloatSettingInfo VerticalAngleAngularDempingInfo = {
        TEXT("Physics"),
        10.0f,
        TEXT("Vertical angular damping"),
        TEXT("How fast the balls lose \nrotational velocity around \nthe vertical axis"),
        1.0f,
        100.0f,
        1.0f
    };
protected:
    
    UPROPERTY(EditAnywhere,Category= Training)
    float SlidingResistanceVelocityDeltaTrigger;
public:
    FFloatSettingInfo SlidingResistanceVelocityDeltaTriggerInfo = {
        TEXT("Physics"),
        1.0f,
        TEXT("Sliding velocity delta"),
        TEXT("How fast the surface of the ball \nat the point of cloth contact can be moving\nto still be considered rolling"),
        0.1f,
        10.0f,
        0.01f
    };
protected:

    // Коэффициент распределения массы по шару (из момента инерции)
    UPROPERTY(EditAnywhere,Category= Training)
    float CoefBallMassDistribution;
public:
    FFloatSettingInfo CoefBallMassDistributionInfo = {
        TEXT("Physics"),
        2.5f,
        TEXT("Moment of inertia"),
        TEXT("How should be the ball's mass \nbe distributed\n2.5 = normal solid ball\n1.5 = hollow sphere"),
        0.5f,
        10.0f,
        0.1f
    };
protected:


    // Коэффициент потери поверхностной скорости шара при касании стенки
    UPROPERTY(EditAnywhere,Category= Training)
    float CoefBallSurfaceVelocityLoss;
public:
    FFloatSettingInfo CoefBallSurfaceVelocityLossInfo = {
        TEXT("Physics"),
        0.005f,
        TEXT("Linear velocity loss"),
        TEXT("How much linear surface velocity \nis lost on cushion contact"),
        0.0f,
        0.01f,
        0.001f
    };
protected:

    // Коэффициент превращения потери поверхностной скорости в потерю линейной скорости при касании стенки
    UPROPERTY(EditAnywhere,Category= Training)
    float CoefLossVelocityIntoLossLinearVelocity;
public:
    FFloatSettingInfo CoefLossVelocityIntoLossLinearVelocityInfo = {
        TEXT("Physics"),
        0.45f,
        TEXT("Velocity loss translation"),
        TEXT("What part of the lost linear \nsurface velocity is translated \nin lost ball linear velocity \non cushion contact"),
        0.f,
        1.0f,
        0.1f
    };
protected:

    // Коэффициент профиля боковин
    UPROPERTY(EditAnywhere,Category= Training)
    float CoefSidewallProfile;
public:
    FFloatSettingInfo CoefSidewallProfileInfo = {
        TEXT("Physics"),
        5.0f,
        TEXT("Profile"),
        TEXT("How much vertical velocity \nis lost on cushion contact"),
        1.0f,
        20.0f,
        1.0f
    };
    
public:
    UFUNCTION(BlueprintCallable)
    void Reset();
    UFUNCTION(BlueprintCallable)
    void ResetPhysics();

    UFUNCTION(BlueprintCallable)
    bool GetHardMode() const  { return HardMode;}

    UFUNCTION(BlueprintCallable)
    void SetHardMode(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnHardModeChanged OnHardModeChanged;

    UFUNCTION(BlueprintCallable)
    float GetPhysSubstepSize() const { return PhysSubstepSize;}

    UFUNCTION(BlueprintCallable)
    void SetPhysSubstepSize(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnPhysSubstepSizeChanged OnPhysSubstepSizeChanged;

    UFUNCTION(BlueprintCallable)
    float GetRollingFriction() const { return RollingFriction;}

    UFUNCTION(BlueprintCallable)
    void SetRollingFriction(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnRollingFrictionChanged OnRollingFrictionChanged;

    UFUNCTION(BlueprintCallable)
    float GetSlipFrictionBallTable() const { return SlipFrictionBallTable; }

    UFUNCTION(BlueprintCallable)
    void SetSlipFrictionBallTable(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnSlipFrictionBallTableChanged OnSlipFrictionBallTableChanged;

    UFUNCTION(BlueprintCallable)
    float GetSlipFrictionBallBall() const { return SlipFrictionBallBall; }

    UFUNCTION(BlueprintCallable)
    void SetSlipFrictionBallBall(float NewValue);

    UPROPERTY(BlueprintCallable)
    FOnSlipFrictionBallBallChanged OnSlipFrictionBallBallChanged;

    UFUNCTION(BlueprintCallable)
    float GetRestrictionBallCloth() const { return RestrictionBallCloth; }

    UFUNCTION(BlueprintCallable)
    void SetRestrictionBallCloth(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnRestrictionBallClothChanged OnRestrictionBallClothChanged;

    UFUNCTION(BlueprintCallable)
    float GetNormalVelocityLossValue() { return NormalVelocityLossValue; }

    UFUNCTION(BlueprintCallable)
    void SetNormalVelocityLossValue(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnNormalVelocityLossValueChanged OnNormalVelocityLossValueChanged;

    UFUNCTION(BlueprintCallable)
    float GetAccelerationOfGravity() const { return AccelerationOfGravity;}

    UFUNCTION(BlueprintCallable)
    void SetAccelerationOfGravity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnAccelerationOfGravityChanged OnAccelerationOfGravityChanged;

    UFUNCTION(BlueprintCallable)
    float GetVerticalAngleAngularDemping() const { return VerticalAngleAngularDemping;}

    UFUNCTION(BlueprintCallable)
    void SetVerticalAngleAngularDemping(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnVerticalAngleAngularDempingChanged OnVerticalAngleAngularDempingChanged;

    UFUNCTION(BlueprintCallable)
    float GetSlidingResistanceVelocityDeltaTrigger() const { return SlidingResistanceVelocityDeltaTrigger;}

    UFUNCTION(BlueprintCallable)
    void SetSlidingResistanceVelocityDeltaTrigger(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnSlidingResistanceVelocityDeltaTriggerChanged OnSlidingResistanceVelocityDeltaTriggerChanged;

    UFUNCTION(BlueprintCallable)
    float GetCoefBallMassDistribution() const { return CoefBallMassDistribution;}

    UFUNCTION(BlueprintCallable)
    void SetCoefBallMassDistribution(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCoefBallMassDistributionChanged OnCoefBallMassDistributionChanged;

    UFUNCTION(BlueprintCallable)
    float GetCoefBallSurfaceVelocityLoss() const { return CoefBallSurfaceVelocityLoss;}

    UFUNCTION(BlueprintCallable)
    void SetCoefBallSurfaceVelocityLoss(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCoefBallSurfaceVelocityLossChanged OnCoefBallSurfaceVelocityLossChanged;

    UFUNCTION(BlueprintCallable)
    float GetCoefLossVelocityIntoLossLinearVelocity() const { return CoefLossVelocityIntoLossLinearVelocity;}

    UFUNCTION(BlueprintCallable)
    void SetCoefLossVelocityIntoLossLinearVelocity(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCoefLossVelocityIntoLossLinearVelocityChanged OnCoefLossVelocityIntoLossLinearVelocityChanged;

    UFUNCTION(BlueprintCallable)
    float GetCoefSidewallProfile() const { return CoefSidewallProfile;}

    UFUNCTION(BlueprintCallable)
    void SetCoefSidewallProfile(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCoefSidewallProfileChanged OnCoefSidewallProfileChanged;
    
protected:
    // Графика камера
    UPROPERTY(EditAnywhere,Category= Graphic)
    bool DrawBallsShadows;
public:
    FBoolSettingInfo DrawBallsShadowsInfo = {
        TEXT("Graphics"),
        true,
        TEXT("Draw ball shadows"),
        TEXT("Disable to not draw \nball shadows \nfor better performance")
    };
protected:

    UPROPERTY(EditAnywhere,Category= Graphic)
    ECameraMoveMode CameraMoveMode;
public:
    FEnumSettingInfo CameraMoveModeInfo = {
        TEXT("Game"),
        static_cast<int>(ECameraMoveMode::FullMode),
        TEXT("Camera move mode"),
        TEXT("Full mode: All camera animations are active\nOnly pockets: pocket cameras for straight shots\nNone: minimal camera animations"),
        {"FullMode", "Only pockets", "None"}
    };
protected:

    UPROPERTY(EditAnywhere,Category= Graphic)
    bool UseCameraAnimation;
    //FBoolSettingInfo UseCameraAnimationInfo;

    UPROPERTY(EditAnywhere,Category= Graphic)
    EPreferredCameraMode PreferredCameraMode;
public:
    FEnumSettingInfo PreferredCameraModeInfo = {
        TEXT("Game"),
        static_cast<int>(EPreferredCameraMode::Cam3D),
        TEXT("Preferred camera mode"),
        TEXT("Which camera mode should \neach game start in"),
        {"2D","3D"}
    };
protected:
    
    UPROPERTY(EditAnywhere,Category= Graphic)
    float SpectatingCamPosition;
public:
    FFloatSettingInfo SpectatingCamPositionInfo = {
        TEXT("Game"),
        0.606f,
        TEXT("Position"),
        TEXT("Where around the table \nshould the spectating camera \nbe placed"),
        0.0f,
        1.0f,
        0.001f
    };
protected:
    UPROPERTY(EditAnywhere,Category= Graphic)
    float SpectatingCamHeight;
public:
    FFloatSettingInfo SpectatingCamHeightInfo = {
        TEXT("Game"),
        80.0f,
        TEXT("Height"),
        TEXT("How high above the table \nshould the spectating camera be"),
        0.0f,
        200.0f,
        1.0f
    };
protected:
    
    UPROPERTY(EditAnywhere,Category= Graphic)
    float SpectatingCamFOV;
public:
    FFloatSettingInfo SpectatingCamFOVInfo = {
        TEXT("Game"),
        90.0f,
        TEXT("FOV"),
        TEXT("How wide should \nthe spectating camera lens be"),
        60.0f,
        120.0f,
        1.0f
    };
protected:
    
    UPROPERTY(EditAnywhere,Category= Graphic)
    float SpectatingCamCenteringHeight;
public:
    FFloatSettingInfo SpectatingCamCenteringHeightInfo = {
        TEXT("Game"),
        90.0f,
        TEXT("Aim height"),
        TEXT("How high should \nthe spectating camera be aimed at"),
        0.0f,
        140.0f,
        1.0f
    };
protected:

    UPROPERTY(EditAnywhere,Category= Sound)
    float MusicVolume;
    FFloatSettingInfo MusicVolumeInfo;

    UPROPERTY(EditAnywhere,Category= Sound)
    float EffectSoundsVolume;
    FFloatSettingInfo EffectSoundsVolumeInfo;

    UPROPERTY(EditAnywhere,Category= Sound)
    float GameSoundsVolume;
    FFloatSettingInfo GameSoundsVolumeInfo;

    UPROPERTY(EditAnywhere,Category= General)
    EMenuOrientation MenuOrientation;
    FEnumSettingInfo MenuOrientationInfo;

public:
    UFUNCTION(BlueprintCallable)
    bool GetDrawBallsShadows() const { return DrawBallsShadows;}

    UFUNCTION(BlueprintCallable)
    void SetDrawBallsShadows(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnDrawBallsShadowsChanged OnDrawBallsShadowsChanged;

    UFUNCTION(BlueprintCallable)
    ECameraMoveMode GetCameraMoveMode() const { return CameraMoveMode; }
    UFUNCTION()
    int32 GetCameraMoveMode_int() const { return static_cast<int32> (CameraMoveMode); }

    UFUNCTION(BlueprintCallable)
    void SetCameraMoveMode(ECameraMoveMode NewValue);
    UFUNCTION()
    void SetCameraMoveMode_int(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCameraMoveModeChanged OnCameraMoveModeChanged;

    UFUNCTION(BlueprintCallable)
    bool GetUseCameraAnimation() const { return UseCameraAnimation;}

    UFUNCTION(BlueprintCallable)
    void SetUseCameraAnimation(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnUseCameraAnimationChanged OnUseCameraAnimationChanged;

    UFUNCTION(BlueprintCallable)
    EPreferredCameraMode GetPreferredCameraMode() const { return PreferredCameraMode;}
    UFUNCTION()
    int32 GetPreferredCameraMode_int() const { return static_cast<int32> (PreferredCameraMode); }

    UFUNCTION(BlueprintCallable)
    void SetPreferredCameraMode(EPreferredCameraMode NewValue);
    UFUNCTION()
    void SetPreferredCameraMode_int(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnPreferredCameraModeChanged OnPreferredCameraModeChanged;
    
    UFUNCTION(BlueprintCallable)
    float GetSpectatingCamPosition() const { return SpectatingCamPosition;}

    UFUNCTION(BlueprintCallable)
    void SetSpectatingCamPosition(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnSpectatingCamPositionChanged SpectatingCamPositionChanged;
    
    UFUNCTION(BlueprintCallable)
    float GetSpectatingCamHeight() const { return SpectatingCamHeight;}

    UFUNCTION(BlueprintCallable)
    void SetSpectatingCamHeight(float newValue);

    UPROPERTY(BlueprintAssignable)
    FOnSpectatingCamHeightChanged SpectatingCamHeightChanged;
    
    UFUNCTION(BlueprintCallable)
    float GetSpectatingCamFOV() const { return SpectatingCamFOV;}

    UFUNCTION(BlueprintCallable)
    void SetSpectatingCamFOV(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnSpectatingCamFOVChanged SpectatingCamFOVChanged;
    
    UFUNCTION(BlueprintCallable)
    float GetSpectatingCamCenteringHeight() const { return SpectatingCamCenteringHeight;}

    UFUNCTION(BlueprintCallable)
    void SetSpectatingCamCenteringHeight(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnSpectatingCamCenteringHeightChanged SpectatingCamCenteringHeightChanged;

    UFUNCTION(BlueprintCallable)
    float GetMusicVolume() const { return MusicVolume;}

    UFUNCTION(BlueprintCallable)
    void SetMusicVolume(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnMusicVolumeChanged OnMusicVolumeChanged;

    UFUNCTION(BlueprintCallable)
    float GetEffectSoundsVolume() const { return EffectSoundsVolume;}

    UFUNCTION(BlueprintCallable)
    void SetEffectSoundsVolume(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnEffectSoundsVolumeChanged OnEffectSoundsVolumeChanged;

    UFUNCTION(BlueprintCallable)
    float GetGameSoundsVolume() const { return GameSoundsVolume;}

    UFUNCTION(BlueprintCallable)
    void SetGameSoundsVolume(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnGameSoundsVolumeChanged OnGameSoundsVolumeChanged;

    UFUNCTION(BlueprintCallable)
    EMenuOrientation GetMenuOrientation() const { return MenuOrientation; }
    UFUNCTION()
    int32 GetMenuOrientation_int() const { return static_cast<int32> (MenuOrientation); }

    UFUNCTION(BlueprintCallable)
    void SetMenuOrientation(EMenuOrientation NewValue);
    UFUNCTION()
    void SetMenuOrientation_int(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnMenuOrientationChanged OnMenuOrientationChanged;
    
protected:
    // Physics
    // Контесты
    UPROPERTY(EditAnywhere,Category= Contest)
    float CenterofCircleCOF;
    FFloatSettingInfo CenterofCircleCOFInfo;
    
    UPROPERTY(EditAnywhere,Category= Graphic)
    float COFRadius;
    FFloatSettingInfo COFRadiusInfo;

    UPROPERTY(EditAnywhere,Category= Graphic)
    int32 CountOfBallsCOF;
    //FFloatSettingInfo CountOfBallsCOFInfo;

    UPROPERTY(EditAnywhere,Category= Graphic)
    FVector2D LineUpDefaultPos;
    //FFloatSettingInfo LineUpDefaultPosInfo;

    UPROPERTY(EditAnywhere,Category= Graphic)
    int32 FoulContractSP;
    //FFloatSettingInfo FoulContractSPInfo;
    
public:
    UFUNCTION(BlueprintCallable)
    float GetCenterofCircleCOF() const { return CenterofCircleCOF;}

    UFUNCTION(BlueprintCallable)
    void SetCenterofCircleCOF(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCenterofCircleCOFChanged OnCenterofCircleCofChanged;

    UFUNCTION(BlueprintCallable)
    float GetCOFRadius() const { return COFRadius;}

    UFUNCTION(BlueprintCallable)
    void SetCOFRadius(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCOFRadiusChanged OnCofRadiusChanged;

    UFUNCTION(BlueprintCallable)
    int32 GetCountOfBallsCOF() { return CountOfBallsCOF;}

    UFUNCTION(BlueprintCallable)
    void SetCountOfBallsCOF(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCountOfBallsCOFChanged OnCountOfBallsCofChanged;

    UFUNCTION(BlueprintCallable)
    FVector2D GetLineUpDefaultPos() { return LineUpDefaultPos;}

    UFUNCTION(BlueprintCallable)
    void SetLineUpDefaultPos(FVector2D NewValue);

    UPROPERTY(BlueprintCallable)
    FOnLineUpDefaultPosChanged OnLineUpDefaultPosChanged;

    UFUNCTION(BlueprintCallable)
    int32 GetFoulContractSP() const { return FoulContractSP;}

    UFUNCTION(BlueprintCallable)
    void SetFoulContractSP(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnFoulContractSPChanged OnFoulContractSPChanged;
    
protected:
    // visualization
    UPROPERTY(EditAnywhere,Category= Visualization)
    float FirstDataSendDelay;

    UPROPERTY(EditAnywhere,Category= Visualization)
    bool bUseInterpolation;

public:
    UFUNCTION(BlueprintCallable)
    float GetFirstDataSendDelay() const { return FirstDataSendDelay;}

    UFUNCTION(BlueprintCallable)
    void SetFirstDataSendDelay(float NewValue);

    UPROPERTY(BlueprintCallable)
    FOnFirstDataSendDelayChanged OnFirstDataSendDelayChanged;

    UFUNCTION(BlueprintCallable)
    float GetUseInterpolation() const { return bUseInterpolation; }

    UFUNCTION(BlueprintCallable)
    void SetUseInterpolation(bool NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnUseInterpolationChanged OnUseInterpolationChanged;

protected:
    // Connectivity
    UPROPERTY(EditAnywhere,Category= Connectivity)
    float LobbyPingFrequency;

    UPROPERTY(EditAnywhere,Category= Connectivity)
    float WaitingPingDelay;

    UPROPERTY(EditAnywhere,Category= Connectivity)
    FString ReleaseBackendAddress;

    UPROPERTY(EditAnywhere,Category= Connectivity)
    FString PreReleaseBackendAddress;

public:
    UFUNCTION(BlueprintCallable)
    float GetLobbyPingFrequency() const { return LobbyPingFrequency;}

    UFUNCTION(BlueprintCallable)
    void SetLobbyPingFrequency(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnLobbyPingFrequencyChanged OnLobbyPingFrequencyChanged;

    UFUNCTION(BlueprintCallable)
    float GetWaitingPingDelay() const { return WaitingPingDelay;}

    UFUNCTION(BlueprintCallable)
    void SetWaitingPingDelay(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnWaitingPingDelayChanged OnWaitingPingDelayChanged;

    UFUNCTION(BlueprintCallable)
    FString GetReleaseBackendAddress() const { return ReleaseBackendAddress;}

    UFUNCTION(BlueprintCallable)
    void SetReleaseBackendAddress(FString NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnReleaseBackendAddressChanged OnReleaseBackendAddressChanged;

    UFUNCTION(BlueprintCallable)
    FString GetPreReleaseBackendAdress() const { return PreReleaseBackendAddress;}

    UFUNCTION(BlueprintCallable)
    void SetPreReleaseBackendAdress(FString NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnPreReleaseBackendAddressChanged OnPreReleaseBackendAddressChanged;
    
protected:
    // PoolPlayer
    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float CameraViewStack;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float MaxAimDisplacementCoef;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float CameraLagSpeed;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    FVector DefaultCameraPos;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    FVector CueRelativeLocation;
    
    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    FVector HitPointScale;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float SoftMinimumCueAngle;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float SoftMaxCueAngle;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float HardMaxCueAngle;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float AngleTriggerNonIntensiveCamera;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float CueLength;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float MaxAimVerticalAdjustment;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    ESideCamera SideCameraMoving;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float CloseUpCameraZooming;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float MinDistFromTable;

    UPROPERTY(EditAnywhere,Category= PoolPlayer)
    float MaxDistFromTable;
public:
    UFUNCTION(BlueprintCallable)
    float GetCameraViewStack() const { return CameraViewStack; }

    UFUNCTION(BlueprintCallable)
    void SetCameraViewStack(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCameraViewStackChanged OnCameraViewStackChanged;

    UFUNCTION(BlueprintCallable)
    float GetMaxAimDisplacementCoef() const { return MaxAimDisplacementCoef; }

    UFUNCTION(BlueprintCallable)
    void SetMaxAimDisplacementCoef(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnMaxAimDisplacementCoefChanged OnMaxAimDisplacementCoefChanged;

    UFUNCTION(BlueprintCallable)
    float GetCameraLagSpeed() const { return CameraLagSpeed; }

    UFUNCTION(BlueprintCallable)
    void SetCameraLagSpeed(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCameraLagSpeedChanged OnCameraLagSpeedChanged;

    UFUNCTION(BlueprintCallable)
    FVector GetDefaultCameraPos() const { return DefaultCameraPos; }

    UFUNCTION(BlueprintCallable)
    void SetDefaultCameraPos(FVector NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnDefaultCameraPosChanged OnDefaultCameraPosChanged;

    UFUNCTION(BlueprintCallable)
    FVector GetCueRelativeLocation() const { return CueRelativeLocation; }

    UFUNCTION(BlueprintCallable)
    void SetCueRelativeLocation(FVector NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCueRelativeLocationChanged OnCueRelativeLocationChanged;

    UFUNCTION(BlueprintCallable)
    FVector GetHitPointScale() const { return HitPointScale; }

    UFUNCTION(BlueprintCallable)
    void SetHitPointScale(FVector NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnHitPointScaleChanged OnHitPointScaleChanged;

    UFUNCTION(BlueprintCallable)
    float GetSoftMinimumCueAngle() const { return SoftMinimumCueAngle; }

    UFUNCTION(BlueprintCallable)
    void SetSoftMinimumCueAngle(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnSoftMinimumCueAngleChanged OnSoftMinimumCueAngleChanged;

    UFUNCTION(BlueprintCallable)
    float GetSoftMaxCueAngle() const { return SoftMaxCueAngle;}

    UFUNCTION(BlueprintCallable)
    void SetSoftMaxCueAngle(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnSoftMaxCueAngleChanged OnSoftMaxCueAngleChanged;

    UFUNCTION(BlueprintCallable)
    float GetHardMaxCueAngle() const { return HardMaxCueAngle; }

    UFUNCTION(BlueprintCallable)
    void SetHardMaxCueAngle(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnHardMaxCueAngleChanged OnHardMaxCueAngleChanged;

    UFUNCTION(BlueprintCallable)
    float GetAngleTriggerNonIntensiveCamera() const { return AngleTriggerNonIntensiveCamera; }

    UFUNCTION(BlueprintCallable)
    void SetAngleTriggerNonIntensiveCamera(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnAngleTriggerNonIntensiveCameraChanged OnAngleTriggerNonIntensiveCameraChanged;

    UFUNCTION(BlueprintCallable)
    float GetCueLength() const { return CueLength; }

    UFUNCTION(BlueprintCallable)
    void SetCueLength(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCueLengthChanged OnCueLengthChanged;

    UFUNCTION(BlueprintCallable)
    float GetMaxAimVerticalAdjustment() const { return MaxAimVerticalAdjustment; }

    UFUNCTION(BlueprintCallable)
    void SetMaxAimVerticalAdjustment(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnMaxAimVerticalAdjustmentChanged OnMaxAimVerticalAdjustmentChanged;

    UFUNCTION(BlueprintCallable)
    ESideCamera GetSideCameraMoving() const { return SideCameraMoving; }
    UFUNCTION()
    int32 GetSideCameraMoving_int() const { return static_cast<int32> (SideCameraMoving); }

    UFUNCTION(BlueprintCallable)
    void SetSideCameraMoving(ESideCamera NewValue);
    UFUNCTION()
    void SetSideCameraMoving_int(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnSideCameraMovingChanged OnSideCameraMovingChanged;

    UFUNCTION(BlueprintCallable)
    float GetCloseUpCameraZooming() const { return CloseUpCameraZooming; }

    UFUNCTION(BlueprintCallable)
    void SetCloseUpCameraZooming(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCloseUpCameraZoomingChanged OnCloseUpCameraZoomingChanged;

    UFUNCTION(BlueprintCallable)
    float GetMinDistFromTable() const { return MinDistFromTable; }

    UFUNCTION(BlueprintCallable)
    void SetMinDistFromTable(float NewValue);

    UPROPERTY(BlueprintCallable)
    FOnMinDistFromTableChanged OnMinDistFromTableChanged;

    UFUNCTION(BlueprintCallable)
    float GetMaxDistFromTable() { return MaxDistFromTable; }

    UFUNCTION(BlueprintCallable)
    void SetMaxDistFromTable(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnMaxDistFromTableChanged OnMaxDistFromTableChanged;

protected:
    // MISC
    UPROPERTY(EditAnywhere,Category = MISC)
    int32 ChalkRetentionShotCount;

    UPROPERTY(EditAnywhere,Category = MISC)
    int32 BallCountInScene;

    UPROPERTY(EditAnywhere,Category = MISC)
    float TriggerPerpendicularCameraCatch;

    UPROPERTY(EditAnywhere,Category= MISC)
    float CameraToAimFrom3DAnimLength;

    UPROPERTY(EditAnywhere,Category= MISC)
    float CubicFocusAnimLength;

    UPROPERTY(EditAnywhere,Category= MISC)
    FVector2D PointToCameraMove;

    UPROPERTY(EditAnywhere,Category= MISC)
    FVector2D VerticalPartOfTheScreenOccupiedByTable;

    UPROPERTY(EditAnywhere,Category= MISC)
    float GameFlowAimDelay;

    UPROPERTY(EditAnywhere,Category= MISC)
    float OpponentPositionReplicateFrequency;

    UPROPERTY(EditAnywhere,Category= MISC)
    float ReadyMessageSendDelay;

    UPROPERTY(EditAnywhere,Category= MISC)
    float DelayToTravelToNextStage;

    UPROPERTY(EditAnywhere,Category= MISC)
    float DelayToChangeLevel;

    UPROPERTY(EditAnywhere,Category= MISC)
    float AngularShiftPocketCameraFromVertical;

    UPROPERTY(EditAnywhere,Category= MISC)
    float AngularShiftPocketCameraFromHorizontal;

    UPROPERTY(EditAnywhere,Category= MISC)
    float ShiftCentralPocketCameraFromVertical;

    UPROPERTY(EditAnywhere,Category= MISC)
    float ShiftCentralPocketCameraFromHorizontal;

public:

    UFUNCTION(BlueprintCallable)
    int32 GetChalkRetentionShotCount() const { return ChalkRetentionShotCount; }

    UFUNCTION(BlueprintCallable)
    void SetChalkRetentionShotCount(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnChalkRetentionShotCountChanged OnChalkRetentionShotCountChanged;

    UFUNCTION(BlueprintCallable)
    int32 GetBallCountInScene() const { return BallCountInScene; }

    UFUNCTION(BlueprintCallable)
    void SetBallCountInScene(int32 NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnBallCountInSceneChanged OnBallCountInSceneChanged;

    UFUNCTION(BlueprintCallable)
    float GetTriggerPerpendicularCameraCatch() const { return TriggerPerpendicularCameraCatch; }

    UFUNCTION(BlueprintCallable)
    void SetTriggerPerpendicularCameraCatch(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnTriggerPerpendicularCameraCatchChanged OnTriggerPerpendicularCameraCatchChanged;

    UFUNCTION(BlueprintCallable)
    float GetCameraToAimFrom3DAnimLength() const { return CameraToAimFrom3DAnimLength; }

    UFUNCTION(BlueprintCallable)
    void SetCameraToAimFrom3DAnimLength(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCameraToAimFrom3DAnimLengthChanged OnCameraToAimFrom3DAnimLengthChanged;

    UFUNCTION(BlueprintCallable)
    float GetCubicFocusAnimLength() const { return CubicFocusAnimLength; }

    UFUNCTION(BlueprintCallable)
    void SetCubicFocusAnimLength(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnCubicFocusAnimLengthChanged OnCubicFocusAnimLengthChanged;

    UFUNCTION(BlueprintCallable)
    FVector2D GetPointToCameraMove() const { return PointToCameraMove; }

    UFUNCTION(BlueprintCallable)
    void SetPointToCameraMove(FVector2D NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnPointToCameraMoveChanged OnPointToCameraMoveChanged;

    UFUNCTION(BlueprintCallable)
    FVector2D GetVerticalPartOfTheScreenOccupiedByTable() const { return VerticalPartOfTheScreenOccupiedByTable; }

    UFUNCTION(BlueprintCallable)
    void SetVerticalPartOfTheScreenOccupiedByTable(FVector2D NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnVerticalPartOfTheScreenOccupiedByTableChanged OnVerticalPartOfTheScreenOccupiedByTableChanged;

    UFUNCTION(BlueprintCallable)
    float GetGameFlowAimDelay() const { return GameFlowAimDelay; }

    UFUNCTION(BlueprintCallable)
    void SetGameFlowAimDelay(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnGameFlowAimDelayChanged OnGameFlowAimDelayChanged;

    UFUNCTION(BlueprintCallable)
    float GetOpponentPositionReplicateFrequency() const { return OpponentPositionReplicateFrequency; }

    UFUNCTION(BlueprintCallable)
    void SetOpponentPositionReplicateFrequency(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnOpponentPositionReplicateFrequencyChanged OnOpponentPositionReplicateFrequencyChanged;

    UFUNCTION(BlueprintCallable)
    float GetReadyMessageSendDelay() const { return ReadyMessageSendDelay; }

    UFUNCTION(BlueprintCallable)
    void SetReadyMessageSendDelay(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnReadyMessageSendDelayChanged OnReadyMessageSendDelayChanged;

    UFUNCTION(BlueprintCallable)
    float GetDelayToTravelToNextStage() const { return DelayToTravelToNextStage; }

    UFUNCTION(BlueprintCallable)
    void SetDelayToTravelToNextStage(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnDelayToTravelToNextStageChanged OnDelayToTravelToNextStageChanged;

    UFUNCTION(BlueprintCallable)
    float GetDelayToChangeLevel() const { return DelayToChangeLevel; }

    UFUNCTION(BlueprintCallable)
    void SetDelayToChangeLevel(float NewValue);
    
    UPROPERTY(BlueprintAssignable)
    FOnDelayToChangeLevelChanged OnDelayToChangeLevelChanged;

    UFUNCTION(BlueprintCallable)
    float GetAngularShiftPocketCameraFromVertical() const { return AngularShiftPocketCameraFromVertical; }

    UFUNCTION(BlueprintCallable)
    void SetAngularShiftPocketCameraFromVertical(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnAngularShiftPocketCameraFromVerticalChanged OnAngularShiftPocketCameraFromVerticalChanged;

    UFUNCTION(BlueprintCallable)
    float GetAngularShiftPocketCameraFromHorizontal() const { return AngularShiftPocketCameraFromHorizontal; }

    UFUNCTION(BlueprintCallable)
    void SetAngularShiftPocketCameraFromHorizontal(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnAngularShiftPocketCameraFromHorizontalChanged OnAngularShiftPocketCameraFromHorizontalChanged;

    UFUNCTION(BlueprintCallable)
    float GetShiftCentralPocketCameraFromVertical() const { return ShiftCentralPocketCameraFromVertical; }

    UFUNCTION(BlueprintCallable)
    void SetShiftCentralPocketCameraFromVertical(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnShiftCentralPocketCameraFromVerticalChanged OnShiftCentralPocketCameraFromVerticalChanged;

    UFUNCTION(BlueprintCallable)
    float GetShiftCentralPocketCameraFromHorizontal() const { return ShiftCentralPocketCameraFromHorizontal; }

    UFUNCTION(BlueprintCallable)
    void SetShiftCentralPocketCameraFromHorizontal(float NewValue);

    UPROPERTY(BlueprintAssignable)
    FOnShiftCentralPocketCameraFromHorizontalChanged OnShiftCentralPocketCameraFromHorizontalChanged;
    
};


