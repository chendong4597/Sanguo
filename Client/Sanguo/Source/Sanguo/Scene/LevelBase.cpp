// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelBase.h"

#include "Kismet/GameplayStatics.h"
#include "data/NpcProtectorData.h"
#include "config/NpcTypeCfg.h"
#include "config/MonsterCfg.h"
#include "data/role/Npc.h"
#include "data/TowerBuildData.h"
#include "data/PlantData.h"
#include "data/HouseData.h"
#include "data/role/Player.h"

#include "../Player/Soldier.h"
#include "../Player/Generals.h"
#include "../Player/IPlayer.h"
#include "../Player/Soldier.h"
#include "../Player/Monster.h"
#include "../Player/Hero.h"
#include "config/TowerBuildingCfg.h"
#include "config/MapSettingCfg.h"

#include "../UI/UILoading.h"
#include "UI/UIManager.h"

#include "ReichGameInstance.h"
#include "InstanceLevel.h"
#include "MainCityLevel.h"

UWorld* gp_UWorld = nullptr;

void ALevelBase::BeginPlay()
{
	Super::BeginPlay();
	gp_UWorld = GetWorld();

	if (UReichGameInstance::IsEditorMode()) return;

	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SetCurrentLevel(this);
	}

	UI_REGISTER_MYEVENT(UILoadintEvent, &ALevelBase::onUILoadintEvent);

	RG_REGISTER_MYEVENT(HandlerMonsterEvent, &ALevelBase::onMonsterEvent);
	RG_REGISTER_MYEVENT(HandlerTransterEvent, &ALevelBase::onTransterEvent);
	RG_REGISTER_MYEVENT(HandlerNpcEvent, &ALevelBase::onHandlerNpcEvent);

	RG_REGISTER_MYEVENT(MsgJoinMapG2C, &ALevelBase::onMsgJoinMapG2C);
	RG_REGISTER_MYEVENT(MsgTeamDungeonReadyToGo_S2C, &ALevelBase::onMsgTeamDungeonReadyToGo_S2C);

	if (m_levelTpy != LevelType::LevelType_Login && m_levelTpy != LevelType::LevelType_CreateRole) {
		//副本编辑时用上
		ShowLoadingUI(false);
	}
}

void ALevelBase::onUILoadintEvent(const UILoadintEvent& evt)
{
	if (evt.m_type == UILoadintEvent::LoadingType::Pre_Load)
	{
		switch (evt.m_loadstate) {
		case ui_unload_state_players:
		{
			AIPlayerMgr::getInstance().reset();
			AMonsterMgr::getInstance().reset();
			AGeneralsMgr::getInstance().reset();
			ASoldierMgr::getInstance().reset();
		}
		break;
		case ui_unload_state_npcs:
		{
		}
		break;
		case ui_unload_state_npcs_end:
		{
			UWorld* world = GetWorld();
			UGameplayStatics::OpenLevel(GetWorld(), m_nxtLevelName);
		}
		break;
		default:
			break;
		}
	}
	else {
		TransLoadState(evt.m_loadstate);
	}
}

void ALevelBase::TransLoadState(int nLoadState)
{
	
}

void ALevelBase::ShowLoadingUI(bool bUnload)
{
	TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(AIPlayerMgr::getInstance().GetHero(),
		TEXT("WidgetBlueprint'/Game/UI/UILoading/UILoading.UILoading_C'"));
	if (WidgetClass) {
		UUserWidget* UserWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		UUILoading* pLoading = Cast<UUILoading>(UserWidget);
		if (pLoading)
		{
			pLoading->AddToViewport(UI_Z_ORDER_MSG_LOADING);
			pLoading->SetIsLoadingPre(bUnload);
		}
	}
}

void ALevelBase::PreUnloadData()
{
	if (PlayerMgr::getInstance().getSelf()) {  //清除所有buff, debuf
		GameAppExtern::c2cNormalDelBuff(0, PlayerMgr::getInstance().getSelf()->getId());
	}
	
	AIPlayerMgr::getInstance().PreReset();
	AMonsterMgr::getInstance().PreReset();
	AGeneralsMgr::getInstance().PreReset();
	ASoldierMgr::getInstance().PreReset();
	MonsterDataMgr::getInstance().Clear();
	PlantDataMgr::getInstance().Clear();
	TowerBuildMgr::getInstance().Clear();
	PlayerMgr::getInstance().ClearOtherPlayer();
	NpcMgr::getInstance().Clear();
	HouseDataMgr::getInstance().Clear();
	NpcProtectorDataMgr::getInstance()->Clear();
}

void ALevelBase::PreUnload()
{
	UNRG_REGISTER_MYEVENT(HandlerMonsterEvent, this);
	UNRG_REGISTER_MYEVENT(HandlerTransterEvent, this);
	UNRG_REGISTER_MYEVENT(MsgJoinMapG2C, this);
}

void ALevelBase::onTransterEvent(const HandlerTransterEvent& evt)
{
	//UE_LOG(LogOutputDevice, Log, TEXT("Log UUILogin::onTransterEvent trans maincity"));
	UWorld* world = GetWorld();
	if (evt.m_type == HandlerTransterEvent::EventTranster::EventTranster_DungeonMap) {
		auto&& pMapCfg = MapSettingCfg::getInstance().findMap(MapDataMgr::getInstance().getMapId());
		if (pMapCfg) {
			MapDataMgr::getInstance().StartMapTime();
			FString strPath = FString(pMapCfg->m_mapPath.c_str());
			TranslateMap(*strPath);
		}
	} else if (evt.m_type == HandlerTransterEvent::EventTranster::EventTranster_World) {
		UE_LOG(LogOutputDevice, Warning, TEXT("UUILogin::onTransterEvent trans maincity to my World"));
		TranslateMap(TEXT("World"));
	} else if (evt.m_type == HandlerTransterEvent::EventTranster::EventTranster_PrivateMap) {
		UE_LOG(LogOutputDevice, Log, TEXT("UUILogin::onTransterEvent trans maincity to my MainCity"));
		TranslateMap(TEXT("MainCity"));
	}
	else if (evt.m_type == HandlerTransterEvent::EventTranster::EventTranster_CreateRole) {
		UE_LOG(LogOutputDevice, Log, TEXT("UUILogin::onTransterEvent trans create role"));
		TranslateMap(TEXT("CreateRole"));
	}
}

