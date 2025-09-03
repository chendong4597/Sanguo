// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenceBuilding.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Animation/AnimMontage.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

#include "../AI/DefenceAI/DefenceAIController.h"
#include "../Skill/SkillEffect.h"
#include "config/DefenceInfoTypeCfg.h"
#include "config/DefencePosTpyCfg.h"
#include "config/BuildingCfg.h"
#include "config/TowerBuildingCfg.h"
#include "data/TowerBuildData.h"
#include "../Common/RootEventObject.h"
#include "../ReichGameInstance.h"

extern UWorld* gp_UWorld;


void ADefenceBuilding::Init()
{
	Super::Init();
}

	// Called when the game starts or when spawned
void ADefenceBuilding::BeginPlay()
{
	Super::BeginPlay();
	SetCamp(CampState::CAMP_HERO);
	RegisterRoleClick();

	RG_REGISTER_MYEVENT(HandlerTowerBuildEvent, &ADefenceBuilding::onTowerBuildEvent);
}

	// Called every frame
void ADefenceBuilding::Tick(float DeltaTime)
{
	if (m_curTm < m_InterTm)
	{
		m_curTm += DeltaTime;
	}
	Super::Tick(DeltaTime);
}

//****************************************************************************************
//
//****************************************************************************************
bool ADefenceBuilding::PlayAttack()
{
	m_curTm = 0.f;
	if (IsAttacking())
	{
		return false;
	}
	auto aniMota = this->GetCurrentMontage();
	if (aniMota)
	{
		this->StopAnimMontage(aniMota);
	}
	UAnimMontage* aniMotaToPlay = NULL;
	auto pCfg = DefenceInfoTypeCfg::getInstance().GetDefenceById(m_nDefenceId);
	if (!pCfg)
	{
		return false;
	}

	std::string str = DefenceInfoTypeCfg::getInstance().GetAttackPath(m_nDefenceId);
	FString strAni(str.c_str());
	UObject* loadObj = StaticLoadObject(UAnimMontage::StaticClass(), NULL, strAni.GetCharArray().GetData());//);//TEXT("Blueprint'/Game/BluePrint/TestObj.TestObj'")
	if (loadObj != nullptr)
	{
		aniMotaToPlay = Cast<UAnimMontage>(loadObj);
	}

	if (!aniMotaToPlay)
	{
		return false;
	}

	PlayAnimMontage(aniMotaToPlay);

	//auto from = GetActorLocation();
	//auto to = GetFocusRole()->GetActorLocation();
	//auto toRota = UKismetMathLibrary::FindLookAtRotation(from, to);
	//FRotator NewRotation(GetActorRotation().Pitch, toRota.Yaw, GetActorRotation().Roll);
	//SetActorRotation(NewRotation);
	return true;
}

//****************************************************************************************
//
//****************************************************************************************
bool ADefenceBuilding::CanAttack()
{
	if (!Super::CanAttack()) {
		return false;
	}
	if (GetFocusRole() == nullptr)
	{
		return false;
	}
	if (m_curTm < m_InterTm) {
		return false;
	}
	return true;
}

//****************************************************************************************
//
//****************************************************************************************
void ADefenceBuilding::AttackEffect()
{
	auto pCfg = DefenceInfoTypeCfg::getInstance().GetDefenceById(m_nDefenceId);
	if (!pCfg)
	{
		return;
	}
	if (!GetFocusRole())
	{
		return;
	}
	
	for (int i = 0 ; i < pCfg->n_AtkNum; i++)
	{
		if (m_aRangeCollion && i >= m_aRangeCollion->GetTargets().Num())
		{
			break;
		}
		//SkillMgr::getInstance().CreateSkill(pCfg->n_SkillId, this, m_aRangeCollion->GetTargets()[i] , 
			//m_aRangeCollion->GetTargets()[i]->GetActorLocation(), FVector::ZeroVector);
	}
	
}

