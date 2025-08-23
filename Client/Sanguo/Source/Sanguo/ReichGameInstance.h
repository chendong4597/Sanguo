// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "GameModule/GameAppModule.h"
#include "TimerManager.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Common/RootEventObject.h"
#include "Scene/LevelBase.h"
//#include "WebBrowserWidget/Public/WebBrowser.h"
#include "ReichGameInstance.generated.h"

#if PLATFORM_ANDROID
#define __ANDROID__ 1
#endif





/**
 * 
 */
UCLASS()
class SANGUO_API UReichGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	static FString TransBPAssertPath(std::string strPath);
	static FString TransAniMotoAssertPath(std::string strPath);
	static FString TransSkeletalMeshAssertPath(std::string strPath);
	static FString TransAnimBluePrintAssertPath(std::string strPath);
	static FString TransClassAssertPath(std::string strPath);
	static FString TransTexturePath(std::string strPath);
	static bool IsPointInsidePolygon(const FVector2D& Point, const TArray<FVector2D>& Polygon);

	static bool IsEditorMode();

	static FVector TransVector(float x , float y , float z);

	//UFUNCTION(BlueprintImplementableEvent, Category = "Triggerable Door|Door Switch")
	//void OpenDoor();
	//UFUNCTION(BlueprintImplementableEvent, Category = "Triggerable Door|Door Switch")
	//void CloseDoor();
public:
	UFUNCTION(BlueprintCallable, Category = "REICH_INS")
	FString GetWeaponDemoPath(int WeaponTpy);

	UFUNCTION(BlueprintCallable, Category = "REICH_INS")
	void PrepareLoadMap();

	static URootEventObject* getRootObject();

	FString GetAndroidFileBasePath();
	FString GetInteralFilePath();

	void SetSoundMusicVolume(float fVolume);
	void SetSoundEffectVolume(float fVolume);

	void SetCurrentLevel(ALevelBase* pLevel) { m_pCurLevel = pLevel; }
	ALevelBase* GetCurrentLevel() { return m_pCurLevel; }

	void InitUESDK();
	void InitAdditionUE();
	void PostTapAddiLoginSuc();
//#if PLATFORM_IOS
	//void HandleTwoParamEvent(AntiAddictionUE::ResultHandlerCode code, const FString& Message);
//#endif
protected:
	/** Called when the game instance is started either normally or through PIE. */
	virtual void Init() override;
	virtual void OnStart() override;
	void update();
	virtual void Shutdown() override;

	void InitGameExtern();
private:
	FTimerHandle m_timerHandler;
	
	ALevelBase* m_pCurLevel{nullptr};
};
