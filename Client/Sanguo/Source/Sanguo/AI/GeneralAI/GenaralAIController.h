// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RoleAIController.h"
#include "GenaralAIController.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API AGenaralAIController : public ARoleAIController
{
	GENERATED_BODY()

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit);

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);


	virtual void InitAI() override;
	virtual void SetPerceptionUpdate(UAIPerceptionComponent* pPercept) override;
	
};
