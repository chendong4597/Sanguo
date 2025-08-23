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
	int     m_nLoseDis{};   //ʧȥ������룬����Ϊ��Զ����ʱ���ж��Ƿ��뿪ʼ�۽���̫Զ
	int     m_nAIId{};
};
