// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SkillEffectLaser.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ASkillEffectLaser : public ASkillEffect
{
	GENERATED_BODY()
	//DECLARE_EVENT_HOST();
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void RegisterOverlayEvents() override;

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	float GetLength() {return m_fLength;}

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	float GetSpeed() { return m_Speed; }

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	float GetTm() { return m_Tm; }

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void DamageSingle(AActor* pActor);

protected:
	void onSkillStopContinueEvent(const SkillStopContinueEvent& evt);
public:
	virtual void StartSkill() override;
	virtual void BeginCollision(AActor* OtherActor) override;
	virtual void EndCollision(AActor* OtherActor) override;
private:
	FVector m_targetPos;
	FVector m_startPos;
	std::string m_strSockName;
	float m_fLength;
	float m_Tm{};
	float m_allTm{};
	float m_Speed{};
	float m_fCastDistance{};		//施法距离，持续施法中判断

	float m_InterTm{ 0.f };
	float m_curInterTm{ 0.f };

	int   m_nDamageTpy{ 0 };
	int   m_nDamageSkillId{ 0 };
};
