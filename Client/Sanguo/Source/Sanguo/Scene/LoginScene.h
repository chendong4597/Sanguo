// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scene/LevelBase.h"
#include "LoginScene.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ALoginScene : public ALevelBase
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
