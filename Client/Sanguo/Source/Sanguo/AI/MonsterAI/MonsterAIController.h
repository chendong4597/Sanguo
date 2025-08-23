// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RoleAIController.h"
#include "MonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API AMonsterAIController : public ARoleAIController
{
	GENERATED_BODY()

public:
	virtual void SetPerceptionUpdate(UAIPerceptionComponent* pPercept) override;

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit);

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION(BlueprintCallable, Category = "AI_MONSTER")
	void BackToBornPosition();

	UFUNCTION(BlueprintCallable, Category = "AI_MONSTER")
	void EndToBornPosition();

	virtual void InitAI() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector m_vecFocusPos;
	int     m_nLoseDis{};   //失去焦点距离，怪物为永远攻击时，判断是否离开始聚焦点太远
	int     m_nAIId{};
};
