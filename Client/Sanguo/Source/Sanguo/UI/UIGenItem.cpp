// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGenItem.h"
#include "../ReichGameInstance.h"
#include "UIWindow.h"

extern UWorld* gp_UWorld;

bool UUIGenItem::Initialize()
{
	if (!Super::Initialize())return false;
	if (UReichGameInstance::IsEditorMode()) return true;

	return true;
}

void UUIGenItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	
}
