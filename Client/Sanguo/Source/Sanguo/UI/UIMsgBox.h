// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include <deque>
#include "GameAppExtern.h"
#include "../Controls/MyButton.h"
#include "../Common/UIEvents.h"
#include "UIManager.h"
#include "UIWindow.h"
#include "UIMsgBox.generated.h"

using namespace NS;
/**
 * 
 */


UCLASS()
class SANGUO_API UUIMsgBox : public UUserWidget
{
	GENERATED_BODY()

public:
	enum MsgBoxConfirmKey : NS::uint // ����ȷ��Key, ������´β�����ʾ
	{
		None = 0,
	};

	enum MsgBoxUIType : NS::uint // ����ȷ��Key, ������´β�����ʾ
	{
		MsgBoxUIType_None = 0,
		MsgBoxUIType_Normal,
		MsgBoxUIType_HouseTips,
		MsgBoxExitDungeon_Tips,  //�˳�������ʾ
		MsgBoxExitPlantFire_Tips, //���ӽ����ʾ		
	};

	enum BoxStyle : NS::uint // ������ʽ
	{
		Close = 1u << 0,
		Cancel = 1u << 1,
		Confirm = 1u << 2,
		Checkbox = 1u << 3,
		PicIcon = 1u << 4,
		Titel = 1u << 5,
		Text = 1u << 6,
		WarnText = 1u << 7,
		MoneyText = 1u << 8,
		ListText = 1u << 9,
		Warning = 1u << 10,
		CountDown = 1u << 11,

		SHOW_TEXT = Cancel | Confirm | Titel | Text,														// �ı� ����
		SHOW_TEXT_WARNTEXT = Cancel | Confirm | Titel | Text | WarnText,									// �ı� ����(�������ı�)
		SHOW_TEXT_WARNTEXT_CHECKBOX = Cancel | Confirm | Checkbox | Titel | Text | WarnText,				// �ı� ����(�������ı�, ��CheckBox)
		SHOW_MONEYTEXT = Cancel | Confirm | Titel | MoneyText,												// ������ı� ����
		SHOW_MONEYTEXT_CHECKBOX = Cancel | Confirm | Checkbox | Titel | MoneyText,							// ������ı� ����(��CheckBox)
		SHOW_MONEYTEXT_WARNTEXT = Cancel | Confirm | Titel | MoneyText | WarnText,							// ������ı� ����(�������ı�)
		SHOW_MONEYTEXT_WARNTEXT_CONFIRM = Confirm | Titel | MoneyText | WarnText,							// ������ı� ����(�������ı�, ֻ��ȷ����ť)
		SHOW_MONEYTEXT_WARNTEXT_CHECKBOX = Cancel | Confirm | Checkbox | Titel | MoneyText | WarnText,		// ������ı� ����(��CheckBox,�������ı�)
		SHOW_LISTTEXT = Cancel | Confirm | Titel | ListText,
		SHOW_LISTTEXT_WARNTEXT = Cancel | Confirm | Titel | ListText | WarnText,
		SHOW_LISTTEXT_WARNTEXT_CHECKBOX = Cancel | Confirm | Titel | ListText | WarnText | Checkbox,
		SHOW_YES = Confirm | Titel | Text,
		SHOW_NO = Cancel | Titel | Text,
		SHOW_YES_NO = Cancel | Confirm | Titel | Text,
		SHOW_YES_CLOSE = Confirm | Close | Titel | Text,
		SHOW_YES_NO_CLOSE = Cancel | Confirm | Close | Titel | Text,
		SHOW_YES_NO_CHKBOX = Cancel | Confirm | Checkbox | Titel | Text,
		SHOW_YES_NO_CHKBOX_CLOSE = Cancel | Confirm | Close | Checkbox | Titel | Text,
		SHOW_YES_NO_WARNING = Warning | Titel | Text,
		SHOW_YES_NO_WARNING_COUNT = Warning | Titel | Text | CountDown,
	};

	struct MsgBoxParam {
		MsgBoxUIType m_nUITpy{ MsgBoxUIType_None };
		NS::uint m_nBoxTpy{ 0 };
		std::string strTitle;
		std::string strContent;
		std::string strConformTxt;
		std::string strCancelTxt;

		bool m_bPostBoxEvent{false};
		std::function<void(const MsgBoxEvent& evt)> m_funcCallBack{ nullptr };
	};

	typedef std::deque<MsgBoxParam> QUENE_MSGBOX_PARAMS;

public:
	virtual bool Initialize() override;
	virtual void NativePreConstruct();
	virtual void NativeDestruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void showMsgBox(UUIMsgBox::MsgBoxUIType nUi, int nStyle, const char* pszContent, const char* pszTitle, bool bFirst);
	void ShowNextMsgBox();
	void setCallBack(MsgBoxUIType nUITpy, std::function<void(const MsgBoxEvent& evt)> callFunc);

	void SetCancelText(FText txt);
	void SetDelayConfirm(float fTm);
protected:
	UFUNCTION()
	void onBtnConfirm();

	UFUNCTION()
	void onBtnCancel();

	UFUNCTION()
	void onBtnClose();


private:
	QUENE_MSGBOX_PARAMS m_queueParams;
	MsgBoxParam m_param;
	UTextBlock* m_textContent{ nullptr };
	UTextBlock* m_textTitle{ nullptr };

	UMyButton* m_btnConfirm{ nullptr };
	UMyButton* m_btnCancel{ nullptr };
	UButton* m_btnClose{ nullptr };

	float m_fLessTm{ 0.f };
};

class MsgBoxMgr// : public bzbee::MsgHandler
{
	MsgBoxMgr();
	//DECLARE_EVENT_HOST();
public:
	bool initialize();
	void terminate();
	bool reset();
	void update(float dt);

	UUIMsgBox* normalMsgBox(UUIMsgBox::MsgBoxUIType nUi, int nStyle, const char* pszContent, const char* pszTitle = nullptr);
	UUIMsgBox* MsgBox(UUIMsgBox::MsgBoxUIType nUi, int nStyle, const std::string& strContent, const std::string& strTitle = "");
public:
	static MsgBoxMgr& getInstance();

private:
};
