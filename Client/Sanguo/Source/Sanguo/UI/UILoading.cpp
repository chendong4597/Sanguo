// Fill out your copyright notice in the Description page of Project Settings.


#include "UILoading.h"
//#include "handler/HandlerLogin.h"
#include "../ReichGameInstance.h"
#include "Kismet/GameplayStatics.h"

bool UUILoading::Initialize()
{
	if (!Super::Initialize())return false;
	m_fMaxTm = 3.f;
	//REGISTER_EVENT(&HandlerLogin::getInstance(), HandlerTransterEvent, &UUILoading::onTransterEvent);
	//
	m_prgBar = Cast<UProgressBar>(GetWidgetFromName(FName("Process")));
	m_txtPrg = Cast<UTextBlock>(GetWidgetFromName(FName("ProText")));
	m_imgBg = Cast<UImage>(GetWidgetFromName(FName("ImgBg")));
	return true;
}

void UUILoading::UnInit()
{
	Super::UnInit();
}

void UUILoading::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	float preTm = m_fCurTm;
	m_fCurTm += InDeltaTime;
	if (m_bUnLoad) {
		IsTriggerState(ui_unload_state_players, preTm, m_fCurTm);
		IsTriggerState(ui_unload_state_npcs, preTm, m_fCurTm);
		IsTriggerState(ui_unload_state_npcs_end, preTm, m_fCurTm);
	}
	else {
		IsTriggerState(ui_load_state_player, preTm, m_fCurTm);
		IsTriggerState(ui_load_state_plant, preTm, m_fCurTm);
		IsTriggerState(ui_load_state_def, preTm, m_fCurTm);
		IsTriggerState(ui_load_state_house, preTm, m_fCurTm);
		IsTriggerState(ui_load_state_npc, preTm, m_fCurTm);
		IsTriggerState(ui_load_state_init, preTm, m_fCurTm);
		IsTriggerState(ui_unload_state_npcs_end, preTm, m_fCurTm);
	}
	if (m_prgBar)
	{
		float fValue = m_fCurTm / m_fMaxTm;
		fValue = fValue > 1.f ? 1.f : fValue;
		m_prgBar->SetPercent(fValue);
	}

	//·ÀÖ¹¿¨×¡
	if (m_fCurTm > (m_fMaxTm + 1.f)) {
		RemoveFromParent();
	}
}

void UUILoading::IsTriggerState(int nState, float preTm, float curTm)
{
	float trigerTm = m_fMaxTm*nState / ui_unload_state_end;
	if (preTm < trigerTm && curTm >= trigerTm) {
		UILoadintEvent evt;
		evt.m_type = m_bUnLoad ? UILoadintEvent::LoadingType::Pre_Load : UILoadintEvent::LoadingType::Nxt_Load;
		evt.m_loadstate = nState;
		UI_DISPATCH_MYEVENT(UILoadintEvent, evt);

		if (m_txtPrg)
		{
			STR_FORMAT_ARG_1(nState);
			std::string str = TCHAR_TO_UTF8(*FString::Format(TEXT("UI_local_Trans_Tips_{0}"), Args));//, nState);
			m_txtPrg->SetText(ORI_TO_TEXT(str));
		}
		if (nState == ui_unload_state_end)
		{
			RemoveFromParent();
		}
	}
}


void UUILoading::SetIsLoadingPre(bool bPreLoad)
{ 
	m_bUnLoad = bPreLoad;
	m_load_status = m_bUnLoad ? ui_unload_state_begin : ui_load_state_begin;
	std::string strTips = m_bUnLoad ? "UI_local_Trans_Tips_0" : "UI_local_Trans_Tips_1";
	if (m_txtPrg)
	{
		m_txtPrg->SetText(ORI_TO_TEXT(strTips));
	}
	m_fCurTm = m_bUnLoad ? 0.f : (m_fMaxTm * ui_load_state_begin / ui_unload_state_end) - 0.02f;

	if (m_imgBg)
	{
		FString strTxt;
		if (m_bUnLoad)
		{
			strTxt = "Texture2D'/Game/UI/Assert/Bg/BgLoad1.BgLoad1'";
		}
		else {
			strTxt = "Texture2D'/Game/UI/Assert/Bg/BgLoad2.BgLoad2'";
		}
		UTexture2D* tmpTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, strTxt.GetCharArray().GetData()));
		if (!tmpTexture)
		{
			return;
		}
		m_imgBg->SetBrushFromTexture(tmpTexture, false);
	}
}
