// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/AudioComponent.h"
#include "IPlayer.h"
#include "Hero.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	float m_fSpeed; // 移动速度

	UPROPERTY(BlueprintReadOnly)
	bool m_bBlend; // 混合动画

	UPROPERTY(BlueprintReadOnly)
	int m_iWeaponTpy; // 武器类型

	UPROPERTY(BlueprintReadOnly)
	bool m_bDead; // 是否死亡

	UPROPERTY(BlueprintReadOnly)
	bool m_bDizz; // 是否晕眩

	UPROPERTY(BlueprintReadOnly)
	bool m_bFreeze; // 是否被冰冻

	UPROPERTY(BlueprintReadOnly)
	float m_fFighting; // 是否战斗状态

	UPROPERTY(BlueprintReadOnly)
	bool m_bRiding; // 是否骑乘状态

	UPROPERTY(BlueprintReadOnly)
	int m_nContinueIdx; // 连续攻击动画索引

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeUninitializeAnimation() override;
	virtual void NativeInitializeAnimation() override;
	virtual void BeginDestroy() override;


	void StartAnimAudio();
	void StopAnimAudio();
	void PlayAnumAudio();
	void PauseAnimAudio();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Horse")
	UAudioComponent* m_audioComp { nullptr };

public:
	UFUNCTION()
	void AnimNotify_NormalEffect(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_SaveCombo(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_HideDarts(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_ShowDarts(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_MagicEff(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_ComboMagicEffect(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_HitEff(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_CalDamage(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_PreEffect(UAnimNotify * Notify);

	UFUNCTION()
	void AnimNotify_TriggerEffect(UAnimNotify * Notify);

public:
	AIPlayer* m_pPlayer;
	bool m_bSoundPlaying{ false };
};
