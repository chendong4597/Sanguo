// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

#include "UIWindow.h"
#include "../Player/IPlayer.h"

extern UWorld* gp_UWorld;


UIManager& UIManager::getInstance()
{
	static UIManager _s;
	return _s;
}

void UIManager::initialize()
{
	//
}

void UIManager::terminate()
{
	reset();
}

void UIManager::reset()
{
	for (int i = 0; i < m_vecWidgets.Num(); i++)
	{
		if (!m_vecWidgets[i])
		{
			continue;
		}
		auto&& pWindow = Cast<UUIWindow>(m_vecWidgets[i]);
		if (pWindow) {
			pWindow->UnInit();
		}
		m_vecWidgets[i]->RemoveFromParent();
	}
	m_vecWidgets.Empty();
}

void UIManager::update(float dt)
{
	ShowLobby(!m_bShowUI);
}

void UIManager::ResetShowUI()
{
	bool bShowUI = false;
	for (int i = 0; i < m_vecWidgets.Num(); i++)
	{
		if (!m_vecWidgets[i])
		{
			continue;
		}
		//UUILobby* pLobby = Cast<UUILobby>(m_vecWidgets[i]);
		//if (pLobby)
		//{
		//	continue;
		//}
		bShowUI = true;
		break;
	}
	if (m_bShowUI != bShowUI)
	{
		m_bShowUI = bShowUI;
	}
}

void UIManager::addWidget(UUserWidget* pWidget)
{
	if (!m_vecWidgets.Contains(pWidget))
	{
		m_vecWidgets.Add(pWidget);
	}
	ResetShowUI();
}

void UIManager::RemoveWidget(UUserWidget* pWidget)
{
	auto&& pWindow = Cast<UUIWindow>(pWidget);
	if (pWindow) {
		pWindow->UnInit();
	}

	m_vecWidgets.Remove(pWidget);
	ResetShowUI();
}

void UIManager::CloseWidget(UUserWidget* pWidget)
{
	if (!pWidget) {
		return;
	}
	RemoveWidget(pWidget);
	pWidget->RemoveFromParent();
	//FString strPath = "SoundWave'/Game/Sound/effect/click2.click2'";
	//UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
	//if (soundObj && gp_UWorld)
	//{
	//	UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
	//}
}


void UIManager::CloseWindow(int uiType)
{
	for (int32 i = 0; i < m_vecWidgets.Num(); i++)
	{
		UUIWindow* pWin = Cast<UUIWindow>(m_vecWidgets[i]);
		if (!pWin || pWin->GetUIType() != uiType) {
			continue;
		}
		CloseWidget(pWin);
		break;
	}
}

bool UIManager::IsUIShowByType(WindowEvent::EventType uiType)
{
	for (int32 i = 0; i < m_vecWidgets.Num(); i++)
	{
		UUIWindow* pWin = Cast<UUIWindow>(m_vecWidgets[i]);
		if (!pWin || pWin->GetUIType() != (int)(uiType)) {
			continue;
		}
		return true;
	}
	return false;
}

bool UIManager::CloseWindowByType(WindowEvent::EventType uiType)
{
	for (int32 i = 0; i < m_vecWidgets.Num(); i++)
	{
		UUIWindow* pWin = Cast<UUIWindow>(m_vecWidgets[i]);
		if (!pWin || pWin->GetUIType() != (int)(uiType)) {
			continue;
		}
		CloseWidget(pWin);
		return true;
	}
	return false;
}


UUserWidget* UIManager::GetWidget(UI_TPY nTpy)
{
	for (int32 i = 0 ; i < m_vecWidgets.Num() ; i++)
	{
		switch (nTpy) {
		case UI_TPY_Lobby:
		{
		}
			break;
		default:break;
		}
	}
	return nullptr;
}

UUserWidget* UIManager::CreateNewWidget(const char* pszBPPath, UObject* InOuter)
{
	//if (pszBPPath == NULL)
	//{
	//	return nullptr;
	//}
	//UUserWidget* UserWidget = nullptr;
	//TSubclassOf<UUserWidget> WidgetClass = StaticLoadClass(UUserWidget::StaticClass() , InOuter, TEXT(pszBPPath) , nullptr);
	//if (WidgetClass) {
	//	UserWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	//	if (UserWidget)
	//	{
	//		UserWidget->AddToViewport();
	//	}
	//}
	return nullptr;
}

bool UIManager::IsPlatformWindow()
{
#if	PLATFORM_WINDOWS
	return true;
#else
	return false;
#endif
}


void UIManager::onWindowEvent(const WindowEvent& evt)
{
	bool bSound = true;
	int nUIType = static_cast<int>(evt.m_type);
	//switch (evt.m_type)
	//{
	//default:
	//	break;
	//}

	if (!bSound) return;

	//FString strPath = "SoundWave'/Game/Sound/effect/click01.click01'";
	//UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
	//if (soundObj && gp_UWorld)
	//{
	//	UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
	//}
}

UUserWidget* UIManager::ShowUI(const TCHAR* path, int nUIType, bool bAddShow)
{
	UUserWidget* UserWidget = nullptr;
	TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(nullptr , path);
	if (WidgetClass) {
		UserWidget = CreateWidget<UUserWidget>(gp_UWorld, WidgetClass);
		if (UserWidget)
		{
			UserWidget->AddToViewport();
			if (bAddShow) {
				addWidget(UserWidget);
			}
			UUIWindow* pWin = Cast<UUIWindow>(UserWidget);
			if (pWin && nUIType > 0) {
				pWin->SetUIType(nUIType);
			}
		}
	}
	else {
		UE_LOG(LogOutputDevice, Log, TEXT("UIManager::ShowUI failed path = %s"), path);
	}
	return UserWidget;
}


void UIManager::ShowLobby(bool bShow)
{
	UUserWidget* pWidget = UIManager::getInstance().GetWidget(UI_TPY_Lobby);
	if (!pWidget)
	{
		return;
	}
	//UUILobby* pUI = Cast<UUILobby>(pWidget);
	//if(!pUI){
	//	return;
	//}
	//pUI->ShowUI(bShow);
}







