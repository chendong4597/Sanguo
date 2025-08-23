// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMsgBoxToast.h"
#include "ReichGameInstance.h"
#include "../Player/IPlayer.h"
#include "../Player/Hero.h"
#include "UIManager.h"

extern UWorld* gp_UWorld;

static UUIMsgBoxToast* g_objMsgboxToast = nullptr;

bool UUIMsgBoxToast::Initialize()
{
	Super::Initialize();
	if (UReichGameInstance::IsEditorMode()) return false;
	m_textBlock  = Cast<UTextBlock>(GetWidgetFromName(FName("labTxt")));
	return true;
}

void UUIMsgBoxToast::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	m_curTm += InDeltaTime;
	if (m_curTm >= 1.5f) {
		RemoveFromParent();
	}
}

void UUIMsgBoxToast::NativePreConstruct()
{
	g_objMsgboxToast = this;
	Super::NativePreConstruct();
}

void UUIMsgBoxToast::NativeDestruct()
{
	g_objMsgboxToast = nullptr;
	Super::NativeDestruct();
}

void UUIMsgBoxToast::ShowText(const char* pszText)
{
	if (!pszText) {
		return;
	}
	FString str = UTF8_TO_TCHAR(pszText);
	m_arrayText.Add(str);
	if (!m_bNext) {
		ShowNextText();
	}
}

void UUIMsgBoxToast::NextTips()
{
	ShowNextText();
}

void UUIMsgBoxToast::ShowNextText()
{
	if (m_textBlock && m_arrayText.Num() > 0) {
		FString str = m_arrayText[0];
		m_textBlock->SetText(FText::FromString(str));
		m_arrayText.RemoveAt(0);
		m_bNext = true;
		StartShow();
		m_curTm = 0.f;
	} else {
		m_bNext = false;
	}
}


//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
bool MsgBoxToastMgr::initialize()
{
	return true;
}

void MsgBoxToastMgr::terminate()
{

}

bool MsgBoxToastMgr::reset()
{
	return true;
}

void MsgBoxToastMgr::update(float dt)
{

}

void MsgBoxToastMgr::ShowText(const std::string& strText)
{
	if (g_objMsgboxToast)
	{
		g_objMsgboxToast->ShowText(strText.c_str());
	}
	else {
		TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(AIPlayerMgr::getInstance().GetHero(), 
			TEXT("WidgetBlueprint'/Game/UI/Base/BP_UIMsgBoxToast.BP_UIMsgBoxToast_C'"));
		if (WidgetClass) {
			UWorld* pWorld = nullptr;
			if (AIPlayerMgr::getInstance().GetHero()) {
				pWorld = AIPlayerMgr::getInstance().GetHero()->GetWorld();
			} else {
				pWorld = gp_UWorld;
			}
			UUserWidget* UserWidget = CreateWidget<UUserWidget>(pWorld, WidgetClass);
			UUIMsgBoxToast* pMsgboxToast = Cast<UUIMsgBoxToast>(UserWidget);
			if (pMsgboxToast)
			{
				pMsgboxToast->AddToViewport(UI_Z_ORDER_MSGTIPS);
				pMsgboxToast->ShowText(strText.c_str());
			}
		}
	}
}

MsgBoxToastMgr::MsgBoxToastMgr()
{
	initialize();
}

MsgBoxToastMgr& MsgBoxToastMgr::getInstance()
{
	static MsgBoxToastMgr s_msgboxToastMgr;
	return s_msgboxToastMgr;
}

