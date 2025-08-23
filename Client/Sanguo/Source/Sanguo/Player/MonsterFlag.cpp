// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterFlag.h"

// Sets default values
AMonsterFlag::AMonsterFlag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMonsterFlag::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMonsterFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMonsterFlag::AddSubMobNum()
{
	m_curSubNum++;
}

