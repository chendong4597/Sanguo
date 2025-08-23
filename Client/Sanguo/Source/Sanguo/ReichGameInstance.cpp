// Fill out your copyright notice in the Description page of Project Settings.


#include "ReichGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Misc/Paths.h"
#include "Common/UIEvents.h"
#include "config/WeaponTypeCfg.h"
#include "config/WeaponAniEffectCfg.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Misc/FileHelper.h"
#include "UI/UIManager.h"
#include "MySound.h"
#include "Common/RootEventObject.h"

#include "Player/IPlayer.h"
#include "Player/Hero.h"
#include "UI/UIManager.h"
#include "GameAppExtern.h"
#include "UI/UIMsgBoxToast.h"


#if PLATFORM_ANDROID
#include "JNI/java_com_wskj_thirdreich.h"
#elif PLATFORM_IOS
#include "Ios/IosInterface.h"
#include "TapBootstrap.h"
#include "TapCommon.h"
#include "TapDB.h"
#include "TapLogin.h"
#include "AntiAddictionUE.h"
#endif






#include "HAL/PlatformProcess.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"



URootEventObject* m_rootObject = nullptr;

static bool s_bGamePlaying = false;
bool g_IosHasAgreement = false;

FString UReichGameInstance::GetAndroidFileBasePath()
{
#if PLATFORM_ANDROID
	extern FString GExternalFilePath;
	return GExternalFilePath + FString("/");
#elif PLATFORM_IOS
	FString Path = FPlatformProcess::UserSettingsDir();
	return Path;
#else
	return FString("");
#endif
}

FString UReichGameInstance::GetInteralFilePath()
{
#if PLATFORM_ANDROID || PLATFORM_IOS
	return FPaths::ProjectContentDir();
#else
	FString TheReadPath = FString(FPaths::ProjectSavedDir());
	return TheReadPath;
#endif
}

//#if PLATFORM_ANDROID
//#include "Android/AndroidApplication.h"
//#include "Android/AndroidJNI.h"
//#include "Android/AndroidPlatform.h"
//
//void KeepScreenOn(bool bEnable)
//{
//	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
//	{
//		jclass ActivityClass = FAndroidApplication::FindJavaClass("com/epicgames/ue4/GameActivity");
//		jmethodID MethodID = Env->GetMethodID(ActivityClass, "setKeepScreenOn", "(Z)V");
//		Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), MethodID, bEnable);
//	}
//}
//#endif



void UReichGameInstance::Init()
{
	Super::Init();


	UE_LOG(LogOutputDevice, Log, TEXT("UReichGameInstance::Init 2"));

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString TempFileName = FPaths::ProjectSavedDir() + "privicy.xml";
	if (PlatformFile.FileExists(*TempFileName)) {
		g_IosHasAgreement = true;
	}


#if PLATFORM_IOS
	if (!g_IosHasAgreement) {
		ShowPrivacyView(true, false);
	}
	else {
		//InitTapSdk();
		InitUESDK();
	}
#endif
	InitGameExtern();
}

void UReichGameInstance::InitUESDK()
{
#if PLATFORM_IOS
	FTUConfig Config;
	Config.ClientID = "e4hzofex4asmrb61v5"; // 必须，开发者中心对应 Client ID
	Config.ClientToken = "FImGsoQ1RHdnbvflz1kAflVWCiDHQwhXWzMA3d5u"; // 必须，开发者中心对应 Client Token
	Config.ServerURL = ""; // 必须，开发者中心 > 你的游戏 > 游戏服务 > 基本信息 > 域名配置 > API
	Config.RegionType = ERegionType::CN; // ERegionType::CN：中国大陆，ERegionType::Global：其他国家或地区
	FTapBootstrap::Init(Config);
	InitAdditionUE();
#endif
}

