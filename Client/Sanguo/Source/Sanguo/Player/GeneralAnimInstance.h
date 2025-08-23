// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Generals.h"
#include "GeneralAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API UGeneralAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	int m_iWeaponTpy; // 武器类型

	UPROPERTY(BlueprintReadOnly)
	float m_fSpeed; // 移动速度

	UPROPERTY(BlueprintReadOnly)
	bool m_bDead; // 是否死亡

	UPROPERTY(BlueprintReadOnly)
	bool m_bDizz; // 是否晕眩

	UPROPERTY(BlueprintReadOnly)
	bool m_bFreeze; // 是否被冰冻

	UPROPERTY(BlueprintReadOnly)
	float m_fFighting; // 是否战斗状态

	UPROPERTY(BlueprintReadOnly)
	bool m_bDefenceing; // 是否在防御状态，只对步兵有用

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;

	UFUNCTION()
	void AnimNotify_NormalEffect(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_CavAtkEnd(UAnimNotify * Notify);
	

public:
	AGenerals* m_pPlayer;
	
};
