// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCityLevel.h"
#include "GameFramework//PlayerStart.h"
#include "Kismet/GameplayStatics.h"

#include "GameAppExtern.h"
#include "common/Heartbeat.h"
#include "data/HouseData.h"
#include "data/NpcProtectorData.h"
#include "config/NpcTypeCfg.h"

#include "../Player/Hero.h"
#include "../Player/Generals.h"
#include "../ReichGameInstance.h"
#include "../UI/UILoading.h"

extern UWorld* gp_UWorld;

void AMainCityLevel::BeginPlay()
{
	SetLevelTpy(LevelType::LevelType_MainCity);
	Super::BeginPlay();

	if (UReichGameInstance::IsEditorMode()) return;

	GameAppExtern::SetClientMode(true);

	//InitBattleFlag();

	UI_REGISTER_MYEVENT(HouseEvent, &AMainCityLevel::onHouseEvent);
	UI_REGISTER_MYEVENT(HeroInitEvent, &AMainCityLevel::onHeroInitEvent);
	
	RG_REGISTER_MYEVENT(HandlerRoleEvent, &AMainCityLevel::onHandlerRoleEvent);
	RG_REGISTER_MYEVENT(HandlerHouseEvent, &AMainCityLevel::onHandlerHouseEvent);
	RG_REGISTER_MYEVENT(HandlerTowerBuildEvent, &AMainCityLevel::onTowerBuildEvent);
	RG_REGISTER_MYEVENT(HandlerPlantEvent, &AMainCityLevel::onPlantsEvent);
}

void AMainCityLevel::TransLoadState(int nLoadState)
{
	switch (nLoadState) {
	case ui_load_state_player:
	{

	}
	break;
	//case ui_load_state_plant:
	//{
	//	APlantsMgr::getInstance().initNonePlants();
	//}
	//break;
	//case ui_load_state_def:
	//{
	//	
	//}
	//break;
	//case ui_load_state_house:
	//{
	//	AHouseMgr::getInstance().ResetHouseData();
	//}
	//break;
	//case ui_load_state_npc:
	//{
	//	ANpcMgr::getInstance().initNpc();
	//}
	//break;
	//case ui_load_state_init:
	//{
	//	InitCityWallMat();
	//	InitWorkFlag();
	//	RefreshCityWall();
	//	RefreshWorkPos();
	//	InitCrystal();
	//	InitLogingCamp();
	//}
	//break;
	default:
		break;
	}
}

// Called every frame
void AMainCityLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float sss_fNpc = 0.f;
	sss_fNpc += DeltaTime;
	if (sss_fNpc >= 20.f) {
		sss_fNpc = 0.f;
	}
}

void AMainCityLevel::PreUnload()
{
	Super::PreUnload();
	UI_UN_REGISTER_MYEVENT(HouseEvent, this);

	UNRG_REGISTER_MYEVENT(HandlerRoleEvent, this);
	UNRG_REGISTER_MYEVENT(HandlerHouseEvent, this);
	UNRG_REGISTER_MYEVENT(HandlerTowerBuildEvent, this);
	UNRG_REGISTER_MYEVENT(HandlerPlantEvent, this);
	UNRG_REGISTER_MYEVENT(HandlerNpcEvent, this);
}

FVector AMainCityLevel::GetBattlePos(int nLv)
{
	FVector vec;
	for (int i = 0; i < m_vecBattlePos.size(); i++)
	{
		if (nLv >= m_vecBattlePos[i].nMinLv && nLv <= m_vecBattlePos[i].nMaxLv)
		{
			vec = m_vecBattlePos[i].pos;
			vec.Z = 90.f;
			return vec;
		}
	}
	if (m_vecBattlePos.size() > 0) {
		vec = m_vecBattlePos[0].pos;
		vec.Z = 90.f;
		return vec;
	}
	return vec;
}

bool AMainCityLevel::IsSameBattleLevel(int nLev1, int nLev2)
{
	for (int i = 0; i < m_vecBattlePos.size(); i++)
	{
		if ((nLev1 >= m_vecBattlePos[i].nMinLv && nLev1 <= m_vecBattlePos[i].nMaxLv) &&
			(nLev2 >= m_vecBattlePos[i].nMinLv && nLev2 <= m_vecBattlePos[i].nMaxLv))
		{
			return true;
		}
	}
	return false;
}

void AMainCityLevel::onHandlerHouseEvent(const HandlerHouseEvent& evt)
{
}

void AMainCityLevel::onHouseEvent(const HouseEvent& evt)
{

}

void AMainCityLevel::onHeroInitEvent(const HeroInitEvent& evt)
{
}


void AMainCityLevel::onHandlerRoleEvent(const HandlerRoleEvent& evt)
{
}


//****************************************************************************************
//
//****************************************************************************************
void AMainCityLevel::onTowerBuildEvent(const HandlerTowerBuildEvent& evt)
{
	if (evt.m_type == HandlerTowerBuildEvent::EventTowerBuild::EventTowerBuild_Add) {
	}
	else if (evt.m_type == HandlerTowerBuildEvent::EventTowerBuild::EventTowerBuild_List) {
	}
}

void AMainCityLevel::onPlantsEvent(const HandlerPlantEvent& evt)
{
	if (evt.m_type == HandlerPlantEvent::EventPlant::EventPlant_List) {
	}
	else if (evt.m_type == HandlerPlantEvent::EventPlant::EventPlant_Add) {
	}
}