void UReichGameInstance::InitAdditionUE()
{
#if PLATFORM_IOS
	FAAUConfig Config;
	Config.ClientID = "e4hzofex4asmrb61v5"; // TapTap 开发者中心对应 Client ID
	Config.ShowSwitchAccount = false;
	Config.UseAgeRange = true; //是否使用年龄段信息
	AntiAddictionUE::Init(Config);
	// 绑定 开发者实现的自定义防沉迷回调对象 AntiAddictionUE::OnCallBack 
	AntiAddictionUE::OnCallBack.BindLambda([this](AntiAddictionUE::ResultHandlerCode code, const FString& Message) {
		if (code == AntiAddictionUE::LoginSuccess) {
			PostTapAddiLoginSuc();
		}
		else {
			MsgBoxToastMgr::getInstance().ShowText(__LC("UI_LOGIN_ADDI_FAIL_ED"));
		}
		
		UE_LOG(LogOutputDevice, Log, TEXT("code: %d, Message: %s"), code, *Message);
	});
#endif
}

void UReichGameInstance::PostTapAddiLoginSuc()
{
	//extern std::string g_strTapUnionid;
	//extern std::string g_strTapOpenid;
	//UITapLoginEvent evt;
	//evt.m_type = UITapLoginEvent::EventType::EventType_Login;
	//evt.strUuid = g_strTapOpenid;
	//evt.strToken = g_strTapUnionid;
	//UI_DISPATCH_MYEVENT(UITapLoginEvent, evt);
}

void UReichGameInstance::InitGameExtern()
{
	if (!m_rootObject) {
		m_rootObject = NewObject<URootEventObject>();

		m_rootObject->AddToRoot();
		m_rootObject->InitializeEvents();
		GameAppExtern::setDataHandler(m_rootObject);
		UE_LOG(LogOutputDevice, Log, TEXT("UReichGameInstance::Init URootEventObject"));
#ifdef PLATFORM_WINDOWS
		FString ThePath = FString(FPaths::ProjectContentDir());
#else
		FString ThePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
		//KeepScreenOn(true);
#endif
		FString TheReadPath = GetInteralFilePath();
		FString TheBasePath = GetAndroidFileBasePath();
		std::string cstr(TCHAR_TO_UTF8(*ThePath));
		std::string cstr1(TCHAR_TO_UTF8(*TheReadPath));
		std::string cstrBase(TCHAR_TO_UTF8(*TheBasePath));
		GameAppExtern::initialize(cstr.c_str(), cstr1.c_str(), cstrBase.c_str());
		UE_LOG(LogOutputDevice, Log, TEXT("UReichGameInstance ThePath = %s TheReadPath = %s TheBasePath = %s"), *ThePath, *TheReadPath, *TheBasePath);
	}
	UIManager::getInstance().initialize();
	UE_LOG(LogOutputDevice, Log, TEXT("UReichGameInstance is intinal"));

#if PLATFORM_ANDROID
	registerNative();
#endif
}

//蓝图加载路径继承C++最后一定要加_C
FString UReichGameInstance::TransBPAssertPath(std::string strPath)
{
	std::string str = "Blueprint'";
	str += strPath;// 
	str += "'";
	return FString(str.c_str());
}

FString UReichGameInstance::TransClassAssertPath(std::string strPath)
{
	std::string str = "Class'";
	str += strPath;// 
	str += "_C'";

	return FString(str.c_str());
}

FString UReichGameInstance::TransAniMotoAssertPath(std::string strPath)
{
	std::string str = "AnimMontage'";
	str += strPath;// 
	str += "'";
	return FString(str.c_str());
}

FString UReichGameInstance::TransSkeletalMeshAssertPath(std::string strPath)
{
	std::string str = "SkeletalMesh'";
	str += strPath;// 
	str += "'";
	return FString(str.c_str());
}

//C++调用动画蓝图加载路径最后一定要加_C
FString UReichGameInstance::TransAnimBluePrintAssertPath(std::string strPath)
{
	std::string str = "AnimBlueprint'";
	str += strPath;// 
	str += "_C'";
	return FString(str.c_str());
}

FString UReichGameInstance::TransTexturePath(std::string strPath)
{
	std::string str = "Texture2D'";
	str += strPath;// 
	str += "'";
	return FString(str.c_str());
}

