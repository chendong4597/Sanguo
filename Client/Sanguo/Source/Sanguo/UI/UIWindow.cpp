// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIWindow.h"


bool UUIWindow::Initialize()
{
	if (!Super::Initialize())return false;
	//if (UReichGameInstance::IsEditorMode()) return false;

	//SetInteractable(false);
	return true;
}

void UUIWindow::NativeDestruct()
{
	//UIManager::getInstance().CloseWidget(this);
	Super::NativeDestruct();
}


void UUIWindow::BeginDestroy()
{
	//UIManager::getInstance().RemoveWidget(this);
	return Super::BeginDestroy();
}

FReply UUIWindow::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
	if (m_bModel) {
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UUIWindow::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UUIWindow::NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
	if (m_bModel) {
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UUIWindow::NativeOnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
	if (m_bModel) {
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UUIWindow::NativeOnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
	if (m_bModel) {
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UUIWindow::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}