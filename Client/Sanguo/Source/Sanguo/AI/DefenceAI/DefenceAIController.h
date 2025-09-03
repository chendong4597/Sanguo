// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RoleAIController.h"
#include "DefenceAIController.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ADefenceAIController : public ARoleAIController
{
	GENERATED_BODY()

public:
	virtual void SetPerceptionUpdate(UAIPerceptionComponent* pPercept) override;

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InitAI() override;
	
};
