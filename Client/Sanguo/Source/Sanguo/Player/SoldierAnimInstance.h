// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SoldierAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API USoldierAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		float m_fSpeed; // 移动速度

	UPROPERTY(BlueprintReadOnly)
		float m_fDirection; // 方向(-180~180)

	UPROPERTY(BlueprintReadOnly)
		bool m_bFocus;

	UPROPERTY(BlueprintReadOnly)
		bool m_bDead;

	UPROPERTY(BlueprintReadOnly)
		int m_iWeaponTpy;

	UPROPERTY(BlueprintReadOnly)
		bool m_bDefenceing;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;

public:
	UFUNCTION()
		void AnimNotify_Attack(UAnimNotify * Notify);

	UFUNCTION()
		void AnimNotify_CavAtkEnd(UAnimNotify * Notify);

};
