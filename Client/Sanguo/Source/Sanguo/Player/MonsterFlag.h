// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterFlag.generated.h"

UCLASS()
class SANGUO_API AMonsterFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonsterFlag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddSubMobNum();

public:
	UPROPERTY(EditAnywhere, Category = "")
	int m_insId{ 0 };

	UPROPERTY(EditAnywhere, Category = "")
	int m_mobTpy{ 0 };  //0:怪物  1:防御塔

	UPROPERTY(EditAnywhere, Category = "")
	int m_nStage{ 0 };  //0:第一阶段  1:第二阶段

	UPROPERTY(EditAnywhere, Category = "")
	int m_id{ 0 };

	UPROPERTY(EditAnywhere, Category = "")
	int m_mobId{ 0 };

	UPROPERTY(EditAnywhere, Category = "")
	int m_subMobId{ 0 };

	UPROPERTY(EditAnywhere, Category = "")
	int m_subMobNum{ 0 };

private:
	int m_curSubNum{ 0 };

};
