// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Role.h"
#include "Animation/BlendSpace1D.h"
#include "../Common/UIEvents.h"
#include "Animation/AnimMontage.h"
#include <map>
#include "config/MonsterCfg.h"
#include "data/role/Monsters.h"
#include "Monster.generated.h"




UENUM(BlueprintType)
enum class ControllerState :uint8 //设置uint8类型
{
	E_ATTACK_RAND UMETA(DisplayName = "Attack"),
	E_S_ATTACK1 UMETA(DisplayName = "MagicAttack1"),
	E_S_ATTACK2 UMETA(DisplayName = "MagicAttack2"),
	E_S_ATTACK3 UMETA(DisplayName = "MagicAttack3"),
	E_S_ATTACK4 UMETA(DisplayName = "MagicAttack4"),
	E_S_ATTACK5 UMETA(DisplayName = "MagicAttack5"),
	E_HURT UMETA(DisplayName = "Hurt"),
	E_DEAD UMETA(DisplayName = "Dead"),
};
/**
 * 
 */
UCLASS()
class SANGUO_API AMonster : public ARole
{
	GENERATED_BODY()
	//DECLARE_EVENT_HOST();

protected:
	void onSightEvent(const SightEvent& evt);

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "MONSTER")
	void Sight(int sightValue);

	UFUNCTION(BlueprintCallable, Category = "MONSTER")
	UAnimMontage* LoadMonsterAniMontage(ControllerState nBehivor);

	UFUNCTION(BlueprintImplementableEvent, Category = "MONSTER")
	bool IsFocusPlayer();

	UFUNCTION(BlueprintImplementableEvent, Category = "MONSTER")
	void DeathEnd();

	UPROPERTY(BlueprintReadWrite, Category = "MONSTER")
	bool m_bFocusPlayer;

	UFUNCTION(BlueprintCallable, Category = "MONSTER")
	void SetMonsterId(int mapObjId, int nInsId, int nMonsterId);

	int GetMonsterId() { return m_nMonsterId; }

	UFUNCTION(BlueprintCallable, Category = "MONSTER")
	int GetWalkSpeed();

	void CalSkillCondition();

	//测试是否可以攻击目标 直接攻击
	UFUNCTION(BlueprintCallable, Category = "MONSTER")
	bool TestCanAttack();

	//设置快速返还出生点
	UFUNCTION(BlueprintCallable, Category = "MONSTER")
	FVector GetBornPos() { return m_vecBornPos; }

	UFUNCTION(BlueprintCallable, Category = "MONSTER")
	bool IsWaitBegin();

	UFUNCTION(BlueprintCallable, Category = "MONSTER")
	void SetReturnBorn(bool bRetBorn);

	UFUNCTION(BlueprintCallable, Category = "MONSTER")
	float GetNormalAtkDistance();

	void ResetWalkSpeed();
	void NormalAttack();
	void MagicAttack();
	void MagicAttackEnd();

	virtual bool HitTest() override;

	void InitSkin();
	void InitMeshScale();
	void UnRiderHorse();

	void GetAight(float& fAtkSight, float& fLoseSight, float& fAtkAngle);

	virtual std::string GetBattleSound(int nSoundTpy) override;
public:
	virtual int GetObjectId() override;
	virtual bool IsDeath() override;
	virtual int GetHp() override;
	virtual int GetMaxHp() override;
	virtual int GetRoleCtrlStatus() override;
	virtual bool IsShowCtrlStatus() override;
	virtual void ClearData() override;
public:
	virtual void PlayHit() override;
	virtual bool PlayAttack() override;
	virtual bool IsCanAttackTarget(ARole* pRole, bool bPassive = false) override;
	virtual bool IsCanBeAttackTarget(ARole* pRole) override;
	virtual void StartAI() override;
	virtual void Death() override;
	virtual void PlayDeath() override;
	virtual bool CanMove() override;
	virtual bool CanAttack() override;
	virtual bool CanSkillAttack() override;
	virtual bool CanActiveAttack() override;
	virtual bool IsForbidAttack() override;
	virtual bool IsOnlyBeNormalAtk() override;

public:
	virtual void DestroyRole() override;
	virtual void ResetAttr() override;
	virtual void ResetSpeed() override;
	virtual void ResetAtkSpeed() override;
	virtual void StateHome() override;
	void HomeEnd();
	void DalayDeathDestroy();

public:
	void onHandlerBattleEvent(const HandlerBattleEvent& evt);

private:
	int m_nIdleSpeed{};
	int m_nIAlertSpeed{};
	FVector m_vecBornPos{};  //出生点
	int m_nAttackDis{};      //物理攻击距离。技能攻击另外计算
	bool    m_bReturnBornPos{};  //是否在返回聚焦点位置中
	
	int m_nMapObjId{};
	int m_nInsId{};
	int m_nMonsterId{};
	bool m_bStateHome{ false };		//怪物失去目标后，快速返回到营地，5秒计算

	//
	bool m_bNormalAttack{ false };
	int m_nPrepareSkillId{ 0 };
	int m_nSkillAniIdx{ 0 };

public:
	std::shared_ptr<Monster> m_pMonsterData;
};


class AMonsterMgr// : public bzbee::MsgHandler
{
	//DECLARE_EVENT_HOST();
public:
	using MapAIPlayer = std::map<int, AMonster*>;
public:
	bool initialize();
	void terminate();
	void PreReset();
	bool reset();
	void update(float dt);

	void CreateMonster(int nMapId, int nInsId, int mMonsterId , FTransform trans);
	void RemoveMonster(int nMapId);
	AMonster* GetMonster(int nObjId);

	const MapAIPlayer& GetMonsters() { return m_monsters; }

public:
	AMonsterMgr();
	~AMonsterMgr() {};
	static AMonsterMgr& getInstance();

private:
	MapAIPlayer m_monsters;
};
