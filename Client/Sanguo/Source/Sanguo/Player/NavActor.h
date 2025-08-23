// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavActor.generated.h"

UCLASS()
class SANGUO_API ANavActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANavActor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Obj | Nav")
	void NavAddMesh(FTransform trans);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
