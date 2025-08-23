// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RoleAIController.h"
#include "SoldierAIController.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ASoldierAIController : public ARoleAIController
{
	GENERATED_BODY()

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit);
	

	virtual void InitAI() override;
	virtual void SetPerceptionUpdate(UAIPerceptionComponent* pPercept) override;
};