FString UReichGameInstance::GetWeaponDemoPath(int WeaponTpy)
{
	auto&& pWeaponTpy = WeaponTypeCfg::getInstance().GetWeaponType(WeaponTpy);
	if (pWeaponTpy == nullptr)
	{
		return FString("");
	}
	return TransBPAssertPath(pWeaponTpy->s_DemoPath);
}

FVector UReichGameInstance::TransVector(float x, float y, float z)
{
	FVector fVec;
	fVec.X = x;
	fVec.Y = y;
	fVec.Z = z;
	return fVec;
}


void UReichGameInstance::OnStart()
{
	s_bGamePlaying = true;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(m_timerHandler, this, &UReichGameInstance::update , 1.0/30.f, true, 1.0f);
	}
	GameAppExtern::Start();

#if PLATFORM_IOS
	dispatch_async(dispatch_get_main_queue(), ^ {
		[[UIApplication sharedApplication]setIdleTimerDisabled:YES];
		});
#endif
}

void UReichGameInstance::update()
{
	//UE_LOG(LogTemp, Log, TEXT("111111111111111"));
	GameAppExtern::update();
	UIManager::getInstance().update(0.65f);
}

void UReichGameInstance::Shutdown()
{
	GetWorld()->GetTimerManager().ClearTimer(m_timerHandler);
	AIPlayerMgr::getInstance().SetHero(nullptr);
	UIManager::getInstance().terminate();
	GameAppExtern::ShutDown();
	s_bGamePlaying = false;

#if PLATFORM_IOS
	dispatch_async(dispatch_get_main_queue(), ^ {
		 [[UIApplication sharedApplication]setIdleTimerDisabled:NO];
		});
#endif
}

void UReichGameInstance::PrepareLoadMap()
{
}

URootEventObject* UReichGameInstance::getRootObject()
{
	return m_rootObject;
}

void UReichGameInstance::SetSoundMusicVolume(float fVolume)
{
	TArray<AActor*> arrActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMySound::StaticClass(), arrActors);
	for (auto&& pSoundAct : arrActors) {
		if (!pSoundAct) {
			continue;
		}
		auto&& pSound = Cast<AMySound>(pSoundAct);
		if (pSound) {
			pSound->SetVolume(fVolume);
		}
	}
}


bool UReichGameInstance::IsPointInsidePolygon(const FVector2D& Point, const TArray<FVector2D>& Polygon)
{
	int count = 0;
	for (int i = 0; i < Polygon.Num(); i++)
	{
		FVector2D edgeStart = Polygon[i];
		FVector2D edgeEnd = Polygon[(i + 1) % Polygon.Num()];

		if (edgeStart.Y > edgeEnd.Y)
		{
			std::swap(edgeStart, edgeEnd);
		}

		if ((Point.Y >= edgeStart.Y && Point.Y < edgeEnd.Y) ||
			(edgeStart.Y == edgeEnd.Y && Point.Y == edgeStart.Y))
		{
			float x = (Point.Y - edgeStart.Y) * (edgeEnd.X - edgeStart.X) / (edgeEnd.Y - edgeStart.Y) + edgeStart.X;
			if (x > Point.X)
			{
				count++;
			}
		}
	}

	return count % 2 == 1;
}

void UReichGameInstance::SetSoundEffectVolume(float fVolume)
{
	UObject* USounMix = StaticLoadObject(USoundMix::StaticClass(), NULL, TEXT("SoundMix'/Game/Sound/class/Effect_SoundMix.Effect_SoundMix'"));
	UObject* USounCla = StaticLoadObject(USoundClass::StaticClass(), NULL, TEXT("SoundClass'/Game/Sound/class/Effect_SoundClass.Effect_SoundClass'"));
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), Cast<USoundMix>(USounMix), Cast<USoundClass>(USounCla), fVolume);
}

bool UReichGameInstance::IsEditorMode()
{
#if PLATFORM_ANDROID || PLATFORM_IOS
	return false;
#else
	return !s_bGamePlaying;
#endif

}


