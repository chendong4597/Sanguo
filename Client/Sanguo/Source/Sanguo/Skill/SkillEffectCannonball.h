// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SkillEffectCannonball.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ASkillEffectCannonball : public ASkillEffect
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
private:
	float m_speed;
	float m_allTm{};
	float m_curTm{};
	bool m_bHitDeath{};
public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	UCurveVector *m_CurveVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_fBomeHei{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_targetPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_startPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_bStartTmline{};

	FTimeline m_timelineBall;
};
