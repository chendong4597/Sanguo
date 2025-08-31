// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/LocalPlayer.h"
#include "BluePrint/UserWidget.h"
#include "BluePrint/WidgetLayoutLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/LineBatchComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

#include "GameAppExtern.h"
#include "Skill/SkillEffect.h"
#include "config/SkillTypeCfg.h"
#include "config/WeaponInfoTypeCfg.h"
#include "config/BuildingCfg.h"
#include "config/ItemCfg.h"
#include "data/NpcProtectorData.h"
#include "../Scene/MainCityLevel.h"
#include "../Scene/InstanceLevel.h"

#include "config/DefenceInfoTypeCfg.h"
#include "config/DefencePosTpyCfg.h"
#include "config/GlobalCfg.h"
#include "config/MapSettingCfg.h"
#include "config/AchieveCfg.h"

#include "common/Heartbeat.h"
#include "data/MapDataMgr.h"
#include "msg/MsgInteraction.pb.h"
#include "msg/MsgPlayer.pb.h"

#include "Monster.h"
#include "Generals.h"

#include "../UI/UILobby.h"
#include "../UI/UIManager.h"
#include "UI/UIMsgBoxToast.h"

#include "../Common/RootEventObject.h"
#include "../ReichGameInstance.h"
#include "../SanguoPlayerController.h"

#define HERO_TOUCH_NUM 3

static int g_interwalk = 0;
static NS::I64 g_heroX = 0;
static NS::I64 g_heroY = 0;
static NS::I64 g_i64RiderHorseUUID = 0;

extern UWorld* gp_UWorld;

// Sets default values
AHero::AHero()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AHero::Init()
{
	AIPlayer::Init();
}

// Called when the game starts or when spawned
void AHero::BeginPlay()
{
	Super::BeginPlay();
	if (UReichGameInstance::IsEditorMode()) return;

	m_playerData = PlayerMgr::getInstance().getSelf();
	if (m_playerData) {
		m_playerData->resetAttr();
	}

	AIPlayerMgr::getInstance().SetHero(this);

	if (m_playerData && m_playerData->getId() > 0) {
		AIPlayerMgr::getInstance().AddPlayer(this);
	}

	/*if (m_playerData && m_playerData->getGender() == 1)
	{
		SetProfessIdx(2);
	}
	else {
		SetProfessIdx(0);
	}*/

	//UE_LOG(LogOutputDevice, Log, TEXT("AHero::BeginPlay"));
	//SetCamp(CampState::CAMP_HERO);
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AHero::MoveToTouchLocation);
	UI_REGISTER_MYEVENT(BallEvent , &AHero::onWalkEvent);
	UI_REGISTER_MYEVENT(AttackEvent, &AHero::onAttackEvent);
	UI_REGISTER_MYEVENT(WindowEvent, &AHero::onWindowEvent);
	UI_REGISTER_MYEVENT(SkillEvent, &AHero::onSkillEvent);

	//RG_REGISTER_MYEVENT(HandlerItemEvent, &AHero::onItemEvent);
	//RG_REGISTER_MYEVENT(HandlerTipsEvent, &AHero::onHandlerTipsEvent);
	//RG_REGISTER_MYEVENT(HandlerItemGetEvent, &AHero::onHandlerItemGetEvent);
	//RG_REGISTER_MYEVENT(HandlerNetEvent, &AHero::onHandlerNetEvent);
	//RG_REGISTER_MYEVENT(HandlerHeroAppearanceEvent, &AHero::onHandlerHeroAppearanceEvent);
	//RG_REGISTER_MYEVENT(HandlerHeroAttrChangeEvent, &AHero::onHandlerHeroAttrChangeEvent);
	//RG_REGISTER_MYEVENT(HandlerSoundEvent, &AHero::onHandlerSoundEvent);
	//RG_REGISTER_MYEVENT(MsgDeleteAccountS2C, &AHero::onMsgDeleteAccountS2C);

	TouchHis his1;
	m_mapTouchHis[ETouchIndex::Type::Touch1] = his1;
	TouchHis his2;
	m_mapTouchHis[ETouchIndex::Type::Touch2] = his2;
	TouchHis his3;
	m_mapTouchHis[ETouchIndex::Type::Touch3] = his3;
	m_fPreSightValue = 0.f;
	m_fPreSightValueRecord = 0.f;
	m_nProfessionIdx = 0;

	//LoadSightInfo();
	//if (IsInMainCity()) {
	//	ResetTouchSight(m_nTouchSightTpy);
	//}
	//else {
	//	ResetTouchSight(TouchSightTpy_B, false);
	//}
	

	//GameAppExtern::requestEnterMap();

	//g_heroX = GetActorLocation().X * 1000000;
	//g_heroY = GetActorLocation().Y * 1000000;

	RefreshEquipAttr();   //属性计算

	//EvtPreGetGenByff();

	HeroInitEvent evt;
	UI_DISPATCH_MYEVENT(HeroInitEvent, evt);

	//if (IsInMainCity() && m_playerData->getHp() <= 0) {
	//	Death();
	//}
}

// Called every frame
void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if	PLATFORM_WINDOWS
	if (m_bAround) {
		Around(DeltaTime);
	}
#endif
	if (m_ArrowActor && !m_ArrowActor->IsHidden())
	{
		m_ArrowActor->SetActorLocation(GetActorLocation());
	}
}


void AHero::Around(float DeltaTime)
{
	FVector2D vec = GetTouchPosition(0);
	if (vec.IsZero())
	{
		return;
	}
	PostUITouchEvent(vec.X, vec.Y, 1);
	if (m_nTouchSightTpy != TouchSightTpy_A
		|| UIManager::getInstance().IsShowUI()) {
		return;
	}
	
	if (vec.X > m_MoveRoundVec.X)
	{
		Round(true, DeltaTime*std::abs(m_MoveRoundVec.X - vec.X));
	}
	else if (vec.X < m_MoveRoundVec.X) {
		Round(false, DeltaTime*std::abs(m_MoveRoundVec.X - vec.X));
	}

	if (vec.Y > m_MoveRoundVec.Y)
	{
		UpAndDown(true, DeltaTime*std::abs(m_MoveRoundVec.Y - vec.Y));
	}
	else if (vec.Y < m_MoveRoundVec.Y) {
		UpAndDown(false, DeltaTime*std::abs(m_MoveRoundVec.Y - vec.Y));
	}
	m_MoveRoundVec = vec;
}


