// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnim.generated.h"




/**
 * 
 */
UCLASS()
class SANGUO_API UMonsterAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	float Speed; // �ƶ��ٶ�

	UPROPERTY(BlueprintReadOnly)
	float fFighting; // �ƶ��ٶ�

	UPROPERTY(BlueprintReadOnly)
	float Direction; // ����(-180~180)

	UPROPERTY(BlueprintReadOnly)
	bool bFocusPlayer;

	UPROPERTY(BlueprintReadOnly)
	bool m_bDead;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;

public:
	UFUNCTION()
	void AnimNotify_Attack(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_Magic(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_MagicEnd(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_DeathEnd(UAnimNotify * Notify);
};
