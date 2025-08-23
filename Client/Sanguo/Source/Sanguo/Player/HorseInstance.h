// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/AudioComponent.h"
#include "HorseInstance.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API UHorseInstance : public UAnimInstance
{
	GENERATED_BODY()

	enum HORSE_RUN_STATE {
		HORSE_RUN_NONE = 0,
		HORSE_RUN_IDLE,
		HORSE_RUN_RUNING
	};
public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUninitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void BeginDestroy() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Horse")
	UAudioComponent* m_audioComp {nullptr};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Horse")
	int m_nSoundPlay{ 0 };

	void StopAudio();
	void StartAudio();


	void PlayAudio();

private:
	int m_nHorseId{ 0 };
};
