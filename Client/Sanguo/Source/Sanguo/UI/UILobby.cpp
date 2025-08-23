// Fill out your copyright notice in the Description page of Project Settings.


#include "UILobby.h"

#include "Kismet/GameplayStatics.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"

#include "UIManager.h"
#include "../Common/UIEvents.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Slider.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateBrush.h"
#include "Engine/CullDistanceVolume.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Components/GridSlot.h"
#include "Components/GridPanel.h"
#include "Kismet/GameplayStatics.h"

#include "config/GlobalCfg.h"
#include "config/SkillTypeCfg.h"
#include "config/LocalizationCfg.h"
#include "data/MapDataMgr.h"
#include "data/role/Player.h"
#include "data/AchieveData.h"
#include "common/Heartbeat.h"
#include "msg/MsgTeam.pb.h"
#include "msg/MsgInteraction.pb.h"

#include "UIRoundProcessBar.h"
#include "UIMsgBox.h"
#include "UIMsgBoxToast.h"

#if PLATFORM_ANDROID
#include "../JNI/java_com_wskj_thirdreich.h"
#endif

#include "../Common/DisplayMgr.h"



extern bool g_bPlaying;

bool UUILobby::Initialize()
{
	UE_LOG(LogOutputDevice, Log, TEXT("UUILobby::Initialize begin"));
	SetIsModel(false);
	if (!Super::Initialize())return false;

	//std::string strTag = "BtnUseWeaponExtern";BtnHero
	REGISTER_BTN_CLICK("BtnHero", OnClickCharacter);
	REGISTER_BTN_CLICK("BtnBag", OnClickBag);
	REGISTER_BTN_CLICK("BtnSetup", OnSetup);

	REGISTER_BTN_CLICK("BtnAttack", onBtnAtkClick);
	REGISTER_BTN_PRESSED("BtnAttack", onBtnAtkPressed);
	REGISTER_BTN_RELEASED("BtnAttack", onBtnAtkRelease);

	REGISTER_BTN_CLICK("BtnDebug", onBtnDebug);

	m_panBall = Cast<UCanvasPanel>(GetWidgetFromName(FName("LeftDownPanel")));
	m_RightPanel = Cast<UCanvasPanel>(GetWidgetFromName(FName("RightPanel")));
	m_TopRightPanel = Cast<UCanvasPanel>(GetWidgetFromName(FName("TopRightPanel")));


	m_panAtkBg = Cast<UCanvasPanel>(GetWidgetFromName(FName("AttackPanelBg")));
	m_imgCancel = Cast<UImage>(GetWidgetFromName(FName("ImgCancel")));
	m_txtCancel = Cast<UTextBlock>(GetWidgetFromName(FName("txt_cancel")));
	m_imgAchieveRed = Cast<UImage>(GetWidgetFromName(FName("ImgAchieveRed")));

#if PLATFORM_ANDROID || PLATFORM_IOS
	UButton* pBtnDebug = Cast<UButton>(GetWidgetFromName(FName("BtnDebug")));
	if (pBtnDebug)
	{
		pBtnDebug->SetVisibility(ESlateVisibility::Hidden);
	}
#endif

	UIManager::getInstance().addWidget(this);

	if (m_btnExit) {
		m_btnExit->SetVisibility(MapDataMgr::getInstance().getCurMapType() == MapDataMgr::MapType::Dungeon ?
			ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	UI_REGISTER_MYEVENT(UILayoutEvent, &UUILobby::onUILayoutEvent);

	LayoutLobby();

	return true;
}

void UUILobby::UnInit()
{
	UI_UN_REGISTER_MYEVENT(WeaponChangeEvent, this);
	UI_UN_REGISTER_MYEVENT(HorseChangeEvent, this);
	UI_UN_REGISTER_MYEVENT(SkillCoolTimeEvent, this);
	UI_UN_REGISTER_MYEVENT(WeaponPreEquipEvent, this);
}

void UUILobby::NativeDestruct()
{
	UIManager::getInstance().CloseWidget(this);
	Super::NativeDestruct();
}


void UUILobby::BeginDestroy()
{
	//UNREGISTER_EVENT(&HandlerLogin::getInstance(), HandlerLoginStateEvent);
	UIManager::getInstance().RemoveWidget(this);
	return Super::BeginDestroy();
}


void UUILobby::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	auto&& hero = AIPlayerMgr::getInstance().GetHero();
	if (!hero || !hero->m_playerData) {
		return;
	}

	static bool ssss_bLobyBang = false;
	if (!ssss_bLobyBang)
	{
		LayoutLobby();
	}
}


//void UUILobby::onBtnChallengeEvent(const HandlerLoginStateEvent& evt)
//{
//	EventUpdateCoin();
//}

void UUILobby::SendWalk(float x, float y)
{
	if (!m_panBall || !m_panBall->IsVisible()) {
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("This is a CPP Function"));
	BallEvent evt;
	evt.x = x;
	evt.y = y;
	UI_DISPATCH_MYEVENT(BallEvent , evt);
}


void UUILobby::SendSight(float value)
{
	SightEvent evt;
	evt.value = value;
	UI_DISPATCH_MYEVENT(SightEvent , evt);
}

void UUILobby::SendAttack()
{
	UE_LOG(LogOutputDevice, Log, TEXT("UUILobby::SendAttack ---------------"));
	AttackEvent evt;
	UI_DISPATCH_MYEVENT(AttackEvent, evt);
}


void UUILobby::SendDodge()
{
}

void UUILobby::SendStartContinueAttack()
{
	auto&& hero = AIPlayerMgr::getInstance().GetHero();
	if (!hero || !hero->CheckSkillConsume(0, AHero::ATK_TPY_SKILL_NORMAL, true) ||
		!hero->CanSkillAttack()) {
		return;
	}

	UE_LOG(LogOutputDevice, Log, TEXT("UUILobby::SendStartContinueAttack ---------------"));
	StartContinueAttackEvent evt;
	UI_DISPATCH_MYEVENT(StartContinueAttackEvent, evt);
}

void UUILobby::SendStopContinueAttack()
{
	UE_LOG(LogOutputDevice, Log, TEXT("UUILobby::SendStopContinueAttack ---------------"));
	StopContinueAttackEvent evt;
	UI_DISPATCH_MYEVENT(StopContinueAttackEvent, evt);
}

void UUILobby::SendAttackHover(FVector vec)
{
	HoverAttackEvent evt;
	evt.x = vec.X;
	evt.y = vec.Y;
	evt.z = vec.Z;
	evt.m_nHitTpy = GetAttackHitType();
	UI_DISPATCH_MYEVENT(HoverAttackEvent, evt);
}

void UUILobby::SendUseWeaponExtern()
{
	UseWeaponExternEvent evt;
	UI_DISPATCH_MYEVENT(UseWeaponExternEvent, evt);
}

void UUILobby::SendWeapon(int nWeaponId)
{
	WeaponEvent evt;
	evt.m_nWeaponId = nWeaponId;
	UI_DISPATCH_MYEVENT(WeaponEvent, evt);
}

void UUILobby::SetCullSetting()
{
#ifndef PLATFORM_WINDOWS
	TArray<AActor*> arrActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACullDistanceVolume::StaticClass(), arrActors);
	if (arrActors.Num() <= 0)
	{
		return;
	}
	auto aActorDis = Cast<ACullDistanceVolume>(arrActors[0]);
	if (!aActorDis)
	{
		return;
	}
	aActorDis->CullDistances.Empty(4);
	const std::vector<CullDistance>& vecDis = GlobalCfg::getInstance().GetCullDistanceVec();
	for (int i = 0 ; i < vecDis.size() ; i++)
	{
		struct FCullDistanceSizePair disPair1;
		disPair1.Size = vecDis[i].nSize;
		disPair1.CullDistance = vecDis[i].nDistance;
		aActorDis->CullDistances.Add(disPair1);
	}
#endif
}

