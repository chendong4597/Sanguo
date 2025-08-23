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
	int m_iWeaponTpy; // ��������

	UPROPERTY(BlueprintReadOnly)
	float m_fSpeed; // �ƶ��ٶ�

	UPROPERTY(BlueprintReadOnly)
	bool m_bDead; // �Ƿ�����

	UPROPERTY(BlueprintReadOnly)
	bool m_bDizz; // �Ƿ���ѣ

	UPROPERTY(BlueprintReadOnly)
	bool m_bFreeze; // �Ƿ񱻱���

	UPROPERTY(BlueprintReadOnly)
	float m_fFighting; // �Ƿ�ս��״̬

	UPROPERTY(BlueprintReadOnly)
	bool m_bDefenceing; // �Ƿ��ڷ���״̬��ֻ�Բ�������

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;

	UFUNCTION()
	void AnimNotify_NormalEffect(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_CavAtkEnd(UAnimNotify * Notify);
	

public:
	AGenerals* m_pPlayer;
	
};