//****************************************************************************************
//
//****************************************************************************************
bool ADefenceBuilding::HitTest()
{
	WindowEvent evt;
	evt.m_type = WindowEvent::EventType::Window_DefenceInfo;
	evt.nParam = m_uuid;
	UI_DISPATCH_MYEVENT(WindowEvent, evt);
	return true;
}

//****************************************************************************************
//
//****************************************************************************************
void ADefenceBuilding::StartDefence(int nDefenceId)
{
	auto pCfg = DefenceInfoTypeCfg::getInstance().GetDefenceById(nDefenceId);
	if (!pCfg)
	{
		return;
	}
	m_InterTm = pCfg->f_IntertTm;
	if (Defence_Build_Tpy_Normal == pCfg->n_Type)
	{
	}
	m_nDefenceId = nDefenceId;
}

//****************************************************************************************
//
//****************************************************************************************
bool ADefenceBuilding::IsCanAttackTarget(ARole* pRole, bool bPassive)
{
	if (!pRole || pRole->IsDeath())
	{
		return false;
	}
	if (pRole->GetCamp() == CampState::CAMP_MONSTER)
	{
		return true;
	}
	return false;
}

//****************************************************************************************
//
//****************************************************************************************
void ADefenceBuilding::StartAI()
{
	auto&& pDefCfg = DefenceInfoTypeCfg::getInstance().GetDefenceById(m_nDefenceId);
	if (!pDefCfg)
	{
		return;
	}
	if (pDefCfg->n_AI == 0)
	{
		return;
	}
	AController* controler = GetController();
	if (!controler)
	{
		return;
	}
	ADefenceAIController* pAICtrl = Cast<ADefenceAIController>(controler);
	if (!pAICtrl)
	{
		return;
	}
	pAICtrl->SetAI(m_nDefenceId);
}


//****************************************************************************************
//
//****************************************************************************************
void ADefenceBuilding::onTowerBuildEvent(const HandlerTowerBuildEvent& evt)
{
	if (evt.m_type == HandlerTowerBuildEvent::EventTowerBuild::EventTowerBuild_Del &&
		evt.m_uuid == m_uuid) {
		DefenceBuidMgr::getInstance().CreateNoneBuild(m_buildId, 0, m_buildTpy, GetTransform());
		DestroyRole();
	}else if (evt.m_type == HandlerTowerBuildEvent::EventTowerBuild::EventTowerBuild_Update &&
		evt.m_uuid == m_uuid) {
	}
}


//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
bool DefenceBuidMgr::initialize()
{
	return true;
}

void DefenceBuidMgr::update(float dt)
{

}


bool DefenceBuidMgr::reset()
{
	m_vecDefenceBuid.clear();
	return true;
}

DefenceBuidMgr::DefenceBuidMgr()
{
	//initialize();
}

void DefenceBuidMgr::InitDefeceBuilding()
{
	TArray<AActor*> arrActors;
	UGameplayStatics::GetAllActorsOfClass(gp_UWorld, ADefenceBuilding::StaticClass(), arrActors);
	for (int i = 0; i < arrActors.Num(); i++)
	{
		ADefenceBuilding* building = Cast<ADefenceBuilding>(arrActors[i]);
		if (!building)
		{
			continue;
		}
		m_vecDefenceBuid.push_back(building);
	}
}

void DefenceBuidMgr::initTowerBuidings()
{
	auto&& mapBuilds = TowerBuildMgr::getInstance().GetBuildingDatas();
	for (auto&& data : mapBuilds)
	{
		if (data.second->m_itemId > 0)
		{
			CreateDefenceBuid(data.first , data.second->m_itemId, CampState::CAMP_HERO , data.second->n_PosX , data.second->n_PosY);
		}
	}
}

void DefenceBuidMgr::AddDefenceBuildByUUId(NS::I64 uuid)
{
	auto&& pItem = TowerBuildMgr::getInstance().findTowerBuildByUuid(uuid);
	if (pItem)
	{
		CreateDefenceBuid(uuid , pItem->m_itemId, CampState::CAMP_HERO, pItem->n_PosX, pItem->n_PosY);
	}
}

