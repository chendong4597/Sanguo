// Fill out your copyright notice in the Description page of Project Settings.


#include "RootEventObject.h"
#include "Blueprint/UserWidget.h"

void URootEventObject::InitializeEvents()
{
	//FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this , &URootEventObject::LoadedMapEvent);
	//FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &URootEventObject::PreLoadedMapEvent);
}

void URootEventObject::LoadedMapEvent(UWorld* world)
{
	
}

void URootEventObject::PreLoadedMapEvent(const FString& strWord)
{
}