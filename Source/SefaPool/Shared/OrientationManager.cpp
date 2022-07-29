// Fill out your copyright notice in the Description page of Project Settings.


#include "OrientationManager.h"

#include "GenericPlatform/GenericPlatformMisc.h"
#include "SharedDefs.h"

enum class AndroidOrientations : int
{
    SCREEN_ORIENTATION_LANDSCAPE = 0,
    SCREEN_ORIENTATION_PORTRAIT = 1,
    SCREEN_ORIENTATION_REVERSE_LANDSCAPE = 8,
    SCREEN_ORIENTATION_REVERSE_PORTRAIT = 9,
    SCREEN_ORIENTATION_SENSOR = 4,
    SCREEN_ORIENTATION_SENSOR_LANDSCAPE = 6,
    SCREEN_ORIENTATION_SENSOR_PORTRAIT = 7,
    SCREEN_ORIENTATION_UNSPECIFIED = -1,
    SCREEN_ORIENTATION_FULL_SENSOR = 10
};

#if PLATFORM_ANDROID

#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"

#define PLATFORM_INDEPENDENT_ORIENTATION_PORTRAIT AndroidOrientations::SCREEN_ORIENTATION_SENSOR_PORTRAIT
#define PLATFORM_INDEPENDENT_ORIENTATION_PORTRAIT_LOCKED AndroidOrientations::SCREEN_ORIENTATION_PORTRAIT
#define PLATFORM_INDEPENDENT_ORIENTATION_ANY_LANDSCAPE AndroidOrientations::SCREEN_ORIENTATION_SENSOR_LANDSCAPE
#define PLATFORM_INDEPENDENT_ORIENTATION_LEFT_LANDSCAPE AndroidOrientations::SCREEN_ORIENTATION_LANDSCAPE
#define PLATFORM_INDEPENDENT_ORIENTATION_RIGHT_LANDSCAPE AndroidOrientations::SCREEN_ORIENTATION_REVERSE_LANDSCAPE
#define PLATFORM_INDEPENDENT_ORIENTATION_UNLOCKED  AndroidOrientations::SCREEN_ORIENTATION_SENSOR

#define APPLY_ORIENTATION(or, name)\
{JNIEnv* JE = FAndroidApplication::GetJavaEnv(); \
jmethodID SetOrientationID = FJavaWrapper::FindMethod(JE, FJavaWrapper::GameActivityClassID, "setRequestedOrientation", "(I)V", false); \
FJavaWrapper::CallVoidMethod(JE, FJavaWrapper::GameActivityThis, SetOrientationID, or);}

#elif PLATFORM_IOS

#include "IOS/IOSAppDelegate.h"

#define PLATFORM_INDEPENDENT_ORIENTATION_PORTRAIT UIInterfaceOrientationPortrait
#define PLATFORM_INDEPENDENT_ORIENTATION_PORTRAIT_LOCKED UIInterfaceOrientationPortrait
#define PLATFORM_INDEPENDENT_ORIENTATION_ANY_LANDSCAPE UIInterfaceOrientationLandscapeLeft
#define PLATFORM_INDEPENDENT_ORIENTATION_LEFT_LANDSCAPE UIInterfaceOrientationLandscapeLeft
#define PLATFORM_INDEPENDENT_ORIENTATION_RIGHT_LANDSCAPE UIInterfaceOrientationLandscapeRight
#define PLATFORM_INDEPENDENT_ORIENTATION_UNLOCKED UIInterfaceOrientationPortrait

#define APPLY_ORIENTATION(or, name)\
{ dispatch_async(dispatch_get_main_queue(),^ { \
    UIInterfaceOrientation currentOrientation = [UIApplication sharedApplication].statusBarOrientation; \
    NSNumber *value = [NSNumber numberWithInt:or]; \
    [[UIDevice currentDevice] setValue:value forKey:@"orientation"]; \
    [IOSAppDelegate GetDelegate].currentSupportedOrientationMask = UIInterfaceOrientationMask##name;\
    [UIViewController attemptRotationToDeviceOrientation]; \
} ); \
}


      
#else

#define PLATFORM_INDEPENDENT_ORIENTATION_PORTRAIT   E_ScreenOrientation::ORIENTATION_PORTRAIT
#define PLATFORM_INDEPENDENT_ORIENTATION_PORTRAIT_LOCKED E_ScreenOrientation::ORIENTATION_PORTRAIT
#define PLATFORM_INDEPENDENT_ORIENTATION_ANY_LANDSCAPE E_ScreenOrientation::ORIENTATION_LANDSCAPE_LEFT_LOCKED
#define PLATFORM_INDEPENDENT_ORIENTATION_LEFT_LANDSCAPE E_ScreenOrientation::ORIENTATION_LANDSCAPE_LEFT_LOCKED
#define PLATFORM_INDEPENDENT_ORIENTATION_RIGHT_LANDSCAPE E_ScreenOrientation::ORIENTATION_LANDSCAPE_LEFT_LOCKED
#define PLATFORM_INDEPENDENT_ORIENTATION_UNLOCKED E_ScreenOrientation::ORIENTATION_PORTRAIT

