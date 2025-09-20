// Fill out your copyright notice in the Description page of Project Settings.


#include "Crystal.h"
#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Monster.h"

#include "config/WeaponTypeCfg.h"
#include "config/WeaponInfoTypeCfg.h"
#include "../ReichGameInstance.h"

// Sets default values
ACrystal::ACrystal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACrystal::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACrystal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

