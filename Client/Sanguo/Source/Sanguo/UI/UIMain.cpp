// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMain.h"
//#include "handler/HandlerLogin.h"
#include "../ReichGameInstance.h"
#include "Kismet/GameplayStatics.h"

bool UUIMain::Initialize()
{
	if (!Super::Initialize())return false;
	return true;
}

void UUIMain::UnInit()
{
	Super::UnInit();
}

void UUIMain::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

