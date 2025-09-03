// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "DefenceBuilding.h"
#include "DefenceAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API UDefenceAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	bool m_bDead; // 是否死亡

	UPROPERTY(BlueprintReadOnly)
	int m_nContinueIdx; // 连续攻击动画索引

	UFUNCTION()
	void AnimNotify_MagicEffect(UAnimNotify * Notify);

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;

public:
	ADefenceBuilding* m_pDefence{};
	
};