// Should maintain long side between rotation, but doesn't work
/*
#define APPLY_ORIENTATION(or)\
if (pc) {\
{\
FVector2D newRes = FVector2D( 1, 1 );  \
if (GSystemResolution.ResX > GSystemResolution.ResY) \
{ \
    switch (or) \
    { \
        case E_ScreenOrientation::ORIENTATION_PORTRAIT: \
            newRes.Y = GSystemResolution.ResX; \
            newRes.X = newRes.Y / pcAspectRatio; \
            break;  \
        case E_ScreenOrientation::ORIENTATION_LANDSCAPE_LEFT_LOCKED: \
            newRes.Y = GSystemResolution.ResX / pcAspectRatio; \
            newRes.X = GSystemResolution.ResX; \
            break;  \
    } \
} \
else \
{ \
    switch (or) \
    { \
        case E_ScreenOrientation::ORIENTATION_PORTRAIT: \
            newRes.X = GSystemResolution.ResY / pcAspectRatio; \
            newRes.Y = GSystemResolution.ResY; \
            break;  \
        case E_ScreenOrientation::ORIENTATION_LANDSCAPE_LEFT_LOCKED: \
            newRes.Y = GSystemResolution.ResY / pcAspectRatio; \
            newRes.X = GSystemResolution.ResY; \
            break;  \
    } \
} \
FString consoleCommand = "r.SetRes "; \
consoleCommand.AppendInt(newRes.X); \
consoleCommand += "x";\
consoleCommand.AppendInt(newRes.Y); \
pc->ConsoleCommand(consoleCommand, true);\
}}
*/

#define APPLY_ORIENTATION(or, name)\
if (false && pc) \
{\
    FVector2D newRes = FVector2D( 1, 1 );\
    {\
        switch (or) \
        { \
            case E_ScreenOrientation::ORIENTATION_PORTRAIT: \
                newRes.Y = portraitLongSide; \
                newRes.X = portraitLongSide / pcAspectRatio; \
                break;  \
            case E_ScreenOrientation::ORIENTATION_LANDSCAPE_LEFT_LOCKED: \
                newRes.Y = landscapeLongSide / pcAspectRatio; \
                newRes.X = landscapeLongSide; \
                break;  \
        } \
    } \
    FString consoleCommand = "r.SetRes "; \
    consoleCommand.AppendInt(newRes.X); \
    consoleCommand += "x";\
    consoleCommand.AppendInt(newRes.Y); \
    pc->ConsoleCommand(consoleCommand, true);\
}

#endif


void UOrientationManager::SwitchToOrientation(E_ScreenOrientation orientation, APlayerController* pc)
{
    //return;
    switch (orientation)
    {
        case E_ScreenOrientation::ORIENTATION_UNKNOWN:
            break;
        case E_ScreenOrientation::ORIENTATION_PORTRAIT:
            APPLY_ORIENTATION(PLATFORM_INDEPENDENT_ORIENTATION_PORTRAIT, Portrait)
            break;
        case E_ScreenOrientation::ORIENTATION_PORTRAIT_LOCKED:
            APPLY_ORIENTATION(PLATFORM_INDEPENDENT_ORIENTATION_PORTRAIT_LOCKED, Portrait)
            break;
        case E_ScreenOrientation::ORIENTATION_ANY_LANDSCAPE:
            APPLY_ORIENTATION(PLATFORM_INDEPENDENT_ORIENTATION_ANY_LANDSCAPE, Landscape)
            break;
        case E_ScreenOrientation::ORIENTATION_LANDSCAPE_LEFT_LOCKED:
            APPLY_ORIENTATION(PLATFORM_INDEPENDENT_ORIENTATION_LEFT_LANDSCAPE, LandscapeLeft)
            break;
        case E_ScreenOrientation::ORIENTATION_LANDSCAPE_RIGHT_LOCKED:
            APPLY_ORIENTATION(PLATFORM_INDEPENDENT_ORIENTATION_RIGHT_LANDSCAPE, LandscapeRight)
            break;
        case E_ScreenOrientation::ORIENTATION_UNLOCKED:
            APPLY_ORIENTATION(PLATFORM_INDEPENDENT_ORIENTATION_UNLOCKED, AllButUpsideDown)
            break;
    }
}
