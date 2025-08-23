// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/OverlaySlot.h"



//*************************************************************************************************
//
//*************************************************************************************************
class DisplayMgr// : public NS::MsgHandler
{
public:
	DisplayMgr();
	 ~DisplayMgr();

	static DisplayMgr& getInstance();

public:
	FVector2D GetScreenSize();
	bool IsTouchSlot(UUserWidget* pWidget, UCanvasPanelSlot* slot , FVector2D vec);
	bool IsTouchOverlaySlot(UUserWidget* pWidget, UOverlaySlot* slot, FVector2D vec, FVector2D size);

};

#define g_objDisplayMgr DisplayMgr::getInstance()


