// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SkillEffectDarts.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ASkillEffectDarts : public ASkillEffect
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void StartSkill() override;
	virtual void BeginCollision(AActor* OtherActor) override;
	virtual void EndCollision(AActor* OtherActor) override;

public:
	UFUNCTION()
	void UpdateEffectLocation(FVector vec);

	UFUNCTION()
	void FinishEffectTimeline();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	void AddTimeEvent(float tm);

	UFUNCTION()
	void TimeEventBack();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	void ClearTargets() { m_vecHasAttacks.clear(); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_targetPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_startPos;
private:
	float m_speed;
	float m_allTm{};
	float m_curTm{};
	bool m_bHitDeath{};
	int   m_nCurve{};  //弹道类型
	FVector m_bendRangePos;
	float m_bendMaxLength;
	float m_bendDis{};
	
	//伤害计算
	float m_fDamageRange{};
	int   m_damageSkillId{};
	std::vector<int> m_vecHasAttacks;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector *m_CurveVector;
	
	FTimeline m_timelineBall;
};