//****************************************************************************************
//²âÊÔÓÃ
//****************************************************************************************
void UUILobby::onBtnDebug()
{
	WindowEvent evt;
	evt.m_type = WindowEvent::EventType::Window_DebugView;
	UI_DISPATCH_MYEVENT(WindowEvent, evt);
}

//****************************************************************************************
//
//****************************************************************************************
void UUILobby::onBtnAtkClick()
{
	auto&& hero = AIPlayerMgr::getInstance().GetHero();
	if (!hero || !hero->CheckSkillConsume(0, AHero::ATK_TPY_SKILL_NORMAL, true)) {
		return;
	}
	SendAttack();
}

void UUILobby::onBtnAtkPressed()
{
	AtkHover(true);
}

void UUILobby::onBtnAtkRelease()
{
	AtkHover(false);
}

FVector2D UUILobby::GetTouchPosition(int nUITpy)
{
	if (!AIPlayerMgr::getInstance().GetHero())
	{
		return FVector2D(0, 0);
	}
	return AIPlayerMgr::getInstance().GetHero()->GetTouchPosition(nUITpy);
}


FVector UUILobby::GetHeroPosition()
{
	return AIPlayerMgr::getInstance().GetHero()->GetActorLocation();
}

FVector2D UUILobby::GetHitDirPosition(int nUITpy)
{
	FVector2D vec = GetTouchPosition(nUITpy);
	float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
	if (!this->GetWorld() || !this->GetWorld()->GetGameViewport())
	{
		return vec;
	}
	int32 X = this->GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X / Scale;
	int32 Y = this->GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y / Scale;
	vec.X = (vec.X - m_posHitScroll.X) * 2;
	vec.Y = (vec.Y - m_posHitScroll.Y) * 2;
	return vec;
}


