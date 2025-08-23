// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "msg/MsgMapServer.pb.h"
#include "GameAppExtern.h"
#include "DataHandler.h"
#include "../Common/UIEvents.h"
//#include "../Player/CityCrystal.h"

#include "LevelBase.generated.h"

UENUM(BlueprintType)
enum class LevelType :uint8 //设置uint8类型
{
	LevelType_MainCity UMETA(DisplayName = "level maincity"),
	LevelType_World UMETA(DisplayName = "level world"),
	LevelType_Dungeon UMETA(DisplayName = "level dungeon"),
	LevelType_Login UMETA(DisplayName = "level login"),
	LevelType_PveOther UMETA(DisplayName = "level pve"),
	LevelType_CreateRole UMETA(DisplayName = "create role"),
};

/**
 * 
 */
UCLASS()
class SANGUO_API ALevelBase : public ALevelScriptActor
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PreUnloadData();
	virtual void PreUnload();

	void ShowLoadingUI(bool bUnload);

	virtual void TranslateMap(FName LevelName);
	virtual void TransLoadState(int nLoadState);
public:
	//场景类型
	UFUNCTION(BlueprintCallable, Category = "Level")
	void SetLevelTpy(LevelType levTpy) { m_levelTpy = levTpy; }

	UFUNCTION(BlueprintCallable, Category = "Level")
	LevelType GetLevelTpy() { return m_levelTpy; }

private:
	LevelType        m_levelTpy{ LevelType::LevelType_Login };
	FName			 m_nxtLevelName;
public:
	void onMsgJoinMapG2C(const MsgJoinMapG2C& msg);
	void onMsgTeamDungeonReadyToGo_S2C(const MsgTeamDungeonReadyToGo_S2C& msg);

	void onMonsterEvent(const HandlerMonsterEvent& evt);
	void onTransterEvent(const HandlerTransterEvent& evt);
	void onHandlerNpcEvent(const HandlerNpcEvent& evt);

	void onUILoadintEvent(const UILoadintEvent& evt);
private:
	//ACityCrystal* m_aCrystal{ nullptr }; //水晶
};
