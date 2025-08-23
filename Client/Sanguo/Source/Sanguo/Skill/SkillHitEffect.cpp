// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillHitEffect.h"

// Sets default values
ASkillHitEffect::ASkillHitEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkillHitEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkillHitEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkillHitEffect::StartSkill()
{
}

