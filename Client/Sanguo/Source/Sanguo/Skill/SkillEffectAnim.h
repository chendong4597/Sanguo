// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SkillEffectAnim.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API USkillEffectAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;
	
};
