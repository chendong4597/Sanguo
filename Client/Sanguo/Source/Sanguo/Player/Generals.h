// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Role.h"
#include "Weapon.h"
#include "data/role/Npc.h"
#include "data/NpcProtectorData.h"
#include "config/NpcTypeCfg.h"
#include "../Common/UIEvents.h"
#include "../Controls/MyButton.h"
#include "Generals.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API AGenerals : public ARole
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	void onGeneralsSkillEvent(const GeneralsSkillEvent& evt);
	void onHandlerNpcEvent(const HandlerNpcEvent& msg);
	void onHandlerProtectorEvent(const HandlerProtectorEvent& msg);
	void onHandlerNpcAliveEvent(const HandlerNpcAliveEvent& msg);
public:
	UFUNCTION(BlueprintCallable, Category = "Generals")
	void SetWeaponId(int nWeaponId);

	UFUNCTION(BlueprintCallable, Category = "Generals")
	int GetWeaponTpy() { return m_nWeaponTpy; }

	UFUNCTION(BlueprintCallable, Category = "Generals")
	void SetSubWeaponId(int nSubWeaponId);

	UFUNCTION(BlueprintImplementableEvent, Category = "Generals")
	void EventPlayAttack();

	UFUNCTION(BlueprintCallable, Category = "Generals")
	bool IsTooFarToPlayer();

	UFUNCTION(BlueprintCallable, Category = "Generals")
	bool IsRest();

	UFUNCTION(BlueprintCallable, Category = "Generals")
	FVector GetMainCityBattlePos();

	UFUNCTION(BlueprintCallable, Category = "Generals")
	bool IsWaitBegin();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Generals")
	void EventDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Generals")
	void BTSetHorse(bool bHorse);

	virtual float GetNormalAttackInterTm() override;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Generals")
	void EventAttachHorse();

	//是否是可攻击的目标
	virtual bool IsCanAttackTarget(ARole* pRole, bool bPassive = false) override;

	void AttachWeaponById(int nWeaponId);
	void AttachSubWeaponById(int nWeaponId);
	AWeapon* LoadWeapon(FString path);
	void RemoveWeapons();
	virtual bool PlayAttack() override;
	virtual void AttackEffect() override;

	virtual void Death() override;
	virtual void Revive() override;
	virtual bool CanMove() override;

	void InitSkin();
	virtual void SetMaterialScalerParamer(FName name, float fValue) override;

	void RiderHorse();
	void UnRiderHorse();

	bool IsDefenceing();		//是否是防御状态，只对步兵有用
	void ResortAttackTargets();
public:
	virtual int GetObjectId() override;
	virtual bool IsDeath() override;
	virtual int GetHp() override;
	virtual int GetMaxHp() override;
	virtual int GetRoleCtrlStatus() override;

	virtual void ResetAttr() override;
	virtual void ResetSpeed() override;
	virtual void ResetAtkSpeed() override;
public:
	UPROPERTY(BlueprintReadOnly)
	FTransform m_oriTrans;

public:
	NS::I64 GetProtectorUuid() const { return m_ProtectorUuid; }
	void SetProtectorUuid(NS::I64 ProtectorUuid);

	NS::int32 GetMapObjectId() const { return m_MapObjectId; }
	void SetMapObjectId(NS::int32 MapObjectId);

public:
	std::shared_ptr<Npc> m_npcData{};
	std::shared_ptr<NpcProtectorData> m_protectData{};

private:
	NS::I64 m_ProtectorUuid{ 0 };
	NS::int32 m_MapObjectId{ 0 };

public:
	void SetFollowPlayer(bool bFollow) { m_bFollowPlayer = bFollow; }

	UFUNCTION(BlueprintCallable, Category = "Generals")
	bool IsFollowPlayer() { return m_bFollowPlayer; }

	virtual std::string GetBattleSound(int nSoundTpy) override;
private:
	bool m_bFollowPlayer{ false };

public:
	void AddSoliderNum() { m_nCurSoliderNum++; }
	int GetSoliderNum() { return m_nCurSoliderNum; }
private:
	int  m_nCurSoliderNum{ 0 };

private:
	AWeapon* m_AWeapon{};
	AWeapon* m_BWeapon{};
	int m_nWeaponId{};
	int m_nWeaponTpy{};
	int m_nSubWeaponId{};
	int m_nSubWeaponTpy{};
	float m_cavInterPlay{0.f};

	int m_nGenelTpy{ NpcTypeCfg::NpcSubType_None };
	
	bool m_bBattleing{ false };
	float m_fLifeInterTm{ 0.f }; //血量回复
	float m_fFindMonsterInterTm{ 0.f }; //AI有时找不到怪，定时自己找
};


class AGeneralsMgr
{
public:
	using MapGenerals = std::map<NS::I64, AGenerals*>;

	bool initialize();
	void terminate();
	void PreReset();
	bool reset();
	void update(float dt);

public:
	AGeneralsMgr();
	~AGeneralsMgr() {};
	static AGeneralsMgr& getInstance();

	void CreateGenels(int nMapObjectId, int64 nProtectId, int mMonsterId);

	void DestroyGenels(int nNpcId);
	void DestroyGenelIns(int nObjId);

	const MapGenerals& GetMapGenerals() { return m_mapGenerals; }

	AGenerals* GetGeneral(int nObjId);
	AGenerals* GetGeneralByUuid(int64 uuid);

	bool GetNpcByType(int nGenTpy, FVector& vec, NS::I64& proUuid);
private:
	
	MapGenerals m_mapGenerals;
};