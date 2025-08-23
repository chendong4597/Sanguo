// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SkillEffectMagicRange.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ASkillEffectMagicRange : public ASkillEffect
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	void SetMaxTm(float fAllTm) { m_allTm = fAllTm; }

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //nBPDamgeRange 脚本加，0的话用 m_nDamageRange1
	void BPDamageIndex(int nIdx, int nBPDamgeRange);

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //nBPDamgeRange 脚本加，0的话用 m_nDamageRange1
	void BPDamageDistanceRange(int nDis);

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //刚开始的时候位置不对，延时下伤害
	void BPDamageDelay();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //刚开始的时候位置不对，延时下伤害
	void BPDamageRange2Delay();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	int GetDamageSkillId();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	int GetDamageRange();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	int GetDamageRange2();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	ARole* GetDamageAttacker();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //双龙卷风
	void BPDamageCircle(int Dis);

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //烟花炮
	void BPDamageSector(int R);

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //五个龙卷风
	void BPDamageFiveTornado();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //清空已攻击
	void BPClearTargets();

public:
	virtual void StartSkill() override;
	virtual void BeginCollision(AActor* OtherActor) override;
	virtual void EndCollision(AActor* OtherActor) override;

private:
	FVector m_targetPos;
	FVector m_startPos;
	float m_allTm{};
	float m_curTm{};
	float m_InterTm{};
	float m_curInterTm{};

	std::vector<int> m_vecHasAttacks;
	int   m_nDamageRange1{};
	int   m_nDamageRange2{};
	int   m_nDamageRange3{};
	int   m_nDamageRange4{};
	int   m_nDamageType{};
	int   m_nBPDamageType{};
	int   m_nDamageSkillId{};
};
