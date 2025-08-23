// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginScene.h"
#include "../ReichGameInstance.h"


void ALoginScene::BeginPlay()
{
	SetLevelTpy(LevelType::LevelType_Login);
	Super::BeginPlay();
}

// Called every frame
void ALoginScene::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
