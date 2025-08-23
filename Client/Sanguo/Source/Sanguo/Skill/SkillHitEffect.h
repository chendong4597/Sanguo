// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillEffect.h"
#include "SkillHitEffect.generated.h"

//技能击中特效

UCLASS()
class SANGUO_API ASkillHitEffect : public ASkillEffect
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillHitEffect();

	virtual void StartSkill() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
