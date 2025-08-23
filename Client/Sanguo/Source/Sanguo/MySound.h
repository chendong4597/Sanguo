// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MySound.generated.h"

UCLASS()
class SANGUO_API AMySound : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMySound();

	UFUNCTION(BlueprintCallable, Category = "")
	void SetVolume(float fVolume);

	UFUNCTION(BlueprintImplementableEvent, Category = "AMySound")
	void VolumeEvt(float fVolume);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