ADefenceBuilding* DefenceBuidMgr::GetDefeceBuild(int nX, int nY)
{
	for (int i = 0; i < m_vecDefenceBuid.size(); i++) {
		if (m_vecDefenceBuid[i]->m_buildId == nY* TOWER_BUILD_ROW_NUM + nX) {
			return m_vecDefenceBuid[i];
		}
	}
	return nullptr;
}

void DefenceBuidMgr::CreateNoneBuild(int nX, int nY, int nBuildTpy, FTransform trans)
{
	UClass* generalClass = LoadObject<UClass>(NULL, TEXT("Blueprint'/Game/Models/defence/BP/BP_DefenceBuid.BP_DefenceBuid_C'"));
	if (!generalClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("DefenceBuidMgr::CreateDefenceBuid failed"));
		return;
	}


	if (gp_UWorld) {
		FActorSpawnParameters params = FActorSpawnParameters();
		params.bNoFail = true;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ADefenceBuilding* pDefence = gp_UWorld->SpawnActor<ADefenceBuilding>(generalClass, trans, params);
		if (pDefence)
		{
			pDefence->SetUuid(0);
			pDefence->m_buildId = nX;
			pDefence->m_buildTpy = nBuildTpy;
			m_vecDefenceBuid.push_back(pDefence);
		}
	}
}

void DefenceBuidMgr::RemoveDefeceBuild(int nX, int nY)
{
	for (int i = 0; i < m_vecDefenceBuid.size(); i++) {
		if (m_vecDefenceBuid[i]->m_buildId == nY * TOWER_BUILD_ROW_NUM + nX) {
			m_vecDefenceBuid[i]->DestroyRole();
			m_vecDefenceBuid.erase(m_vecDefenceBuid.begin() + i);
			return;
		}
	}
}

void DefenceBuidMgr::CreateDefenceBuid(NS::I64 uuid, int nBuildId, CampState camp, int posX , int posY)
{
	auto pDefenceCfg = TowerBuildingCfg::getInstance().getTowerBuildingDefence(nBuildId);
	if (!pDefenceCfg)
	{
		return;
	}

	if (pDefenceCfg->m_category == TowerBuildingCategory::CRYSTAL)  //水晶不用建设
	{
		return;
	}

	auto pCfg = DefenceInfoTypeCfg::getInstance().GetDefenceById(pDefenceCfg->m_buildingTypeId);
	if (!pCfg)
	{
		return;
	}
	
	ADefenceBuilding* pCurBuild = GetDefeceBuild(posX, posY);

	if (!pCurBuild) {
		return;
	}

	std::string strBP = DefenceInfoTypeCfg::getInstance().GetBpPath(pDefenceCfg->m_buildingTypeId);
	if (strBP.empty())
	{
		return;
	}
	FString strPath(strBP.c_str());
	UClass* generalClass = LoadObject<UClass>(NULL, *strPath);
	if (!generalClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("DefenceBuidMgr::CreateDefenceBuid failed"));
		return;
	}
	
	FTransform trans = pCurBuild->GetTransform();

	if (gp_UWorld) {
		FActorSpawnParameters params = FActorSpawnParameters();
		params.bNoFail = true;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ADefenceBuilding* pDefence = gp_UWorld->SpawnActor<ADefenceBuilding>(generalClass, trans, params);
		if (pDefence)
		{
			pDefence->SetCamp(camp);
			pDefence->StartDefence(pDefenceCfg->m_buildingTypeId);
			pDefence->SetUuid(uuid);
			pDefence->m_buildId = pCurBuild->m_buildId;
			pDefence->m_buildTpy = pCurBuild->m_buildTpy;
			RemoveDefeceBuild(posX, posY);
			pDefence->CreateMyCollision(100);
			m_vecDefenceBuid.push_back(pDefence);
		}
	}
}

DefenceBuidMgr& DefenceBuidMgr::getInstance()
{
	static DefenceBuidMgr s_DefenceMgr;
	return s_DefenceMgr;
}