// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SkillEffectArrow.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ASkillEffectArrow : public ASkillEffect
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void StartSkill() override;
	virtual void BeginCollision(AActor* OtherActor) override;
	virtual void EndCollision(AActor* OtherActor) override;

	void StartTimeline();

public:
	UFUNCTION()
	void UpdateEffectLocation(FVector vec);

	UFUNCTION()
	void FinishEffectTimeline();

	void FinishEffect();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	void AddTimeEvent(float tm);

	UFUNCTION()
	void TimeEventBack();

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	void ClearTargets() { m_vecHasAttacks.clear(); }

	UFUNCTION(BlueprintCallable, Category = "SKILL EFFECT")
	void BPDamageCircle(ARole* pRole, int nDamageRange, int nDamageSkillId);
private:
	float m_speed;
	
	float m_curTm{};
	bool m_bHitDeath{};
	FVector m_prePos{};
	int m_trace{};
	int m_nNxtSkilNum{ 0 };
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector *m_CurveVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_targetPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_startPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_fBomeHei{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_bStartTmline{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_allTm{};

	//…À∫¶
	std::vector<int> m_vecHasAttacks;
	int m_nDamageRange{};
	int m_nDamageRange1{};
	int m_nDamageTpy{};
	int m_damageSkillId{};

	FTimeline m_timelineBall;
};