void ALevelBase::TranslateMap(FName LevelName)
{
	PreUnload();
	m_nxtLevelName = LevelName;
	ShowLoadingUI(true);
}

void ALevelBase::onMsgJoinMapG2C(const MsgJoinMapG2C& msg)
{
	PreUnloadData();
}

void ALevelBase::onMsgTeamDungeonReadyToGo_S2C(const MsgTeamDungeonReadyToGo_S2C& msg)
{
	UIManager::getInstance().CloseWindowByType(WindowEvent::EventType::Window_InsMap);
	UIManager::getInstance().CloseWindowByType(WindowEvent::EventType::Window_InsSelect);

	if (msg.countdown() > 0)
	{
	}
}


void ALevelBase::onMonsterEvent(const HandlerMonsterEvent& evt)
{
	//(X = 19138.544922, Y = 49090.335938, Z = 90.899994)
	FTransform trans;
	bool bSolider = false;
	if (HandlerMonsterEvent::EventMonster::EventMonster_Appearance == evt.m_type) {
		auto&& monsterData = MonsterDataMgr::getInstance().getMonster(evt.mapObjId);
		if (!monsterData) {
			return;
		}

		if (GetLevelTpy() == LevelType::LevelType_Dungeon) {
			AInstanceLevel* pLevel = Cast<AInstanceLevel>(this);
			if (pLevel) {
				if (monsterData->getMasterUuid() == 0)
				{
					trans = pLevel->GetMonsterTransFlag(evt.insId);
				}
				else {
					trans = pLevel->GetMonsterTransNpcFlag(monsterData->getMasterUuid());
				}
			}

			if (trans.GetLocation().IsZero())
			{
				FVector vec;
				vec.X = FMath::RandRange(14860, 22088);
				vec.Y = FMath::RandRange(36447, 41061);
				vec.Z = 90;
				trans.SetLocation(vec);
			}

		} else if (GetLevelTpy() == LevelType::LevelType_MainCity) {
			AMainCityLevel* pLevel = Cast<AMainCityLevel>(this);
			if (!pLevel) {
				return;
			}

			auto&& pMonster = MonsterCfg::getInstance().findMonsterType(evt.defId);
			if (!pMonster) {
				return;
			}

			FVector vec;
			vec = pLevel->GetBattlePos(pMonster->n_MobLv);
			if (vec == FVector::ZeroVector) {
				return;
			}
			vec.X = vec.X - 1000 + std::rand() % 1000;
			vec.Y = vec.Y - 1000 + std::rand() % 1000;
			vec.Z = 90;
			trans.SetLocation(vec);
		}
		
		if (monsterData->getMasterUuid() == 0 || monsterData->getMasterUuid() == 1)
		{
			AMonsterMgr::getInstance().CreateMonster(evt.mapObjId, evt.insId, evt.defId, trans);
		}
		else {
			ASoldierMgr::getInstance().CreateSoldier(evt.mapObjId, monsterData->getMasterUuid(), evt.defId, trans);
		}
	} else if (evt.m_type == HandlerMonsterEvent::EventMonster::EventMonster_DisAppearance) {
		AMonsterMgr::getInstance().RemoveMonster(evt.mapObjId);
		ASoldierMgr::getInstance().RemoveSoldier(evt.mapObjId);
	}
}

void ALevelBase::onHandlerNpcEvent(const HandlerNpcEvent& evt)
{
	if (evt.m_type == HandlerNpcEvent::EventNpc::EventNpc_Appearance) {
		UE_LOG(LogOutputDevice, Log, TEXT("ALevelBase::onHandlerNpcEvent EventNpc_Appearance"));
		auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(evt.insId);
		if (!data) {
			UE_LOG(LogOutputDevice, Log, TEXT("ALevelBase::onHandlerNpcEvent data nil"));
			return;
		}
		auto&& pNpcType = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
		if (!pNpcType)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("ALevelBase::onHandlerNpcEvent pNpcType nil"));
			return;
		}
		if (pNpcType->n_Type == NpcTypeCfg::NpcType_Func) {
			//ANpcMgr::getInstance().CreateNpc(evt.mapObjId, evt.insId, evt.defId);
		} else if (pNpcType->n_Type == NpcTypeCfg::NpcType_Genels) {
			UE_LOG(LogOutputDevice, Log, TEXT("ALevelBase::onHandlerNpcEvent CreateGenels"));
			AGeneralsMgr::getInstance().CreateGenels(evt.mapObjId, evt.insId, evt.defId);
			if (m_levelTpy == LevelType::LevelType_Dungeon)
			{
				GeneralsEvent evt1;
				evt1.objId = evt.mapObjId;
				evt1.m_type = GeneralsEvent::EventType::Gen_Add;
				UI_DISPATCH_MYEVENT(GeneralsEvent, evt1);
			}
		}
	} else if (evt.m_type == HandlerNpcEvent::EventNpc::EventNpc_DisAppearance) {
		AGeneralsMgr::getInstance().DestroyGenels(evt.mapObjId);
		ASoldierMgr::getInstance().RemoveSoldier(evt.mapObjId);
	}
}

