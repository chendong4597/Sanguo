// Fill out your copyright notice in the Description page of Project Settings.


#include "DisplayMgr.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Input/Reply.h"
#include "Logging/LogMacros.h"
#include "Blueprint/WidgetLayoutLibrary.h"


DisplayMgr::DisplayMgr()
{

}

DisplayMgr::~DisplayMgr()
{

}

DisplayMgr& DisplayMgr::getInstance()
{
	static DisplayMgr s_insDisplayMgr;
	return s_insDisplayMgr;
}

FVector2D DisplayMgr::GetScreenSize()
{
	return FVector2D(1920.f , 1080.f);
}


bool DisplayMgr::IsTouchSlot(UUserWidget* pWidget, UCanvasPanelSlot* slot, FVector2D vec)
{
	if (!pWidget || !slot || !pWidget->GetWorld() || !pWidget->GetWorld()->GetGameViewport())
	{
		return false;
	}
	int32 X = pWidget->GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
	int32 Y = pWidget->GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
	float Scale = UWidgetLayoutLibrary::GetViewportScale(pWidget);
	if (Scale < 0.000001f)
	{
		UE_LOG(LogOutputDevice, Log, TEXT("UUILobby::IsTouchSlot , Scale is Zero"));
		return false;
	}

	FVector2D viewVec = FVector2D(X/ Scale , Y/Scale);
	FVector2D vecPos = slot->GetPosition();
	FVector2D vecSize = slot->GetSize();
	UE_LOG(LogOutputDevice, Log, TEXT("UUILobby::IsTouchSlot , vec.x = %0.2f , vec.y = %0.2f , X = %d , Y=%d , scale =%0.2f vecSize.x = %0.2f vecSize.y = %0.2f vecPos.x = %0.2f vecPos.y = %0.2f"), vec.X, vec.Y, X, Y, Scale, vecSize.X, vecSize.Y, vecPos.X, vecPos.Y);

	FVector2D scaleVec = vec;
	//scaleVec.X = vec.X * Scale;
	//scaleVec.Y = vec.Y * Scale;
	//scaleVec.X = scaleVec.X*viewVec.X / X;
	//scaleVec.Y = scaleVec.Y*viewVec.Y / Y;
	vecPos.Y = viewVec.Y + vecPos.Y;
	if (slot->GetAnchors().Maximum.X == 1.0f)
	{
		vecPos.X = viewVec.X + vecPos.X;
	}
	if (scaleVec.X >= vecPos.X && scaleVec.X <= vecPos.X + vecSize.X
		&& scaleVec.Y >= vecPos.Y && scaleVec.Y <= vecPos.Y + vecSize.Y)
	{
		return true;
	}
	return false;
}

bool DisplayMgr::IsTouchOverlaySlot(UUserWidget* pWidget, UOverlaySlot* slot, FVector2D vec, FVector2D desSize)
{
	if (!pWidget || !slot || !pWidget->GetWorld() || !pWidget->GetWorld()->GetGameViewport())
	{
		return false;
	}
	int32 X = pWidget->GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
	int32 Y = pWidget->GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
	float Scale = UWidgetLayoutLibrary::GetViewportScale(pWidget);
	if (Scale < 0.000001f)
	{
		UE_LOG(LogOutputDevice, Log, TEXT("UUILobby::IsTouchOverlaySlot , Scale is Zero"));
		return false;
	}
	FVector2D viewVec = FVector2D(X / Scale, Y / Scale);
	

	FVector2D vecSize = FVector2D(desSize.X, desSize.Y);
	FVector2D oriPos = FVector2D(0, viewVec.Y - vecSize.Y);
	
	UE_LOG(LogOutputDevice, Log, TEXT("UUILobby::IsTouchOverlaySlot , vec.x = %0.2f , vec.y = %0.2f , X = %d , Y=%d , scale =%0.2f vecSize.x = %0.2f vecSize.y = %0.2f vecPos.x = %0.2f vecPos.y = %0.2f"),
		vec.X, vec.Y, X, Y, Scale, vecSize.X, vecSize.Y, oriPos.X, oriPos.Y);

	FVector2D scaleVec = vec;
	if (scaleVec.X >= oriPos.X && scaleVec.X <= oriPos.X + vecSize.X
		&& scaleVec.Y >= oriPos.Y && scaleVec.Y <= oriPos.Y + vecSize.Y)
	{
		return true;
	}
	return false;
}