// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Role.h"
#include "BuffEffect.generated.h"

class ARole;

UCLASS()
class SANGUO_API ABuffEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuffEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RestStatus();

	void StartBuff(ARole* pOwner, int nBuffId);

	void BuffDestroy();

		
	//UFUNCTION(BlueprintCallable, Category = "BUFF")
	//int GetBuffId() { return m_nBuffId; }

	//UFUNCTION(BlueprintCallable, Category = "BUFF")
	//void SetBuffId(int nBuffId) { m_nBuffId = m_nBuffId; }

private:
	int m_nBuffId{};
	float m_fCurTm{};
	float m_fAllTm{ 2.f };
	int m_nAdditionNum{ 0 };
	int m_nFlllowRot{ 0 };

	ARole* m_pOwnerRole{ nullptr };
};
