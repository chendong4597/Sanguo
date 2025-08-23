// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Role.h"
#include "Weapon.h"
#include "../Common/UIEvents.h"
#include "config/MonsterCfg.h"
#include "config/NpcTypeCfg.h"
#include "data/role/Monsters.h"
#include "Soldier.generated.h"

class AWeapon;
/**
 * 
 */
UCLASS()
class SANGUO_API ASoldier : public ARole
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void onGeneralsSkillEvent(const GeneralsSkillEvent& evt);
public:
	UFUNCTION(BlueprintCallable, Category = "Soldier")
	void SetHideWeapon(bool bHideWeapon);

	UFUNCTION(BlueprintCallable, Category = "Soldier")
	void SetWeaponId(int nWeaponId);

	UFUNCTION(BlueprintCallable, Category = "Soldier")
	void SetSubWeaponId(int nSubWeaponId);

	UFUNCTION(BlueprintImplementableEvent, Category = "Soldier")
	void BTPlayAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "Soldier")
	bool BTDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Soldier")
	void BTSetHorse(bool bHorse);

	//是否是可攻击的目标
	virtual bool IsCanAttackTarget(ARole* pRole, bool bPassive = false) override;

	void AttachWeaponById(int nWeaponId);
	void AttachSubWeaponById(int nWeaponId);
	AWeapon* LoadWeapon(FString path);
	void RemoveWeapons();

	UFUNCTION(BlueprintCallable, Category = "Soldier")
	int GetWeaponTpy() { return m_nWeaponTpy; }

	UFUNCTION(BlueprintCallable, Category = "Generals")
	bool IsWaitBegin();

	bool IsDefenceing();
	void UnRiderHorse();

	UFUNCTION(BlueprintCallable, Category = "Soldier")
	ARole* GetParentGenerals();

	void InitSkin();
	void ResortAttackTargets();

public:
	virtual bool PlayAttack() override;
	virtual void AttackEffect() override;

	virtual void Death() override;
	virtual bool CanMove() override;

public:
	virtual int GetObjectId() override;
	virtual bool IsDeath() override;
	virtual int GetHp() override;
	virtual int GetMaxHp() override;
	virtual int GetRoleCtrlStatus() override;
	virtual bool IsShowCtrlStatus() override;
	virtual void ClearData() override;
	virtual void ResetAttr() override;

public:
	NS::I64 GetProtectorUuid() const { return m_ProtectorUuid; }
	void SetProtectorUuid(NS::I64 ProtectorUuid) { m_ProtectorUuid = ProtectorUuid; }

	NS::int32 GetMapObjectId() const { return m_MapObjectId; }
	void SetMapObjectId(NS::int32 MapObjectId) { m_MapObjectId = MapObjectId; }
private:
	NS::I64 m_ProtectorUuid{ 0 };
	NS::int32 m_MapObjectId{ 0 };
	NS::int32 m_nMonsterId{ 0 };
private:
	AWeapon* m_AWeapon{};
	AWeapon* m_BWeapon{};
	int m_nWeaponId{};
	int m_nSubWeaponId{};
	int m_nWeaponTpy{};
	int m_nSubWeaponTpy{};
	float m_cavInterPlay{ 0.f };
	bool m_bHideWeapon{ false };
	int m_nGenelTpy{ NpcTypeCfg::NpcSubType_None };

public:
	void SetMonsterId(int mapObjId, int64 nInsId, int nMonsterId);
public:
	std::shared_ptr<Monster> m_pMonsterData;
};



class ASoldierMgr
{
public:
	using MapSolider = std::map<NS::I64, ASoldier*>;

	bool initialize();
	void terminate();
	bool reset();
	void update(float dt);
	void PreReset();

public:
	ASoldierMgr();
	~ASoldierMgr() {};
	static ASoldierMgr& getInstance();

	void CreateSoldier(int nMapObjectId, NS::I64 nProtectId, int mMonsterId, FTransform trans);

	void RemoveSoldier(int nMapObjectId);
	ASoldier* GetSolider(int nObjId);

	const MapSolider& GetSoliders() { return m_mapSoldier; }

private:
	
	MapSolider m_mapSoldier;
};
