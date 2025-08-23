// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelBase.h"
#include "../Player/Hero.h"
#include "../Player/IPlayer.h"
#include "../Player/Monster.h"
#include "../Player/Generals.h"
#include "../Player/Soldier.h"

#include "InstanceLevel.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API AInstanceLevel : public ALevelBase
{
	GENERATED_BODY()
public:
	//virtual void SynchronizeProperties() override;
public:
	//场景类型
	UFUNCTION(BlueprintCallable, Category = "Level")
	void ShowTroopView();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void onMsgDungeonBossChest_MS2C(const MsgDungeonBossChest_MS2C& msg);
	void onMsgDungeonBossOpenChestRs_MS2C(const MsgDungeonBossOpenChestRs_MS2C& msg);
	void onMsgDungeonNextMob_S2C(const MsgDungeonNextMob_S2C& msg);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTroopInfo(int nTroopId, const std::map<int, int64> info);

	void ResetHeroPos();
	FTransform GetTransByTroop(int64 uuid);

	FTransform GetTransByCovory(int64 uuid);

	FTransform GetMonsterTransFlag(int nId);
	FTransform GetMonsterTransNpcFlag(int64 uuid);
	FTransform GetMonsterTransConvoy(int64 uuid);

	bool IsStartBattle() { return m_bStartBattle; }
	void SetStartBattle();

	bool IsEndBattle() { return m_bEndBattle; }
	void SetEndBattle();

	void DestroyDoorsAndTowers();

	UFUNCTION()
	void InitAllTargets();

	void InitGeneralsTargets(AGenerals* pGen);
	void InitSoliderTargets(ASoldier* pSolider);

	void InitMonsterTargets(AMonster* pMonster);

	//测试显示怪物
	void GetPlayerStartTransform(FTransform& OutTransform);
	void TestInitMobs(int nMonsterId, int insId, FTransform mobTrans);

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "")
	void ExportInstanceMobs();

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Content")
	bool IsExportMobs;

private:
	int m_nTroopsId{ 0 };
	std::map<int, int64> m_mapTroopsInfo;
	FTransform m_heroTrans;
	bool m_bStartBattle{ false };
	bool m_bEndBattle{ false };
	bool m_bBuffStart{ false }; //全速开战

public:
	void SetCavTime(int64 n64Tm) { m_nLessCavTime = n64Tm; }
	int64 GetCavTime() { return m_nLessCavTime; }

	void SetArcTime(int64 n64Tm) { m_nLessArcTime = n64Tm; }
	int64 GetArcTime() { return m_nLessArcTime; }

	void SetBlaTime(int64 n64Tm) { m_nLessBlaTime = n64Tm; }
	int64 GetBlaTime() { return m_nLessBlaTime; }

	void SetMagTime(int64 n64Tm) { m_nLessMagTime = n64Tm; }
	int64 GetMagTime() { return m_nLessMagTime; }

	void ResetInfDefence() { m_bInfDefence = !m_bInfDefence; }
	bool IsInfDefence() { return m_bInfDefence; }

	void SetBuffStart() { m_bBuffStart = true; }
private:
	int64 m_nLessCavTime{ 0 };
	int64  m_nLessArcTime{ 0 };
	int64  m_nLessBlaTime{ 0 };
	int64  m_nLessMagTime{ 0 };
	bool m_bInfDefence{ false };

public:
	int GetFocusBossId() { return m_nFocusBossId; }
	void SetFocusBossId(int nObjId) { m_nFocusBossId = nObjId; }
private:
	int	 m_nFocusBossId{ 0 };
public:
	UPROPERTY(EditAnywhere, Category = "")
	FTransform m_convoyTrans;
};
