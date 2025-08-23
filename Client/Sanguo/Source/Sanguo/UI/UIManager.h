// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HAL/Platform.h"
#include "../Controls/MyButton.h"

#include "../Common/UIEvents.h"

enum {
	UI_Z_ORDER_UNDER = -1,
	UI_Z_ORDER_NOPRMAL = 0,
	UI_Z_ORDER_MSGBOX = 900,
	UI_Z_ORDER_MSGTIPS = 901,
	UI_Z_ORDER_MSG_DUN_READY = 910,
	UI_Z_ORDER_MSG_DEATH = 980,
	UI_Z_ORDER_MSG_LOADING = 990,
	UI_Z_ORDER_MSG_OUT = 1000,
};

#define REGISTER_BTN_CLICK(BTNName , FUN_NAME) \
{\
UButton* btn = Cast<UButton>(GetWidgetFromName(FName(BTNName))); \
if (btn)\
{\
	FScriptDelegate sgbDelegate;  \
	sgbDelegate.BindUFunction(this, #FUN_NAME );  \
	btn->OnClicked.Add(sgbDelegate);  \
}\
}\

#define REGISTER_BTN_CLICK_M(btn, BTNName , FUN_NAME) \
{\
btn = Cast<UButton>(GetWidgetFromName(FName(BTNName))); \
if (btn)\
{\
	FScriptDelegate sgbDelegate;  \
	sgbDelegate.BindUFunction(this, #FUN_NAME );  \
	btn->OnClicked.Add(sgbDelegate);  \
}\
}\

#define REGISTER_BTN_PRESSED(BTNName , FUN_NAME) \
{\
UButton* btn = Cast<UButton>(GetWidgetFromName(FName(BTNName))); \
if (btn)\
{\
	FScriptDelegate sgbDelegate;  \
	sgbDelegate.BindUFunction(this, #FUN_NAME );  \
	if(UIManager::getInstance().IsPlatformWindow()) { \
		btn->OnPressed.Add(sgbDelegate);  \
	} \
	else { \
		btn->OnHovered.Add(sgbDelegate);  \
	} \
}\
}\

#define REGISTER_BTN_RELEASED(BTNName , FUN_NAME) \
{\
UButton* btn = Cast<UButton>(GetWidgetFromName(FName(BTNName))); \
if (btn)\
{\
	FScriptDelegate sgbDelegate;  \
	sgbDelegate.BindUFunction(this, #FUN_NAME );  \
	if(UIManager::getInstance().IsPlatformWindow()) { \
		btn->OnReleased.Add(sgbDelegate);  \
	} \
	else { \
		btn->OnUnhovered.Add(sgbDelegate);  \
	} \
}\
}\

//*************************************************************************************************************************
//自定义控件
//*************************************************************************************************************************

#define REGISTER_MY_BTN_CLICK(BTNName , FUN_NAME) \
{\
UMyButton* btn = Cast<UMyButton>(GetWidgetFromName(FName(BTNName))); \
if (btn)\
{\
	FScriptDelegate sgbDelegate;  \
	sgbDelegate.BindUFunction(this, #FUN_NAME );  \
	btn->m_btn->OnClicked.Add(sgbDelegate);  \
}\
}\

#define REGISTER_MY_BTN_CLICK_M(BTN, BTNName , FUN_NAME) \
{\
BTN = Cast<UMyButton>(GetWidgetFromName(FName(BTNName))); \
if (BTN)\
{\
	FScriptDelegate sgbDelegate;  \
	sgbDelegate.BindUFunction(this, #FUN_NAME );  \
	BTN->m_btn->OnClicked.Add(sgbDelegate);  \
}\
}\


//*************************************************************************************************************************
//自定义宏
//*************************************************************************************************************************
enum UI_TPY {
	UI_TPY_None = 0,    //
	UI_TPY_Login,    //
	UI_TPY_CreateRole,    //
	UI_TPY_Lobby,    //
	UI_TPY_DEBUGVIEW,  //
	UI_TPY_PlantView,   //
	UI_TPY_DefenceInfo,  //
	UI_TPY_PlantsInfo,  //
	UI_TPY_HouseInfo,  //
	UI_TPY_TeamInfo,  //
	UI_TPY_Setup,  //
	UI_TPY_GeneralEquip,  //
};

enum UI_VIEWMODE {
	UI_VIEWMODE_HERO = 0,  //摄像机在人身体上跟踪
	UI_VIEWMODE_CITY  //城市模式，用于种植等操作
};

class UIManager
{
	//DECLARE_EVENT_HOST();
public:
	static UIManager& getInstance();

	void initialize();
	void terminate();
	void update(float dt);
	void reset();

	void addWidget(UUserWidget* pWidget);
	void RemoveWidget(UUserWidget* pWidget);

	void CloseWidget(UUserWidget* pWidget);
	void CloseWindow(int uiType);

	UUserWidget* GetWidget(UI_TPY);
	UUserWidget* CreateNewWidget(const char* pszBPPath, UObject* InOuter);

	UUserWidget* ShowUI(const TCHAR* path, int nUIType, bool bAddShow = true);
	void ShowLobby(bool bShow);

	bool IsPlatformWindow();

	void onWindowEvent(const WindowEvent& evt);

	UI_VIEWMODE GetViewMode() { return m_viewMode; }
	void SetViewMode(UI_VIEWMODE viewMode) { m_viewMode = viewMode; }
	bool IsShowUI() const { return m_bShowUI; }
	void ResetShowUI();
	bool IsUIShowByType(WindowEvent::EventType uiType);
	bool CloseWindowByType(WindowEvent::EventType uiType);

	void SetLayoutBang(int nLayoutBang) { m_nLayoutBang = nLayoutBang; }
	int  GetLayoutBang() { return m_nLayoutBang; }
private:
	TArray<UUserWidget*> m_vecWidgets;
	UI_VIEWMODE m_viewMode{ UI_VIEWMODE_HERO };
	bool m_bShowUI{ false };

	int  m_nLayoutBang{ 100 };//刘海留边
};
