// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SkillEffectEmitter.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ASkillEffectEmitter : public ASkillEffect
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void RegisterOverlayEvents() override;
protected:
	void onSkillStopContinueEvent(const SkillStopContinueEvent& evt);
public:
	virtual void StartSkill() override;

	virtual void BeginCollision(AActor* OtherActor) override;
	virtual void EndCollision(AActor* OtherActor) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "SKILL_EMITTER")
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "SKILL_EMITTER")
	void NotifyAttack();

	//箭雨伤害  特殊要求
	UFUNCTION(BlueprintCallable, Category = "SKILL_EMITTER")
	void BPDamageInteral();

	//斜向冰柱伤害，特殊需求
	UFUNCTION(BlueprintCallable, Category = "SKILL_EMITTER")
	void BPDamageDisRange(float nDis, int nRange);

private:
	float m_allTm{};
	float m_interTm{};
	float m_curTm{};
	float m_interCurTm{};
	int   m_nNxtSkilNum{ 0 };
	int   m_nSkillIdx{ 0 };
	int   m_nTraceTpy{0};
	bool  m_bContinueAtk{ false };

	int m_nDamageRange{};
	int m_damageSkillId{};
}; 