ENU_TOUCH_UITPY UUILobby::SetTouchInLobby(ETouchIndex::Type TouchIndex, FVector2D vec)
{
	ENU_TOUCH_UITPY nTouchUITpy = ENU_TOUCH_UITPY_NONE;
	if (!m_panBall) {
		return ENU_TOUCH_UITPY_NONE;
	}
	UOverlaySlot* slot = Cast<UOverlaySlot>(m_panBall->Slot);
	if (!slot) {
		return ENU_TOUCH_UITPY_NONE;
	}
	FVector2D desiredSize = m_panBall->GetDesiredSize();
	int nBang = UIManager::getInstance().GetLayoutBang();
	if (g_objDisplayMgr.IsTouchOverlaySlot(this, slot, FVector2D(vec.X - nBang, vec.Y), desiredSize)) {
		nTouchUITpy = ENU_TOUCH_UITPY_BALL;
	}
	else {
		if (GetAttackHitType() != 0) {
			UCanvasPanelSlot* slot1 = Cast<UCanvasPanelSlot>(m_panAtkBg->Slot);
			if (!slot1) {
				return ENU_TOUCH_UITPY_NONE;
			}
			if (g_objDisplayMgr.IsTouchSlot(this, slot1, FVector2D(vec.X + nBang, vec.Y))) {
				nTouchUITpy = ENU_TOUCH_UITPY_ATK;
				m_posHitScroll = vec;
			}
		}
	}
	return nTouchUITpy;
}

void UUILobby::SetTouchOutLobby(int nTouchUITpy , FVector2D vec)
{
	if (1 == nTouchUITpy) {
		BallControll(false, vec);
	}
	else if (2 == nTouchUITpy) {
		AtkControll(false, vec);
		//AIPlayerMgr::getInstance().GetHero()->StopHoverAttack();
	}
}

bool UUILobby::IsTouchImageCancel(FVector2D vec)
{
	UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(m_imgCancel->Slot);
	if (!slot) {
		return false;
	}
	int nBang = UIManager::getInstance().GetLayoutBang();
	if (g_objDisplayMgr.IsTouchSlot(this, slot, FVector2D(vec.X + nBang, vec.Y))) {
		return true;
	}
	return false;
}

void UUILobby::SetTouchCancelHover(FVector2D vec, bool bRelease)
{
	float fOpacity = 0.4f;
	if (m_imgCancel && !bRelease)
	{
		if (GetAttackHitType() == SkillEffectHitTpy::SkillEffectHitTpy_ScrollHit ||
			GetAttackHitType() == SkillEffectHitTpy::SkillEffectHitTpy_ArrowHit) {
			UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(m_imgCancel->Slot);
			int nBang = UIManager::getInstance().GetLayoutBang();
			if (slot && g_objDisplayMgr.IsTouchSlot(this, slot, FVector2D(vec.X + nBang, vec.Y))) {
				fOpacity = 1.0f;
			}
		}
		
	}
	m_imgCancel->SetOpacity(fOpacity);
}


