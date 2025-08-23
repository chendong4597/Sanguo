// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMsgBox.h"
#include "ReichGameInstance.h"
#include "../Player/IPlayer.h"
#include "../Player/Hero.h"
#include "UIManager.h"

extern UWorld* gp_UWorld;

static UUIMsgBox* g_objMsgbox = nullptr;

bool UUIMsgBox::Initialize()
{
	Super::Initialize();
	if (UReichGameInstance::IsEditorMode()) return false;
	m_textContent = Cast<UTextBlock>(GetWidgetFromName(FName("LabContent")));
	m_textTitle = Cast<UTextBlock>(GetWidgetFromName(FName("LabTitle")));

	REGISTER_MY_BTN_CLICK_M(m_btnConfirm, "BtnConfirm", onBtnConfirm);
	REGISTER_MY_BTN_CLICK_M(m_btnCancel, "BtnCancel", onBtnCancel);
	REGISTER_BTN_CLICK_M(m_btnClose, "BtnClose", onBtnClose);
	return true;
}

void UUIMsgBox::NativePreConstruct()
{
	g_objMsgbox = this;
	Super::NativePreConstruct();
}

void UUIMsgBox::NativeDestruct()
{
	g_objMsgbox = nullptr;
	Super::NativeDestruct();
}

void UUIMsgBox::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (m_fLessTm >= 0.f) {
		m_fLessTm -= InDeltaTime;
	}
	else {
		return;
	}
	if (!m_btnConfirm) return;

	if (m_fLessTm >= 0.f) {
		STR_FORMAT_ARG_2(LC_TO_STRING(__LC("UI_MSGBOX_NORMAL_CONFIRM")), static_cast<int>(m_fLessTm + 1.f));
		FString str = FString::Format(TEXT("{0} ({1}s)"), Args);
		m_btnConfirm->SetMyText(FText::FromString(str));
		m_btnConfirm->SetState(UMyButton_State_Disable);
	}
	else {
		//_LC("UI_MSGBOX_NORMAL_CONFIRM")
		std::string str = __LC("UI_MSGBOX_NORMAL_CONFIRM");
		m_btnConfirm->SetMyText(LC_TO_TEXT(str));
		m_btnConfirm->SetState(UMyButton_State_Normal);
	}
}

void UUIMsgBox::SetDelayConfirm(float fTm)
{
	m_fLessTm = fTm;
}

void UUIMsgBox::showMsgBox(UUIMsgBox::MsgBoxUIType nUi, int nStyle, const char* pszContent, const char* pszTitle, bool bFirst)
{
	MsgBoxParam param;
	param.m_nUITpy = nUi;
	param.m_nBoxTpy = nStyle;
	param.strTitle = pszTitle ? pszTitle : __LC("UI_MSGBOX_NORMAL_TITLE");
	param.strContent = pszContent ? pszContent : "";
	m_queueParams.push_back(param);
	if (bFirst)
	{
		ShowNextMsgBox();
	}
}

void UUIMsgBox::setCallBack(MsgBoxUIType nUITpy, std::function<void(const MsgBoxEvent& evt)> callFunc)
{
	if (m_param.m_nUITpy == nUITpy)
	{
		m_param.m_funcCallBack = callFunc;
	}
	else {
		for (unsigned i = 0; i < m_queueParams.size(); i++)
		{
			if (m_queueParams.at(i).m_nUITpy == nUITpy)
			{
				m_queueParams.at(i).m_funcCallBack = callFunc;
				break;
			}
		}
	}
}

void UUIMsgBox::ShowNextMsgBox()
{
	if (m_queueParams.size() > 0) {
		m_param = m_queueParams.at(0);
		if (m_textTitle) {
			FString str = UTF8_TO_TCHAR(m_param.strTitle.c_str());
			m_textTitle->SetText(FText::FromString(str));
		}
		if (m_textContent) {
			FString str = UTF8_TO_TCHAR(m_param.strContent.c_str());
			m_textContent->SetText(FText::FromString(str));
		}
		m_queueParams.pop_front();
	}
	else {
		RemoveFromParent();
	}
}

void UUIMsgBox::SetCancelText(FText txt)
{
	if (m_btnCancel) {
		m_btnCancel->SetMyText(txt);
	}
}

void UUIMsgBox::onBtnConfirm()
{
	if (m_param.m_nUITpy != MsgBoxUIType_None) {
		MsgBoxEvent evt;
		evt.m_nUIType = m_param.m_nUITpy;
		evt.m_nBoxType = BoxStyle::Confirm;
		if (m_param.m_funcCallBack)
		{
			m_param.m_funcCallBack(evt);
		}
		else if(m_param.m_bPostBoxEvent) {
			UI_DISPATCH_MYEVENT(MsgBoxEvent, evt);
		}
	}
	ShowNextMsgBox();
}

void UUIMsgBox::onBtnCancel()
{
	if (m_param.m_nUITpy != MsgBoxUIType_None) {
		MsgBoxEvent evt;
		evt.m_nUIType = m_param.m_nUITpy;
		evt.m_nBoxType = BoxStyle::Cancel;
		if (m_param.m_funcCallBack)
		{
			m_param.m_funcCallBack(evt);
		}
		else if (m_param.m_bPostBoxEvent) {
			UI_DISPATCH_MYEVENT(MsgBoxEvent, evt);
		}
	}
	ShowNextMsgBox();
}

void UUIMsgBox::onBtnClose()
{
	ShowNextMsgBox();
}

//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
bool MsgBoxMgr::initialize()
{
	return true;
}

void MsgBoxMgr::terminate()
{

}

bool MsgBoxMgr::reset()
{
	return true;
}

void MsgBoxMgr::update(float dt)
{

}

UUIMsgBox* MsgBoxMgr::normalMsgBox(UUIMsgBox::MsgBoxUIType nUi, int nStyle, const char* pszContent, const char* pszTitle)
{
	if (g_objMsgbox)
	{
		g_objMsgbox->showMsgBox(nUi, nStyle, pszContent, pszTitle, false);
		return g_objMsgbox;
	}
	else {
		TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(AIPlayerMgr::getInstance().GetHero(),
			TEXT("WidgetBlueprint'/Game/UI/Base/BP_UIMsgBox.BP_UIMsgBox_C'"));
		if (WidgetClass) {
			UWorld* pWorld = nullptr;
			if (AIPlayerMgr::getInstance().GetHero()) {
				pWorld = AIPlayerMgr::getInstance().GetHero()->GetWorld();
			}
			else {
				pWorld = gp_UWorld;
			}
			UUserWidget* UserWidget = CreateWidget<UUserWidget>(pWorld, WidgetClass);
			UUIMsgBox* pMsgbox = Cast<UUIMsgBox>(UserWidget);
			if (pMsgbox)
			{
				pMsgbox->AddToViewport(UI_Z_ORDER_MSGBOX);
				g_objMsgbox->showMsgBox(nUi, nStyle, pszContent, pszTitle, true);
			}
			return pMsgbox;
		}
	}
	return NULL;
}

UUIMsgBox* MsgBoxMgr::MsgBox(UUIMsgBox::MsgBoxUIType nUi, int nStyle, const std::string& strContent, const std::string& strTitle)
{
	return normalMsgBox(nUi, nStyle, strContent.c_str(), strTitle.c_str());
}

MsgBoxMgr::MsgBoxMgr()
{
	initialize();
}

MsgBoxMgr& MsgBoxMgr::getInstance()
{
	static MsgBoxMgr s_msgboxMgr;
	return s_msgboxMgr;
}
