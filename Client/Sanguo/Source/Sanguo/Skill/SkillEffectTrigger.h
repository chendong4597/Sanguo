// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SkillEffectTrigger.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ASkillEffectTrigger : public ASkillEffect
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void StartSkill() override;
private:
	float m_curTm{};
	float m_interTm{};
	float m_allTm{};
	int   m_nBuffId{};
	bool  m_nHasAdd{false};
};
