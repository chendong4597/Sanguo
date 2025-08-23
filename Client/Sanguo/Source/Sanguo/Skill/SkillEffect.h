// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"

#include "Player/Role.h"
#include <map>
#include "../Common/UIEvents.h"
#include "config/SkillTypeCfg.h"
#include "config/SkillCfg.h"

#include "SkillEffect.generated.h"

UCLASS()
class SANGUO_API ASkillEffect : public AActor
{
	GENERATED_BODY()
	//DECLARE_EVENT_HOST();
public:	
	// Sets default values for this actor's properties
	ASkillEffect();

	virtual void StartSkill() {}

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	virtual void SkillDestroy();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void onInsEndEvent(const InsEndEvent& evt);
public:	

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	virtual void RegisterOverlayEvents();

	UFUNCTION()
	virtual void OnOverlayBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnOverlayEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit);

	virtual void BeginCollision(AActor* OtherActor);
	virtual void EndCollision(AActor* OtherActor);


	UFUNCTION(BlueprintImplementableEvent, Category = "SKILL EFFECT")
	void TimeLineEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "SKILL EFFECT")
	void InitSkillEvent();
public:
	void SetInfo(int nSkillId , ARole* aAttackRole , ARole* m_rTargetRole , FVector targetPos , FVector rota , int nSkillIdx = 0);
	void SetParentSkill(ASkillEffect* pSkillEffect);

	int GetSkillId() { return m_nSkillId; }
	int GetParentSkillId() { return m_nParentSkillId; }

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	ARole* GetAttackRole() { return m_rAttackRole; }

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	void SetBPAttackRole(ARole* pRole) { m_rAttackRole = pRole; }

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	ARole* GetTargetRole() { return m_rTargetRole; }

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	FVector GetInitTargetPos();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	FVector GetInitRotaArrowPos() { return m_rotaTarget; }

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	int GetSkillIdx() { return m_nSkillIdx; }

	//技能怪攻击
	bool IsOriginSource() { return m_bOriginSKill; }

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	ASkillEffect* GetParentSkillEffect() { return m_pParentEffect; }
	FTransform GetRoleTransByName(const char* pszSock);

	bool IsSkillInvalid() { return m_bInvalid; } //无效的技能状态
	void SetSkillInvalid(bool bInvalid) { m_bInvalid = bInvalid; }

	void PlaySkillSound();

	//计算伤害
	void Damage(int nSkillId, int nDamageTpy, int nDamageParam1, int nDamageParam2 = 0, int nDamageParam3 = 0, int nDamageParam4 = 0);
	virtual void StartDamage();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	virtual void BpStartDamage();

private:
	ARole* m_rAttackRole{};
	ARole* m_rTargetRole{};
	FVector m_vecTargetPos{};
	FVector m_rotaTarget{};
	int m_nSkillId{};
	int m_nParentSkillId{};
	int m_nSkillIdx{};  //一个技能有多个分支时，第几个分支
	bool m_bOriginSKill{};  //技能是否源于技能怪
	bool m_bInvalid{false};		//技能是否已失效
	float m_fDieTm{ 0.f };	//技能死亡消失时间

private:
	ASkillEffect* m_pParentEffect{nullptr};     //父技能
	bool m_bSubSkillEffect{};           //是否是子技能
};

class SkillMgr// : public bzbee::MsgHandler
{
	//DECLARE_EVENT_HOST();
public:
	bool initialize();
	void terminate();
	bool reset();
	void update(float dt);

public:
	SkillMgr();
	~SkillMgr() {};
	static SkillMgr& getInstance();

	void CreateSkill(int nSkillId , ARole* attackRole , ARole* targetRole , FVector vec = FVector::ZeroVector , FVector rator = FVector::ZeroVector);
	void CreateFrontSkill(int nSkillId , ARole* attackRole, ARole* targetRole, FVector vec = FVector::ZeroVector, FVector rator = FVector::ZeroVector);
	void CreateSubSkill(int nSkillId, ARole* attackRole, ARole* targetRole , ASkillEffect* pParentSkill, FVector vec = FVector::ZeroVector, FVector rator = FVector::ZeroVector, int nSkillIdx = -1);
private:
	using MapSkillEffect = std::map<int, ASkillEffect*>;
	MapSkillEffect m_mapSkillEffects;
};