//****************************************************************************************
//
//****************************************************************************************
int UUILobby::GetAttackHitType()
{
	return m_nAttackUITpye;
}

void UUILobby::SetAttackHitType(int nHitTpy)
{
	m_nAttackUITpye = nHitTpy;
	if (m_panAtkBg)
	{
		m_panAtkBg->SetVisibility(m_nAttackUITpye != 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::SelfHitTestInvisible);
	}
}

//****************************************************************************************
//
//***************************************************************************************
void UUILobby::OnClickCharacter()
{
	WindowEvent evt;
	evt.m_type = WindowEvent::EventType::Window_CharaterInfo;
	UI_DISPATCH_MYEVENT(WindowEvent, evt);
}


void UUILobby::OnClickBag()
{
	WindowEvent evt;
	evt.m_type = WindowEvent::EventType::Window_Bag;
	UI_DISPATCH_MYEVENT(WindowEvent, evt);
}

void UUILobby::OnClickExit()
{
	UUIMsgBox* pMsgbox = MsgBoxMgr::getInstance().MsgBox(UUIMsgBox::MsgBoxExitDungeon_Tips, UUIMsgBox::SHOW_TEXT, __LC("UI_EXIT_TIPS"));
	if (pMsgbox) {
		pMsgbox->setCallBack(UUIMsgBox::MsgBoxExitDungeon_Tips, [this](const MsgBoxEvent& evt)
		{
			if (evt.m_nBoxType == UUIMsgBox::Confirm)
			{
				MsgTeamRequestLeaveTeam_C2S msg;
				msg.set_serialid(MapDataMgr::getInstance().getSerialID());
				GameAppExtern::sendMsg(msg.msgtype(), msg);
			}
		});
	}
}

void UUILobby::OnSetup()
{
	WindowEvent evt;
	evt.m_type = WindowEvent::EventType::Window_SetUp;
	UI_DISPATCH_MYEVENT(WindowEvent, evt);
}

void UUILobby::onUILayoutEvent(const UILayoutEvent& evt)
{
	LayoutLobby();
}

//****************************************************************************************
//
//****************************************************************************************
void UUILobby::ShowUI(bool bShow)
{
	if (m_panBall) {
		m_panBall->SetVisibility((bShow || UIManager::getInstance().IsUIShowByType(WindowEvent::EventType::Window_Bag))
			? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	if (m_RightPanel) {
		m_RightPanel->SetVisibility(bShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
	if (m_TopRightPanel) {
		m_TopRightPanel->SetVisibility(bShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}

//****************************************************************************************
//
//****************************************************************************************
void UUILobby::LayoutLobby()
{
	int nBang = UIManager::getInstance().GetLayoutBang();

	UOverlaySlot* slotBall = Cast<UOverlaySlot>(m_panBall->Slot);
	if (slotBall) {
		FMargin mar = slotBall->Padding;
		slotBall->SetPadding(FMargin(nBang, mar.Top, mar.Right, mar.Bottom));
	}

	UOverlaySlot* slotRight = Cast<UOverlaySlot>(m_RightPanel->Slot);
	if (slotRight) {
		FMargin mar = slotRight->Padding;
		slotRight->SetPadding(FMargin(mar.Left, mar.Top, nBang, mar.Bottom));
	}

	UOverlaySlot* slotRightTop = Cast<UOverlaySlot>(m_TopRightPanel->Slot);
	if (slotRightTop) {
		FMargin mar = slotRightTop->Padding;
		slotRightTop->SetPadding(FMargin(mar.Left, mar.Top, nBang, mar.Bottom));
	}
}

void UUILobby::SetLayoutBang(float fValue)
{
	static bool s_bHasBang = false;
	if (s_bHasBang)
	{
		return;
	}
	s_bHasBang = true;
	int nBang = (int)(fValue * 100);
	UIManager::getInstance().SetLayoutBang(nBang);
}