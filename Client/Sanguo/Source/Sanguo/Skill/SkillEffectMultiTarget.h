// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SkillEffectMultiTarget.generated.h"


/**
 * 
 */
UCLASS()
class SANGUO_API ASkillEffectMultiTarget : public ASkillEffect
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void StartSkill() override;
	virtual void BeginCollision(AActor* OtherActor) override;
	virtual void EndCollision(AActor* OtherActor) override;

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")  //刚开始的时候位置不对，延时下伤害
	void BPDamageDelay();

	void SingleDamage();

private:
	FVector m_targetPos;
	FVector m_startPos;
	float m_allTm{};
	float m_curTm{};
	int   m_nDamageRange1{};
	int   m_nDamageSkillId{};
};