//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
FVector2D AHero::GetTouchPosition(int nUITpy)
{
#if	PLATFORM_WINDOWS
	return UWidgetLayoutLibrary::GetMousePositionOnViewport(gp_UWorld);
#else
	for (int i = 0; i < HERO_TOUCH_NUM; i++)
	{
		if (m_mapTouchHis[(ETouchIndex::Type)i].bTouch && nUITpy == (int)m_mapTouchHis[(ETouchIndex::Type)i].nTouchUITpy)
		{
			return m_mapTouchHis[(ETouchIndex::Type)i].vec;
		}
	}
	FVector2D vec;
	return vec;
#endif
}

// Called to bind functionality to input
void AHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHero::SetTouchSightRecord(int nSight)
{
	auto&& stuSight = GlobalCfg::getInstance().GetSightInfo();
	if (stuSight.nMax - stuSight.nMin > 0) {
		m_fPreSightValueRecord = (nSight - stuSight.nMin)*1.f / stuSight.nMax;
		m_fPreSightValueRecord = m_fPreSightValueRecord < 0.f ? 0.f : m_fPreSightValueRecord > 1.f ? 1.f : m_fPreSightValueRecord;
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onWalkEvent(const BallEvent& evt)
{
	ResetWaitTrans();
	FVector vec(-evt.y, evt.x , 0);
	BPWalk(vec);
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::Walk(FVector vec, float fScale)
{
	AGenerals* pGen = Cast<AGenerals>(GetAttachParentActor());
	if (pGen)
	{
		FVector CharacterLocation = GetActorLocation();

		// 获取摄像机位置
		APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector CameraLocation = CameraManager->GetCameraLocation();

		// 计算从摄像机到主角的方向
		FVector Direction = (CharacterLocation - CameraLocation).GetSafeNormal();

		FRotator rot1 = CameraManager->GetCameraRotation();

		FVector vecDir = vec;// +rot1.Vector();
		FRotator Rota = vecDir.Rotation();
		//Rota.Yaw += rot1.Yaw;
		vecDir = Rota.Vector();

		pGen->AddMovementInput(vecDir, fScale);

		//UActorComponent* pComp = this->GetComponentByClass(USpringArmComponent::StaticClass());
		//if (pComp) {
		//	FRotator rRot = UKismetMathLibrary::FindLookAtRotation(pComp->, this);
		//}
		
		//Rota.Yaw = 0;
		//Rota.Pitch = 0;
		pGen->SetActorRotation(Rota);
	}
}

void AHero::PostHudShow()
{
	static NS::I64 sss_n64ShowTm = 0;
	if (Heartbeat::getInstance().getServerTimeMs() - sss_n64ShowTm <= 500)
	{
		return;
	}
	sss_n64ShowTm = Heartbeat::getInstance().getServerTimeMs();
	FVector pos = this->GetActorLocation();
	HeroWalkEvent evt;
	evt.m_nX = pos.X;
	evt.m_nY = pos.Y;
	evt.m_nZ = pos.Z;
	UI_DISPATCH_MYEVENT(HeroWalkEvent, evt);
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onSightEvent(const SightEvent& evt)
{
	float value = evt.value;
	auto&& stuSight = GlobalCfg::getInstance().GetSightInfo();

	Sight(stuSight.nMin + value*(stuSight.nMax - stuSight.nMin));
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onAttackEvent(const AttackEvent& evt)
{
	if (!IsCanAttack()) {
		return;
	}
	ResetWaitTrans();
	MsgInteractionC2S msg;
	msg.set_objid(m_playerData->getId());
	msg.set_player_iggid(m_playerData->getIggId());
	msg.set_action_type(ACTION_TYPE_ATTACK);
	GameAppExtern::sendMsg(msg.msgtype(), msg);
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onUseWeaponExternEvent(const UseWeaponExternEvent& evt)
{
	if (IsDeath()) return;
	ResetWaitTrans();
}

//****************************************************************************************
// 激励视频
//****************************************************************************************
void AHero::onUITapLoginEvent(const UITapLoginEvent& evt)
{
	if (evt.m_type != UITapLoginEvent::EventType::EventType_Video)
	{
		return;
	}
	UE_LOG(LogOutputDevice, Log, TEXT("AHero::onUITapLoginEvent"));
	MsgPlayerADNVideoAwardC2S msg;
	GameAppExtern::sendMsg(msg.msgtype(), msg);
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onWeaponEvent(const WeaponEvent& evt)
{
	if (evt.m_nWeaponId <= 0 || evt.m_nWeaponId > 7)
	{
		return;
	}
	int nWeaponTpy = 0;
	NS::I64 wUuid = GetEquipWeapon(evt.m_nWeaponId);
	if (wUuid != 0) {
		auto&& itemData = ItemPacketMgr::getInstance().findItemDataByUuid(wUuid);
		if (itemData && itemData->getPacketType() == MsgPB::PACKET_TYPE::BODY)
		{
			if (itemData->getLevel() > m_playerData->getLevel()) {
				MsgBoxToastMgr::getInstance().ShowText(__LC("UI_TIPS_LEVEL_LIMIT"));
				return;
			}
			GameAppExtern::requestItemEquip(itemData->getUuid());
		}
		else {
			WindowEvent evtUI;
			evtUI.m_type = WindowEvent::EventType::Window_OperaSetUp;
			UI_DISPATCH_MYEVENT(WindowEvent, evtUI);
		}
	} else {
		WindowEvent evtUI;
		evtUI.m_type = WindowEvent::EventType::Window_OperaSetUp;
		UI_DISPATCH_MYEVENT(WindowEvent, evtUI);
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onBuildEvent(const BuildEvent& evt)
{
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onWindowEvent(const WindowEvent& evt)
{
	UE_LOG(LogOutputDevice, Log, TEXT("UIManager::ShowUI AHero::onWindowEvent = %d"), evt.m_type);
	UIManager::getInstance().onWindowEvent(evt);
}


//****************************************************************************************
//
//****************************************************************************************
void AHero::onStartContinueAttackEvent(const StartContinueAttackEvent& evt)
{
	if (IsDeath()) return;
	ResetWaitTrans();
	auto&& pWeaponCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pWeaponCfg)
	{
		return;
	}
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(pWeaponCfg->n_SkillId);
	if (!pCfg)
	{
		return;
	}
	if (pCfg->n_ScrollHit == SkillEffectHitTpy_ScrollHit)
	{
		m_bScrollHit = true;
	}
	else if (pCfg->n_ScrollHit == SkillEffectHitTpy_ArrowHit) {
		m_bScrollHit = true;
	}
	else if(pCfg->n_AniTpy == ENU_ATK_ANI_TPY_CONTINUE){
		MsgInteractionC2S msg;
		msg.set_objid(m_playerData->getId());
		msg.set_player_iggid(m_playerData->getIggId());
		msg.set_action_type(ACTION_TYPE_CONTINUE_S_START);
		GameAppExtern::sendMsg(msg.msgtype(), msg);
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onStopContinueAttackEvent(const StopContinueAttackEvent& evt)
{
	ResetWaitTrans();
	StopHoverAttack();
}

void AHero::StopHoverAttack(bool bRelease)
{
	if (m_bScrollHit && m_GroundActor) {
		auto&& pWeaponCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
		if (!pWeaponCfg)
		{
			return;
		}
		auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(pWeaponCfg->n_SkillId);
		if (!pCfg)
		{
			return;
		}
		if (bRelease) {
			if (pCfg->n_ScrollHit == SkillEffectHitTpy_ScrollHit)
			{
				m_vecScrollPos = m_GroundActor->GetActorLocation();
				
				if (CheckSkillConsume(0, ATK_TPY_SKILL_NORMAL, true)) {

					auto toRota = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), m_vecScrollPos);
					FRotator NewRotation(GetActorRotation().Pitch, toRota.Yaw, GetActorRotation().Roll);
					SetActorRotation(NewRotation);

					{
						MsgInteractionTurnC2S msg;
						msg.set_objid(m_playerData->getId());
						msg.set_player_iggid(m_playerData->getIggId());
						auto& R = *msg.mutable_r();
						R.set_p(NewRotation.Pitch * 1000000);
						R.set_y(NewRotation.Yaw * 1000000);
						R.set_r(NewRotation.Roll * 1000000);
						GameAppExtern::sendMsg(msg.msgtype(), msg);
					}

					{
						MsgInteractionC2S msg;
						msg.set_objid(m_playerData->getId());
						msg.set_player_iggid(m_playerData->getIggId());
						msg.set_action_type(ACTION_TYPE_SKILL_P_ATTACK);
						msg.set_data1(m_vecScrollPos.X * 10000);
						msg.set_data2(m_vecScrollPos.Y * 10000);
						msg.set_data3(m_vecScrollPos.Z * 10000);
						GameAppExtern::sendMsg(msg.msgtype(), msg);
					}
				}
			}
			else if (pCfg->n_ScrollHit == SkillEffectHitTpy_ArrowHit) {
				
				m_rotScroll = m_ArrowActor->GetActorForwardVector();

				if (CheckSkillConsume(0, ATK_TPY_SKILL_NORMAL, true)) {
					auto toRota = m_ArrowActor->GetActorRotation();
					FRotator NewRotation(GetActorRotation().Pitch, toRota.Yaw, GetActorRotation().Roll);
					SetActorRotation(NewRotation);

					{
						MsgInteractionTurnC2S msg;
						msg.set_objid(m_playerData->getId());
						msg.set_player_iggid(m_playerData->getIggId());
						auto& R = *msg.mutable_r();
						R.set_p(NewRotation.Pitch * 1000000);
						R.set_y(NewRotation.Yaw * 1000000);
						R.set_r(NewRotation.Roll * 1000000);
						GameAppExtern::sendMsg(msg.msgtype(), msg);
					}

					{
						MsgInteractionC2S msg;
						msg.set_objid(m_playerData->getId());
						msg.set_player_iggid(m_playerData->getIggId());
						msg.set_action_type(ACTION_TYPE_SKILL_F_ATTACK);
						msg.set_data1(m_rotScroll.X * 10000);
						msg.set_data2(m_rotScroll.Y * 10000);
						msg.set_data3(m_rotScroll.Z * 10000);
						GameAppExtern::sendMsg(msg.msgtype(), msg);
					}
				}
			}
		}
	}

	auto&& pWeaponCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (pWeaponCfg)
	{
		auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(pWeaponCfg->n_SkillId);
		if (pCfg && pCfg->n_AniTpy == 1 && bRelease && GetContinueIdx() != 0)
		{
			MsgInteractionC2S msg;
			msg.set_objid(m_playerData->getId());
			msg.set_player_iggid(m_playerData->getIggId());
			msg.set_action_type(ACTION_TYPE_CONTINUE_S_END);
			GameAppExtern::sendMsg(msg.msgtype(), msg);
		}
	}
	

	m_bScrollHit = false;
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::onHoveAttackEvent(const HoverAttackEvent& evt)
{
	if (IsDeath()) return;
	ResetWaitTrans();
	FVector vec;
	vec.X = evt.x;
	vec.Y = evt.y;
	vec.Z = evt.z;
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::SetHeroFocusRole(ARole* aRole)
{
	if (GetFocusRole() == aRole) {
		return;
	}
	if (aRole == NULL)
	{
		GetFocusRole()->ShowHighLightMesh(false);
		SetFocusRole(NULL);
		return;
	}
	SetFocusRole(aRole);
	aRole->ShowHighLightMesh(true);
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::GetNearstGens(NS::I64 uuid, std::vector<NS::I64>& vecUuids)
{
	vecUuids.push_back(uuid);
	auto&& generals = AGeneralsMgr::getInstance().GetMapGenerals();
	for (auto&& gen : generals)
	{
		AGenerals* pRole = Cast<AGenerals>(gen.second);
		if (!pRole || FVector::Distance(GetActorLocation(), pRole->GetActorLocation()) > 1000.f) {
			continue;
		}
		if (pRole->GetProtectorUuid() == uuid) {
			continue;
		}
		vecUuids.push_back(pRole->GetProtectorUuid());
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::ResetCamera()
{
	UActorComponent* pComp = this->GetComponentByClass(USpringArmComponent::StaticClass());
	if (!pComp) {
		return;
	}
	USpringArmComponent* pArmComp = Cast<USpringArmComponent>(pComp);
	if (!pArmComp)
	{
		return;
	}
	Sight(m_nPreSpringLen);
	pArmComp->SetRelativeRotation(m_preSpringRota);
}


//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
float AHero::GetTouchDistance()
{
	FVector2D vec(0 , 0);
	for (int i = 0; i < HERO_TOUCH_NUM; i++)
	{
		if (m_mapTouchHis[(ETouchIndex::Type)i].bTouch && ENU_TOUCH_UITPY_NONE == m_mapTouchHis[(ETouchIndex::Type)i].nTouchUITpy)
		{
			if (vec.IsZero())
			{
				vec = m_mapTouchHis[(ETouchIndex::Type)i].vec;
			}
			else
			{
				return FVector2D::Distance(vec, m_mapTouchHis[(ETouchIndex::Type)i].vec);
			}
		}
	}
	return 0.f;
}

//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
void AHero::HeroInputTouch(const ETouchIndex::Type TouchIndex, ETouchType::Type nTouchType, FVector oriVec)
{
	if (TouchIndex > ETouchIndex::Type::Touch2)
	{
		return;
	}

	if (nTouchType == ETouchType::Ended) {
		UE_LOG(LogOutputDevice, Log, TEXT("AHero::SetTouchPosition = finidx = %d nTouchTpy  = %d, x = %0.2f , y = %0.2f , z = %0.2f "),
			TouchIndex, nTouchType, oriVec.X, oriVec.Y, oriVec.Z);
	}

	float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
	FVector2D vec(oriVec.X / Scale, oriVec.Y / Scale);

/////////////////////种植模式/////////////////////////////////////////////////////////////////////////

/////////////////////人物模式/////////////////////////////////////////////////////////////////////////
	UUserWidget* pWidget = UIManager::getInstance().GetWidget(UI_TPY_Lobby);
	if (!pWidget)
	{
		return;
	}
	UUILobby* pLobby = Cast<UUILobby>(pWidget);
	if (!pLobby)
	{
		return;
	}

	static FVector2D s_vecBegin;
	if (nTouchType == ETouchType::Type::Began) {
		SightAround(TouchIndex , vec, true);
		if (TouchIndex == ETouchIndex::Type::Touch1) {
			s_vecBegin = vec;
			UE_LOG(LogOutputDevice, Log, 
				TEXT("HeroInputTouch Touch1 Begin oriVec.X = %0.2f oriVec.Y = %0.2f Scale = 0.2f"), oriVec.X, oriVec.Y, Scale);
		}
		else if (TouchIndex == ETouchIndex::Type::Touch2) {
			m_fPreSightValue = m_fPreSightValueRecord;
			UE_LOG(LogOutputDevice, Log, TEXT("HeroInputTouch Touch2 Begin"));
		}
	}
	else if (nTouchType == ETouchType::Moved) {
		if (TouchIndex == ETouchIndex::Type::Touch1) {
			UE_LOG(LogOutputDevice, Log, TEXT("HeroInputTouch Touch1 Move"));
		}
		else if (TouchIndex == ETouchIndex::Type::Touch2) {
			UE_LOG(LogOutputDevice, Log, TEXT("HeroInputTouch Touch2 Move"));
		}

		if (m_mapTouchHis[TouchIndex].bTouch == false ) {
#if PLATFORM_IOS
			if (!UIManager::getInstance().IsShowUI()) {
				SightAround(TouchIndex, s_vecBegin, true);
			}
#else
			SightAround(TouchIndex, s_vecBegin, true);
#endif
		}

		m_mapTouchHis[TouchIndex].vec = vec;
		if (!m_bAround && m_fTouchDistance > 0.000001f && GetTouchDistance() > 0.f)
		{
			float value = GetTouchDistance() / m_fTouchDistance;
			value = 1.f - value;
			value += m_fPreSightValue;
			value = value > 1.0f ? 1.0f : value < 0.0000001f ? 0.0000001f : value;
			auto&& stuSight = GlobalCfg::getInstance().GetSightInfo();
			Sight(stuSight.nMin + value * (stuSight.nMax - stuSight.nMin));
		}
		else if (m_bAround) {
			Around(0.01f);
		} else if(m_bScrollHit){
			pLobby->HitSkillControll();
		}
		if (m_mapTouchHis[TouchIndex].nTouchUITpy == ENU_TOUCH_UITPY_ATK)
		{
			pLobby->SetTouchCancelHover(FVector2D(vec.X , vec.Y));
		}
	}
	else if (nTouchType == ETouchType::Type::Ended) {
		SightAround(TouchIndex , vec, false);
		if (TouchIndex == ETouchIndex::Type::Touch1) {
			UE_LOG(LogOutputDevice, Log, TEXT("HeroInputTouch Touch1 End"));
		}
		else if (TouchIndex == ETouchIndex::Type::Touch2) {
			UE_LOG(LogOutputDevice, Log, TEXT("HeroInputTouch Touch2 End"));
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::PostUITouchEvent(float X, float Y, int touchTpy)
{
	UITouchEvent evt;
	evt.m_nX = X;
	evt.m_nY = Y;
	evt.m_type = UITouchEvent::TouchEvent_Tpy(touchTpy);
	UI_DISPATCH_MYEVENT(UITouchEvent, evt);
}

//****************************************************************************************
//
//****************************************************************************************
void AHero::SightAround(ETouchIndex::Type TouchIndex , FVector2D vec, bool bPressed)
{
/////////////////////////////////种植模式///////////////////////////////////////////////////////////////////
	PostUITouchEvent(vec.X, vec.Y, bPressed ? 0 : 2);

/////////////////////////////////主界面模式///////////////////////////////////////////////////////////////////
	UUserWidget* pWidget = UIManager::getInstance().GetWidget(UI_TPY_Lobby);
	if (!pWidget)
	{
		return;
	}
	UUILobby* pLobby = Cast<UUILobby>(pWidget);
	if (!pLobby)
	{
		return;
	}

#if PLATFORM_WINDOWS
	if (bPressed) {
		ENU_TOUCH_UITPY nTouchUITpy = pLobby->SetTouchInLobby(TouchIndex, vec);
		if (nTouchUITpy > ENU_TOUCH_UITPY_NONE)
		{
			if (nTouchUITpy == ENU_TOUCH_UITPY_BALL) {
				pLobby->BallControll(true, vec);
			}
			else if (nTouchUITpy == ENU_TOUCH_UITPY_ATK) {
				if (!UIManager::getInstance().IsShowUI()) {
					pLobby->AtkControll(true, vec);
					if (pLobby->GetAttackHitType() == SkillEffectHitTpy_ScrollHit)
					{
						m_bScrollHit = true;
					}
					else if (pLobby->GetAttackHitType() == SkillEffectHitTpy_ArrowHit) {
						m_bScrollHit = true;
					}
				}
			}
		}
		else {
			m_MoveRoundVec = vec;
			m_bAround = true;
		}
	}
	else {
		pLobby->SetTouchOutLobby(1, vec);
		pLobby->SetTouchOutLobby(2, vec);
		StopHoverAttack(!pLobby->IsTouchImageCancel(FVector2D(vec.X , vec.Y)));
		pLobby->SetTouchCancelHover(FVector2D(vec.X, vec.Y) , true);
		m_bAround = false;
		SaveHeroCameraInfo();
	}
#else
	if (bPressed) {
		ENU_TOUCH_UITPY nTouchUITpy = pLobby->SetTouchInLobby(TouchIndex, vec);
		m_mapTouchHis[TouchIndex].bTouch = true;
		m_mapTouchHis[TouchIndex].vec.X = vec.X;
		m_mapTouchHis[TouchIndex].vec.Y = vec.Y;
		m_mapTouchHis[TouchIndex].nTouchUITpy = nTouchUITpy;

		if (nTouchUITpy == ENU_TOUCH_UITPY_NONE)
		{
			for (int i = 0; i < HERO_TOUCH_NUM; i++)
			{
				if (i == TouchIndex)
				{
					continue;
				}
				if (m_mapTouchHis[(ETouchIndex::Type)i].bTouch && m_mapTouchHis[(ETouchIndex::Type)i].nTouchUITpy == ENU_TOUCH_UITPY_NONE)
				{
					m_fTouchDistance = FVector2D::Distance(m_mapTouchHis[(ETouchIndex::Type)i].vec, vec);
				}
			}
			if (m_fTouchDistance <= 0.f)
			{
				m_MoveRoundVec = vec;
				m_bAround = true;
			}
			else {
				m_bAround = false;
			}
		}
		else if (nTouchUITpy == ENU_TOUCH_UITPY_BALL) {
			if (!UIManager::getInstance().IsShowUI() ||
				UIManager::getInstance().IsUIShowByType(WindowEvent::EventType::Window_Bag)) {
				pLobby->BallControll(true, vec);
			}
		}
		else if (nTouchUITpy == ENU_TOUCH_UITPY_ATK) {
			if (!UIManager::getInstance().IsShowUI()) {
				pLobby->AtkControll(true, vec);
				m_bScrollHit = true;
				if (pLobby->GetAttackHitType() == SkillEffectHitTpy_ScrollHit)
				{
					m_bScrollHit = true;
				}
				else if (pLobby->GetAttackHitType() == SkillEffectHitTpy_ArrowHit) {
					m_bScrollHit = true;
				}
			}
		}
	}
	else {
		pLobby->SetTouchOutLobby(m_mapTouchHis[TouchIndex].nTouchUITpy, vec);
		if (m_mapTouchHis[TouchIndex].bTouch)
		{
			if (m_mapTouchHis[TouchIndex].nTouchUITpy == ENU_TOUCH_UITPY_NONE)
			{
				m_fTouchDistance = 0.f;
				m_bAround = false;
			}
			else if (m_mapTouchHis[TouchIndex].nTouchUITpy == ENU_TOUCH_UITPY_ATK) {
				StopHoverAttack(!pLobby->IsTouchImageCancel(FVector2D(vec.X, vec.Y)));
				pLobby->SetTouchCancelHover(FVector2D(vec.X, vec.Y) , true);
			}
		}
		m_mapTouchHis[TouchIndex].bTouch = false;
		m_mapTouchHis[TouchIndex].nTouchUITpy = ENU_TOUCH_UITPY_NONE;
		SaveHeroCameraInfo();
	}
#endif
}


//****************************************************************************************
//
//****************************************************************************************
void AHero::SetEquipWeapon(int nPos, NS::I64 uuid, bool bEquip)
{
	if (nPos <= 0 || nPos > 7) {
		return;
	}
	if (bEquip) {
		auto iter = m_mapEquipWeapon.begin();
		for (;iter != m_mapEquipWeapon.end(); iter++)
		{
			if (iter->second == uuid) {
				iter->second = 0;
			}
		}
		m_mapEquipWeapon[nPos] = uuid;
	} else {
		m_mapEquipWeapon[nPos] = 0;
	}
	SavePreEquipWeapons();
	WeaponPreEquipEvent evt;
	UI_DISPATCH_MYEVENT(WeaponPreEquipEvent, evt);
}

NS::I64 AHero::GetEquipWeapon(int nPos)
{
	auto&& iter = m_mapEquipWeapon.find(nPos);
	if (iter != m_mapEquipWeapon.end())
	{
		return iter->second;
	}
	return 0;
}

void AHero::InitPreEquipWeapons()
{
	if (!m_playerData) {
		return;
	}

	FString strData = ReadHeroCfgFile("preweapon.ini");
	std::string strWeapons(TCHAR_TO_UTF8(*strData));

	std::vector<std::string> vecList;
	GameAppExtern::SplitVecString(vecList, strWeapons, ",");
	for (int i = 0 ; i < vecList.size(); i++)
	{
		std::vector<std::string> vecE;
		GameAppExtern::SplitVecString(vecE, vecList[i], "|");
		if (vecE.size() == 2)
		{
			NS::I64 uuid = atoll(vecE[1].c_str());
			if (uuid == 0 || !ItemPacketMgr::getInstance().findItemDataByUuid(uuid))
			{
				continue;
			}
			m_mapEquipWeapon[atoi(vecE[0].c_str())] = uuid;
		}
	}
	WeaponPreEquipEvent evt;
	UI_DISPATCH_MYEVENT(WeaponPreEquipEvent, evt);
}

void AHero::SavePreEquipWeapons()
{
	if (!m_playerData) {
		return;
	}

	std::string strSave;
	auto iter = m_mapEquipWeapon.begin();
	for (;iter != m_mapEquipWeapon.end(); iter++)
	{
		if (iter->second != 0) {
			TArray<FStringFormatArg> Args;
			Args.Add(FStringFormatArg(iter->first));  // {0}
			Args.Add(FStringFormatArg(iter->second)); // {1}

			// 格式化字符串
			FString StrF = FString::Format(TEXT("{0}|{1},"), Args);
			strSave += TCHAR_TO_UTF8(*StrF);
		}
	}

	SaveHeroCfgFile("preweapon.ini", strSave.c_str());
}


//****************************************************************************************
//
//****************************************************************************************
void AHero::SetHeroCamera()
{
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (OurPlayerController)
	{
		OurPlayerController->SetViewTarget(this);
	}
}

void AHero::CityAround(float DeltaTime)
{
	if (!m_pCityCamera)
	{
		return;
	}
	FVector2D vec = GetTouchPosition(3);
	if (vec.IsZero())
	{
		return;
	}
	auto loc = m_pCityCamera->GetActorLocation();
	loc.X += -DeltaTime * (m_MoveRoundVec.Y - vec.Y)*300;
	loc.Y += DeltaTime * (m_MoveRoundVec.X - vec.X)*300;
	m_pCityCamera->SetActorLocation(loc);
	m_MoveRoundVec = vec;
}


void AHero::onHorseEvent(const HorseEvent& evt)
{
	if (IsDeath()) return;
	static int s_horseId = 1310081;
	if (!m_playerData) return;
	MsgInteractionC2S msg;
	msg.set_objid(m_playerData->getId());
	msg.set_player_iggid(m_playerData->getIggId());
	msg.set_action_type(ACTION_TYPE_HORSE);
	msg.set_data1(s_horseId);
	GameAppExtern::sendMsg(msg.msgtype(), msg);
	//RideHorse(s_horseId);
	s_horseId++;
	if (s_horseId > 1310094)
	{
		s_horseId = 1310081;
	}
}

void AHero::onProfessEvent(const ProfessEvent& evt)
{
	SetProfessIdx(m_nProfessionIdx++);
	if (m_nProfessionIdx >= 4) {
		m_nProfessionIdx = 0;
	}
}

void AHero::onSkillEvent(const SkillEvent& evt)
{
	if (IsDeath()) return;
	ResetWaitTrans();
	MsgInteractionC2S msg;
	msg.set_objid(m_playerData->getId());
	msg.set_player_iggid(m_playerData->getIggId());
	msg.set_action_type(ACTION_TYPE_SKILL_ATTACK);
	msg.set_data1(evt.m_nSkillId);
	GameAppExtern::sendMsg(msg.msgtype(), msg);
}

void AHero::onHorseSkillEvent(const HorseSkillEvent& evt)
{
	ResetWaitTrans();
	MsgInteractionC2S msg;
	msg.set_objid(m_playerData->getId());
	msg.set_player_iggid(m_playerData->getIggId());
	msg.set_action_type(ACTION_TYPE_HORSE_SKILL);
	GameAppExtern::sendMsg(msg.msgtype(), msg);
}

void AHero::ResetTouchSight(int nSightTpy, bool bSetSight)
{
	int nPreSight = m_nTouchSightTpy;
	if (nSightTpy != -1 && bSetSight) {
		m_nTouchSightTpy = nSightTpy;
	}
	FString LevelName = this->GetWorld()->GetCurrentLevel()->GetOutermost()->GetName();
	FString CleanLevelName = FPaths::GetCleanFilename(LevelName);
	auto&& pSight = GlobalCfg::getInstance().GetSightInfo(nSightTpy, CleanLevelName.Contains("_city"));
	if (!pSight)
	{
		return;
	}

	UActorComponent* pComp = this->GetComponentByClass(USpringArmComponent::StaticClass());
	if (!pComp) {
		return;
	}
	USpringArmComponent* pArmComp = Cast<USpringArmComponent>(pComp);
	if (!pArmComp)
	{
		return;
	}
	
	auto rota = pArmComp->GetRelativeRotation();
	if (nPreSight == TouchSightTpy_A) {
		m_nPreSpringLen = pArmComp->TargetArmLength;
		m_preSpringRota = rota;
	}
	
	Sight(pSight->nArmLen);
	rota.Roll = pSight->nRoll;
	rota.Pitch = pSight->nPitch;
	rota.Yaw = pSight->nYaw;
	pArmComp->SetRelativeRotation(rota);
	if (nSightTpy != -1) {
		SaveSightInfo();
	}
}

void AHero::LoadSightInfo()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString TempFolder = FPaths::ProjectSavedDir();
	FString TempFileName = TempFolder + TEXT("touchsight.ini");

	FString strData;
	FFileHelper::LoadFileToString(strData, *TempFileName);
	if (!strData.IsEmpty()) {
		m_nTouchSightTpy = FCString::Atoi(*strData);
	}
}

void AHero::SaveSightInfo()
{
	FString strData = FString::Printf(TEXT("%d"), m_nTouchSightTpy);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString TempFolder = FPaths::ProjectSavedDir();
	FString TempFileName = TempFolder + TEXT("touchsight.ini");

	// 创建临时文件夹
	PlatformFile.CreateDirectory(*TempFolder);
	FFileHelper::SaveStringToFile(strData, *TempFileName);
}

FString AHero::ReadHeroCfgFile(FString strFile)
{
	if (!m_playerData) {
		return "";
	}

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString strFonder = FString::Printf(TEXT("Hero_%lld/"), m_playerData->getIggId());
	FString TempFolder = FPaths::ProjectSavedDir() + *strFonder;
	FString TempFileName = TempFolder + strFile;

	FString strData;
	FFileHelper::LoadFileToString(strData, *TempFileName);
	return strData;
}

void AHero::SaveHeroCfgFile(FString strFile, FString strData)
{
	if (!m_playerData) {
		return;
	}

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString strFonder = FString::Printf(TEXT("Hero_%lld/"), m_playerData->getIggId());
	FString TempFolder = FPaths::ProjectSavedDir() + *strFonder;
	FString TempFileName = TempFolder + strFile;

	// 创建临时文件夹
	PlatformFile.CreateDirectory(*TempFolder);
	FFileHelper::SaveStringToFile(strData, *TempFileName);
}


//****************************************************************************************
//
//****************************************************************************************
void AHero::onHandlerTipsEvent(const HandlerTipsEvent& evt)
{
	if (evt.m_strtips.empty() || evt.m_type != HandlerTipsEvent::EventType::Unknown) {
		return;
	}
	MsgBoxToastMgr::getInstance().ShowText(evt.m_strtips.c_str());
}


void AHero::onHandlerItemGetEvent(const HandlerItemGetEvent& evt)
{
	if (evt.m_itemUuid != 0 && IsInMainCity()) {
	}
}

void AHero::onHandlerNetEvent(const HandlerNetEvent& evt)
{
	if (evt.m_type == HandlerNetEvent::EventType::NET_BREAK ||
		evt.m_type == HandlerNetEvent::EventType::NET_FAILED) {
	}
}

void AHero::onHandlerHeroAppearanceEvent(const HandlerHeroAppearanceEvent& evt)
{
	if (m_playerData->getId() > 0) {
		AIPlayerMgr::getInstance().AddPlayer(this);
	}
}

void AHero::onHandlerHeroAttrChangeEvent(const HandlerHeroAttrChangeEvent& evt)
{
}

void AHero::onItemEvent(const HandlerItemEvent& evt)
{
	ResetWeapon();
	ResetHorse();
	auto&& itemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::EQUIPMENT, EQUIP_PACKET_HORSE);
	if (itemData) {
		g_i64RiderHorseUUID = itemData->getUuid();
	}
	RefreshEquipAttr();
	ResetAttr();
}

void AHero::onHandlerSoundEvent(const HandlerSoundEvent& evt)
{
	std::string strSound = "";

	switch (evt.m_type) {
	case HandlerSoundEvent::EventType::Evt_Equip:
		strSound = "SoundWave'/Game/Sound/effect/clock.clock'";
		break;
	case HandlerSoundEvent::EventType::Evt_ItemMan:
		strSound = "SoundWave'/Game/Sound/effect/money04.money04'";
		break;
	default:
		break;
	}

	if (!strSound.empty()) {
		FString strPath = strSound.c_str();
		UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
		if (soundObj && gp_UWorld)
		{
			UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
		}
	}
}

void AHero::RefreshEquipAttr()
{
	if (!m_playerData) {
		return;
	}
	Role::EquipMap mapEquip;
	for (int pos = MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_HEAD; pos < MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_POSITION_MAX; pos++)
	{
		auto pItemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::EQUIPMENT, pos);
		if (pItemData) {
			mapEquip[pos] = pItemData;
		}
	}
	m_playerData->setEquipMap(mapEquip);
	m_playerData->calcAttr(true);
}


//***************************************************************************************************************************************************
//
//***************************************************************************************************************************************************
void AHero::InitCurrencyItems()
{
	FString strData = ReadHeroCfgFile("savematerial.ini");
	std::string strMats(TCHAR_TO_UTF8(*strData));

	std::vector<std::string> vecList;
	GameAppExtern::SplitVecString(vecList, strMats, ",");
	for (int i = 0; i < vecList.size(); i++)
	{
		int nItemId = atoi(vecList[i].c_str());
		if (nItemId == 0) {
			continue;
		}
		if (!ItemCfg::getInstance().getMaterial(nItemId)) {
			continue;
		}
		m_vecSaveMat.push_back(nItemId);
	}
	MaterialEvent evt;
	UI_DISPATCH_MYEVENT(MaterialEvent, evt);
}

void AHero::SaveCurrencyItems()
{
	std::string strSave;
	for (int i = 0 ; i < m_vecSaveMat.size(); i++)
	{
		if (m_vecSaveMat[i] == 0) {
			continue;
		}

		TArray<FStringFormatArg> Args;
		Args.Add(FStringFormatArg(m_vecSaveMat[i]));  // {0}

		strSave += TCHAR_TO_UTF8(*FString::Format(TEXT("{0}|{1},"), Args));
	}
	SaveHeroCfgFile("savematerial.ini", strSave.c_str());
}

void AHero::SetCurrencyItems(int nItemId, bool bEquip)
{
	auto&& iter = std::find(m_vecSaveMat.begin(), m_vecSaveMat.end(), nItemId);
	if (bEquip && iter == m_vecSaveMat.end()) {
		if (m_vecSaveMat.size() >= 6) {
			MsgBoxToastMgr::getInstance().ShowText(__LC("UI_COMMON_CURRENCY_FULL"));
			return;
		}
		m_vecSaveMat.push_back(nItemId);
	} else if (!bEquip && iter != m_vecSaveMat.end()) {
		m_vecSaveMat.erase(iter);
	}
	SaveCurrencyItems();
	MaterialEvent evt;
	UI_DISPATCH_MYEVENT(MaterialEvent, evt);
}

bool AHero::IsCurrencyItems(int nItemId)
{
	auto&& iter = std::find(m_vecSaveMat.begin(), m_vecSaveMat.end(), nItemId);
	return iter != m_vecSaveMat.end();
}

//***************************************************************************************************************************************************
//
//***************************************************************************************************************************************************
void AHero::HeroRideHorse(bool bRide)
{
	ResetWaitTrans();
	auto&& itemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::EQUIPMENT, EQUIP_PACKET_HORSE);
	if (!bRide && itemData) {
		GameAppExtern::requestItemEquip(0, EQUIP_PACKET_HORSE, -1);
	}
	if (bRide && !itemData) {
		if (g_i64RiderHorseUUID != 0) {
			auto&& itemDataH = ItemPacketMgr::getInstance().findItemDataByUuid(g_i64RiderHorseUUID);
			if (itemDataH) {
				GameAppExtern::requestItemEquip(itemDataH->getUuid());
				return;
			}
		}
		auto&& itemDataB = ItemPacketMgr::getInstance().findFirstBagItemDataByEquipPos(MsgPB::ARMOR_EQUIP_HORSE);
		if (itemDataB) {
			GameAppExtern::requestItemEquip(itemDataB->getUuid());
			g_i64RiderHorseUUID = itemDataB->getUuid();
			return;
		}
	}
}

void AHero::ResetWaitTrans()
{
	if (!m_bWaitTrans)
	{
		return;
	}
	m_bWaitTrans = false;
	GetWorldTimerManager().ClearTimer(MyTimerHand);
}

void AHero::TranslateEnd()
{
	if (!m_bWaitTrans)
	{
		return;
	}
	ResetWaitTrans();

	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return;
	}

	ALevelBase* pLevel = GameInstance->GetCurrentLevel();
	if (!pLevel || pLevel->GetLevelTpy() != LevelType::LevelType_MainCity)
	{
		return;
	}
	AMainCityLevel* pMainCity = Cast<AMainCityLevel>(pLevel);
	if (!pMainCity)
	{
		return;
	}
}

//***************************************************************************************************************************************************
//
//***************************************************************************************************************************************************
void AHero::TranslateToBorth()
{
	if (m_bWaitTrans) {
		return;
	}
	m_bWaitTrans = true;
	GetWorldTimerManager().ClearTimer(MyTimerHand);
	GetWorldTimerManager().SetTimer(MyTimerHand, this, &AHero::TranslateEnd, 5.0f, false);
}

//***************************************************************************************************************************************************
//
//***************************************************************************************************************************************************
bool AHero::CheckSkillConsume(int nSkillId, int nAtkTpySkill, bool bTips)
{
	if (!m_playerData) {
		return false;
	}
	if (nSkillId == 0) {
		auto pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
		if (!pCfg) {
			return false;
		}
		if (nAtkTpySkill == ATK_TPY_SKILL_NORMAL) {
			nSkillId = pCfg->n_SkillId;
		} else if (nAtkTpySkill == ATK_TPY_SKILL_EXT) {
			nSkillId = pCfg->n_exSkillId;
		} else if (nAtkTpySkill == ATK_TPY_SKILL_COMBO) {
			nSkillId = pCfg->n_ComboSkill;
		} else if (nAtkTpySkill == ATK_TPY_SKILL_PERSKILL) {
			nSkillId = pCfg->n_PreSkillId;
		}
	}

	if (nSkillId == 0) {
		return true;
	}

	auto&& pSkillCfg = SkillTypeCfg::getInstance().GetSkillType(nSkillId);
	if (!pSkillCfg) {
		return false;
	}
	if (pSkillCfg->f_Hp > 0.000001f) {
		if (m_playerData->CalcConsumeHp(pSkillCfg->f_Hp) > m_playerData->getHp()) {
			if (bTips) {
				MsgBoxToastMgr::getInstance().ShowText(__LC("UI_SKILL_CONSUM_HP"));
			}
			return false;
		}
	}

	if (pSkillCfg->f_Mana > 0.000001f) {
		if (m_playerData->CalcConsumeHp(pSkillCfg->f_Mana) > m_playerData->getMp()) {
			if (bTips) {
				MsgBoxToastMgr::getInstance().ShowText(__LC("UI_SKILL_CONSUM_MP"));
			}
			return false;
		}
	}

	if (pSkillCfg->n_Sten > 0.000001f) {
		if (pSkillCfg->n_Sten > m_playerData->getFood()) {
			if (bTips) {
				MsgBoxToastMgr::getInstance().ShowText(__LC("UI_SKILL_CONSUM_STRENTH"));
			}
			return false;
		}
	}

	if (pSkillCfg->n_Anger > 0.000001f) {
		if (pSkillCfg->n_Anger > m_playerData->getAnger()) {
			if (bTips) {
				MsgBoxToastMgr::getInstance().ShowText(__LC("UI_SKILL_CONSUM_ANGER"));
			}
			return false;
		}
	}
	return true;
}

bool AHero::SkillConsume(int nHp, int nMp, int nStrenth, int nAnger)
{
	if (!m_playerData)
	{
		return false;
	}
	if (nHp > m_playerData->getHp() || nMp > m_playerData->getMp() || nStrenth > m_playerData->getFood() || nAnger > m_playerData->getAnger()) {
		return false;
	}
	if (nHp > 0) {
		GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_HP, false, nHp);
	}
	if (nMp > 0) {
		GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_MP, false, nMp);
	}
	if (nStrenth > 0) {
		GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_STRENTH, false, nStrenth);
	}

	if (nAnger > 0) {
		GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_ANGER, true, nAnger);
	}
	return true;
}

void AHero::Death() 
{
	Super::Death();

	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance) {
		return;
	}

	ALevelBase* pLevel = GameInstance->GetCurrentLevel();
	if (!pLevel) {
		return;
	}

	if (pLevel->GetLevelTpy() == LevelType::LevelType_MainCity) {
	}
	else if (pLevel->GetLevelTpy() == LevelType::LevelType_Dungeon) {
		WindowEvent evtUI;
		evtUI.m_type = WindowEvent::EventType::Window_InsDefeat;
		UI_DISPATCH_MYEVENT(WindowEvent, evtUI);

		AInstanceLevel* pIns = Cast<AInstanceLevel>(pLevel);
		if (pIns) {
			pIns->SetEndBattle();
		}
	}
}

void AHero::Revive()
{
	if (m_playerData) {
		m_playerData->setHp(m_playerData->getMaxHp());
	}
	if (IsInMainCity()) {
		m_bWaitTrans = true;
		ResetHorse();
		TranslateEnd();
	}
	GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_HP,
		true, m_playerData->getHp(), MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON_MSG_SYN_BASE_TPY_REASON_UPDATE);
	Super::Revive();
}

void AHero::BeAttackAnger(int nDamage)
{
	if (nDamage <= 0 || !m_playerData) {
		return;
	}
	int nPerAnger = m_playerData->getMaxHp() / 5;
	if (nPerAnger == 0) {
		return;
	}
	int nAnger = nDamage * 100.f / nPerAnger;
	nAnger = std::min(100, std::max(1, nAnger));
	GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_ANGER, 
		false, nAnger, MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON_MSG_SYN_BASE_TPY_REASON_BE_ATTACK);
}

//***************************************************************************************************************************************************
// 攻击后，临时保存攻击的对象
//***************************************************************************************************************************************************
void AHero::LockFocusObject()
{
	m_strLockObject = "";
	m_fLockLessTm = 0.0f;
	if (GetFocusRole() && !GetFocusRole()->IsDeath())
	{
		m_strLockObject = GetFocusRole()->GetName();
		m_fLockLessTm = 5.0f;
	}
}

void AHero::ResetFocusObject()
{
	if (GetDamageCollision()) {
		GetDamageCollision()->CalFocusRole(m_strLockObject);
	}
}

//***************************************************************************************************************************************************
// 副本开战后，武将Buff设置
//***************************************************************************************************************************************************
void AHero::SetBattleGenBuff(int nBattleBuf)
{
	m_nBattleGenBuf = nBattleBuf;
}

int AHero::GetBattleGenBuff()
{
	return m_nBattleGenBuf;
}

void AHero::SetBattleGenBuffInx(int nIdx, bool bSet)
{
	if (bSet) {
		m_nBattleGenBuf |= (1 << (nIdx - 1));
	}
	else {
		m_nBattleGenBuf &= ~(1 << (nIdx - 1));
	}
	EvtSetGenByff(m_nBattleGenBuf);
}

bool AHero::IsGenBuffState(int nIdx)
{
	if (!m_playerData) {
		return false;
	}
	if ((m_nBattleGenBuf & (1 << (nIdx - 1))) == 0) {
		return false;
	}
	return false;
}

//***************************************************************************************************************************************************
// 设置自动吃药
//***************************************************************************************************************************************************
void AHero::SetAutoMediaPercent(int nTpy, int nPerent)
{
	switch (nTpy)
	{
	case 0:
		m_nAutoLifePer = nPerent;
		break;
	case 1:
		m_nAutoManaPer = nPerent;
		break;
	case 2:
		m_nAutoStrePer = nPerent;
		break;
	case 3:
		m_nAutoBreaPer = nPerent;
		break;
	default:
		break;
	}
}

//删除账号，直接退出游戏
void AHero::onMsgDeleteAccountS2C(const MsgDeleteAccountS2C& msg)
{
	exit(0);
}



//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
bool AIPlayerMgr::initialize()
{
	return true;
}

void AIPlayerMgr::terminate()
{

}


void AIPlayerMgr::PreReset()
{
	if (m_Hero) {
		m_Hero->Reset();
	}
	auto iter = m_players.begin();
	for (; iter != m_players.end(); iter++)
	{
		iter->second->Reset();
	}
}

bool AIPlayerMgr::reset()
{
	m_players.clear();
	m_Hero = nullptr;
	return true;
}

bool AIPlayerMgr::AddPlayer(AIPlayer* pPlayer)
{
	if (!pPlayer)
	{
		return false;
	}

	MapAIPlayer::iterator iter = m_players.begin();
	for (; iter != m_players.end(); iter++)
	{
		if (iter->second == pPlayer)
		{
			m_players.erase(iter);
			break;
		}
	}
	m_players[pPlayer->GetObjectId()] = pPlayer;
	return true;
}

bool AIPlayerMgr::RemovePlayer(int nObjId)
{
	m_players.erase(nObjId);
	return true;
}

AIPlayer* AIPlayerMgr::GetPlayer(int nObjId)
{
	if (m_players.find(nObjId) == m_players.end()) {
		return nullptr;
	}
	return m_players[nObjId];
}

void AIPlayerMgr::update(float dt)
{

}

AHero* AIPlayerMgr::GetHero()
{
	return m_Hero;
}

void AIPlayerMgr::SetHero(AHero* hero)
{
	m_Hero = hero;
}

AIPlayerMgr::AIPlayerMgr()
{
	initialize();
}

AIPlayerMgr& AIPlayerMgr::getInstance()
{
	static AIPlayerMgr playerMgr;
	return playerMgr;
}

