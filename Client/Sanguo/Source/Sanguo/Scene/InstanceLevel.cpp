// Fill out your copyright notice in the Description page of Project Settings.


#include "InstanceLevel.h"
#include "Kismet/GameplayStatics.h"

#include "common/Heartbeat.h"
#include "config/TroopsCfg.h"
#include "config/MapSettingCfg.h"
#include "data/MapDataMgr.h"
#include "../ReichGameInstance.h"
#include "../Player/Hero.h"
#include "../Player/IPlayer.h"
#include "../Player/Monster.h"
#include "../Player/Generals.h"
#include "../Player/Soldier.h"
#include "../Player/MonsterFlag.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"

#include "UI/UIManager.h"

#include "HAL/PlatformFilemanager.h"


FVector CalculateGridPointPosition(
	const FVector& PointA,      // A点位置
	const FVector& PointB,      // B点位置
	float DistanceS,            // 阵中心距离A点的距离
	float SpacingW,             // 阵点间距
	int32 Index                 // 阵点索引 (0-15)
)
{
	// 1. 计算方向向量 (从A指向B)
	FVector Direction = (PointB - PointA).GetSafeNormal();

	// 2. 计算一个垂直于方向向量的向量 (用于构建阵点的坐标系)
	FVector Perpendicular = FVector(-Direction.Y, Direction.X, Direction.Z).GetSafeNormal();

	// 3. 计算阵中心的坐标
	FVector GridCenter = PointA + Direction * DistanceS;

	// 4. 根据索引计算阵点的行列
	int32 Row = Index / 4;      // 行 (0-3)
	int32 Col = Index % 4;      // 列 (0-3)

	// 5. 计算阵点的偏移量
	float OffsetX = (Col - 1.5f) * SpacingW; // 列偏移 (居中)
	float OffsetY = (Row - 1.5f) * SpacingW; // 行偏移 (居中)

	FVector vecP = Direction * OffsetY;

	// 6. 计算阵点的最终位置
	FVector GridPoint = GridCenter + Perpendicular * OffsetX + vecP;

	return GridPoint;
}

void AInstanceLevel::BeginPlay()
{
	SetLevelTpy(LevelType::LevelType_Dungeon);
	Super::BeginPlay();

	if (UReichGameInstance::IsEditorMode()) return;

	m_convoyTrans.SetLocation(FVector(19998, 38211, 143));
	FRotator rot;
	rot.Pitch = 0.f;
	rot.Yaw = -90;
	rot.Roll = 0.f;
	m_convoyTrans.SetRotation(rot.Quaternion());

	RG_REGISTER_MYEVENT(MsgDungeonBossChest_MS2C, &AInstanceLevel::onMsgDungeonBossChest_MS2C);
	RG_REGISTER_MYEVENT(MsgDungeonBossOpenChestRs_MS2C, &AInstanceLevel::onMsgDungeonBossOpenChestRs_MS2C);
	RG_REGISTER_MYEVENT(MsgDungeonNextMob_S2C, &AInstanceLevel::onMsgDungeonNextMob_S2C);

#ifdef PLATFORM_WINDOWS
	//副本编辑
	//ExportInstanceMobs();
#endif
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AInstanceLevel::ShowTroopView, 0.01f, false);
}

void AInstanceLevel::ExportInstanceMobs()
{
	FString ThePath = FString(FPaths::ProjectContentDir());
	std::string cstr(TCHAR_TO_UTF8(*ThePath));

	TArray<AActor*> arrActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterFlag::StaticClass(), arrActors);

	std::string str;

	for (int i = 0; i < arrActors.Num(); i++)
	{
		AMonsterFlag* flag = Cast<AMonsterFlag>(arrActors[i]);
		if (!flag || flag->m_mobId == 0)
		{
			continue;
		}

		int32 X = flag->GetActorLocation().X;
		int32 Y = flag->GetActorLocation().Y;
		int32 Z = flag->GetActorLocation().Z;
		int32 R = flag->GetActorRotation().Yaw;

		TArray<FStringFormatArg> Args;
		Args.Add(FStringFormatArg(flag->m_mobTpy));
		Args.Add(FStringFormatArg(flag->m_nStage));
		Args.Add(FStringFormatArg(i + 1));
		Args.Add(FStringFormatArg(flag->m_mobId));
		Args.Add(FStringFormatArg(flag->m_subMobId));
		Args.Add(FStringFormatArg(flag->m_subMobNum));
		Args.Add(FStringFormatArg(X));
		Args.Add(FStringFormatArg(Y));
		Args.Add(FStringFormatArg(Z));
		Args.Add(FStringFormatArg(R));

		str += TCHAR_TO_UTF8(*FString::Format(TEXT("[{0};{1};{2};{3};{4};{5};{6};{7};{8};{9}]"), Args));//, flag->m_mobTpy, flag->m_nStage, i + 1,
			//flag->m_mobId, flag->m_subMobId, flag->m_subMobNum, X, Y, Z);
		if (i != arrActors.Num() - 1)
		{
			str += ",";
		}

		TestInitMobs(flag->m_mobId, i + 1, flag->GetActorTransform());
		for (int j = 0; j < flag->m_subMobNum; j++)
		{
			TestInitMobs(flag->m_subMobId, (i + 1)*1000 + j + 1, flag->GetActorTransform());
		}
	}
	FString strData(str.c_str());
	if (strData.IsEmpty())
	{
		return;
	}

	IPlatformFile& formFile = FPlatformFileManager::Get().GetPlatformFile();
	//FString strFonder = "InsTemp/";
	FString TempFolder = FPaths::ProjectSavedDir();
	FString TempFileName = TempFolder + "ins.txt";

	// 创建临时文件夹
	//formFile.CreateDirectory(*TempFolder);
	FFileHelper::SaveStringToFile(strData, *TempFileName);
}

void AInstanceLevel::GetPlayerStartTransform(FTransform& OutTransform)
{
	TArray<AActor*> arrActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), arrActors);
	if (arrActors.Num() > 0)
	{
		OutTransform = arrActors[0]->GetActorTransform();
	}
}

void AInstanceLevel::TestInitMobs(int nMonsterId, int insId, FTransform mobTrans)
{
	static int  mmm_mapId = 1;

	FTransform trans;

	FTransform transStart;
	GetPlayerStartTransform(transStart);
	FVector vecBack = transStart.GetRotation().GetForwardVector()*-1.f;

	if (insId >= 1000) {
		int nPos = insId % 1000;
		FVector vec(mobTrans.GetLocation().X, mobTrans.GetLocation().Y, mobTrans.GetLocation().Z);
		FVector vecTo = vec + FVector(vecBack.X*-400.f, vecBack.Y*-400.f, 0.f);
		FVector vecPos = CalculateGridPointPosition(vec, vecTo, 600, 300, nPos - 1);
		vecPos.Z = mobTrans.GetLocation().Z;
		trans.SetLocation(vecPos);
		FRotator rot = transStart.GetRotation().Rotator();
		rot.Yaw += 180;
		trans.SetRotation(rot.Quaternion());
	}
	else
	{
		FVector vec(mobTrans.GetLocation().X, mobTrans.GetLocation().Y, mobTrans.GetLocation().Z);
		vec.Z = mobTrans.GetLocation().Z;
		trans.SetLocation(vec);
		FRotator rot = transStart.GetRotation().Rotator();
		rot.Yaw += 180;
		trans.SetRotation(rot.Quaternion());
	}
	AMonsterMgr::getInstance().CreateMonster(mmm_mapId, insId, nMonsterId, trans);
	mmm_mapId++;
}

void AInstanceLevel::ShowTroopView()
{
	//副本编辑时用上
	//return;
	WindowEvent evt;
	evt.m_type = WindowEvent::EventType::Window_TroopsView;
	UI_DISPATCH_MYEVENT(WindowEvent, evt);
}

// Called every frame
void AInstanceLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInstanceLevel::SetTroopInfo(int nTroopId, const std::map<int, int64> info)
{
	m_nTroopsId = nTroopId;
	m_mapTroopsInfo = info;
	ResetHeroPos();
}

FTransform AInstanceLevel::GetMonsterTransFlag(int nId)
{
	FTransform trans;

	FTransform transStart;
	GetPlayerStartTransform(transStart);
	FVector vecBack = transStart.GetRotation().GetForwardVector()*-1.f;

	auto&& pMapCfg = MapSettingCfg::getInstance().findMap(MapDataMgr::getInstance().getMapId());
	if (pMapCfg) {
		for (int i = 0; i < pMapCfg->m_vecInsMobs.size(); i++)
		{
			if (nId >= 1000 && nId /1000 == pMapCfg->m_vecInsMobs[i].m_nId) {
				int nPos = nId % 1000;
				FVector vec(pMapCfg->m_vecInsMobs[i].m_X, pMapCfg->m_vecInsMobs[i].m_Y, pMapCfg->m_vecInsMobs[i].m_Z);
				FVector vecTo = vec + FVector(vecBack.X*-400.f, vecBack.Y*-400.f, 0.f);
				FVector vecPos = CalculateGridPointPosition(vec, vecTo, 600, 300, nPos - 1);
				if (pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
					FRotator tor;
					tor.Yaw = pMapCfg->m_vecInsMobs[i].m_Yaw;
					FVector vecG = tor.Vector();
					vecTo = vec + vecG * 100.f;
					vecPos = CalculateGridPointPosition(vec, vecTo, 600, 300, nPos - 1);
					//vecTo = vec + FVector(vecBack.X * -400.f, vecBack.Y * -400.f, 0.f);
				}				
				vecPos.Z = pMapCfg->m_vecInsMobs[i].m_Z;
				trans.SetLocation(vecPos);
				FRotator rot = transStart.GetRotation().Rotator();
				if (pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
					rot.Yaw = pMapCfg->m_vecInsMobs[i].m_Yaw;
				}
				else {
					rot.Yaw += 180;
				}
				trans.SetRotation(rot.Quaternion());
				break;
			}
			else if (pMapCfg->m_vecInsMobs[i].m_nId == nId)
			{
				FVector vec(pMapCfg->m_vecInsMobs[i].m_X, pMapCfg->m_vecInsMobs[i].m_Y, pMapCfg->m_vecInsMobs[i].m_Z);
				vec.Z = pMapCfg->m_vecInsMobs[i].m_Z;
				trans.SetLocation(vec);
				FRotator rot = transStart.GetRotation().Rotator();
				if (pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
					rot.Yaw = pMapCfg->m_vecInsMobs[i].m_Yaw;
				}
				else {
					rot.Yaw += 180;
				}
				trans.SetRotation(rot.Quaternion());
				break;
			}
		}
	}

	return trans;
}

FTransform AInstanceLevel::GetMonsterTransNpcFlag(int64 uuid)
{
	FTransform transStart;
	FTransform trans;
	int nIdx = 0;
	AGenerals* gen = AGeneralsMgr::getInstance().GetGeneralByUuid(uuid);
	if (gen)
	{
		transStart = gen->GetActorTransform();
		nIdx = gen->GetSoliderNum();
	}
	else {
		GetPlayerStartTransform(transStart);
	}
	
	FVector vecBack = transStart.GetRotation().GetForwardVector();
	FVector oriPos = transStart.GetLocation();

	FVector vec(oriPos.X, oriPos.Y, oriPos.Z);
	FVector vecTo = vec + FVector(vecBack.X*-400.f, vecBack.Y*-400.f, 0.f);
	FVector vecPos = CalculateGridPointPosition(vec, vecTo, 600, 300, nIdx);
	vecPos.Z = oriPos.Z;
	trans.SetLocation(vecPos);
	FRotator rot = transStart.GetRotation().Rotator();
	trans.SetRotation(rot.Quaternion());

	if (gen)
	{
		gen->AddSoliderNum();
	}

	return trans;
}


void AInstanceLevel::ResetHeroPos()
{
	FTransform tran;
	auto&& hero = AIPlayerMgr::getInstance().GetHero();
	if (!hero)
	{
		return;
	}
	auto&& troopData = TroopsCfg::getInstance().GetTroopsType(m_nTroopsId);
	if (!troopData)
	{
		return;
	}

	tran = hero->GetTransform();
	m_heroTrans = tran;

	bool bCity = false;
	FString LevelName = this->GetOutermost()->GetName();
	FString CleanLevelName = FPaths::GetCleanFilename(LevelName);
	if (CleanLevelName.Contains("_city")) {
		bCity = true;
	}

	float fMaxTroop = 6000.f;
	auto&& pMapCfg = MapSettingCfg::getInstance().findMap(MapDataMgr::getInstance().getMapId());
	if (pMapCfg && pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
		fMaxTroop = 2000.f;
	}

	float perX = fMaxTroop / troopData->n_cop;
	float perY = fMaxTroop / troopData->n_cop;
	float fBeginX = tran.GetLocation().X + fMaxTroop/2;
	float fBeginY = tran.GetLocation().Y + fMaxTroop/2;

	if (bCity)
	{
		fBeginX = tran.GetLocation().X - fMaxTroop / 2;
		fBeginY = tran.GetLocation().Y - fMaxTroop / 2;
	}

	FVector vec = FVector(fBeginX - ((troopData->n_mainPos - 1) % troopData->n_cop) * perX,
		fBeginY - ((troopData->n_mainPos - 1) / troopData->n_cop) * perY, tran.GetLocation().Z);
	if (bCity)
	{
		vec = FVector(fBeginX + ((troopData->n_mainPos - 1) % troopData->n_cop) * perX,
			fBeginY + ((troopData->n_mainPos - 1) / troopData->n_cop) * perY, tran.GetLocation().Z);
	}
	hero->SetActorLocation(vec);
}

FTransform AInstanceLevel::GetTransByTroop(int64 uuid)
{
	FTransform tran = m_heroTrans;
	auto&& troopData = TroopsCfg::getInstance().GetTroopsType(m_nTroopsId);
	if (!troopData || troopData->n_cop == 0) {
		return tran;
	}

	bool bCity = false;
	FString LevelName = this->GetOutermost()->GetName();
	FString CleanLevelName = FPaths::GetCleanFilename(LevelName);
	if (CleanLevelName.Contains("_city")) {
		bCity = true;
	}

	float fMaxTroop = 6000.f;
	auto&& pMapCfg = MapSettingCfg::getInstance().findMap(MapDataMgr::getInstance().getMapId());
	if (pMapCfg && pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
		fMaxTroop = 2000.f;
	}

	float perX = fMaxTroop / troopData->n_cop;
	float perY = fMaxTroop / troopData->n_cop;
	float fBeginX = tran.GetLocation().X + fMaxTroop/2;
	float fBeginY = tran.GetLocation().Y + fMaxTroop/2;
	if (bCity) {
		fBeginX = tran.GetLocation().X - fMaxTroop/2;
		fBeginY = tran.GetLocation().Y - fMaxTroop/2;
	}

	auto&& iter = m_mapTroopsInfo.begin();
	for (; iter != m_mapTroopsInfo.end(); iter++)
	{
		if (iter->second == uuid)
		{
			FVector vec = FVector(fBeginX - ((iter->first) % troopData->n_cop) * perX,
				fBeginY - ((iter->first) / troopData->n_cop) * perY, tran.GetLocation().Z);
			if (bCity)
			{
				vec = FVector(fBeginX + ((iter->first) % troopData->n_cop) * perX,
					fBeginY + ((iter->first) / troopData->n_cop) * perY, tran.GetLocation().Z);
			}
			tran.SetLocation(vec);
		}
	}
	
	return tran;
}


//****************************************************************************************
//重置所有怪物的目标，AI系统不靠谱 Monster
//****************************************************************************************
void AInstanceLevel::InitMonsterTargets(AMonster* pMonster)
{
	auto&& generals = AGeneralsMgr::getInstance().GetMapGenerals();
	auto&& soliders = ASoldierMgr::getInstance().GetSoliders();

	AHero* pHero = AIPlayerMgr::getInstance().GetHero();

	bool bFollow = false;
	auto&& pMapCfg = MapSettingCfg::getInstance().findMap(MapDataMgr::getInstance().getMapId());
	float fDetRange = 200.f;
	if (pMapCfg && pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
		bFollow = true;
		fDetRange = 10;
	}
	
	ARole* pRole = Cast<ARole>(pMonster);
	if (!pRole || !pRole->GetDamageCollision() || pRole->IsDeath()) {
		return;
	}

	pRole->SetAIMoveAvodiance(true, 500.f);

	if (pMapCfg && pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
		return;
	}

	if (pMapCfg && pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_All
		&& pMonster->m_pMonsterData
		&& pMonster->m_pMonsterData->getMonsterType()
		&& pMonster->m_pMonsterData->getMonsterType()->n_nTowerTpy == MonsterCfg::MonsterType::MobTowerTypeDefTower) {
		return;
	}

	for (auto&& gen : generals)
	{
		if (pRole->IsCanAttackTarget(gen.second))
		{
			pRole->AddTarget(gen.second);
		}
	}

	for (auto&& solider : soliders)
	{
		if (pRole->IsCanAttackTarget(solider.second))
		{
			pRole->AddTarget(solider.second);
		}
	}

	if (pHero && pRole->IsCanAttackTarget(pHero))
	{
		pRole->AddTarget(pHero);
	}

	if (pHero && pHero->IsCanAttackTarget(pRole))
	{
		pHero->AddTarget(pRole);
	}

	pRole->GetDamageCollision()->MonsterRefocusRole(fDetRange);
}

void AInstanceLevel::InitGeneralsTargets(AGenerals* pGen)
{
	auto&& monsters = AMonsterMgr::getInstance().GetMonsters();
	//auto&& genGuards = AGeneralsGuardMgr::getInstance().GetMapGenerals();
	//auto&& soliGuards = ASoldierGuardMgr::getInstance().GetSoliders();

	AHero* pHero = AIPlayerMgr::getInstance().GetHero();

	bool bFollow = false;
	auto&& pMapCfg = MapSettingCfg::getInstance().findMap(MapDataMgr::getInstance().getMapId());
	float fDetRange = 200.f;
	if (pMapCfg && pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
		bFollow = true;
		fDetRange = 10;
	}

	ARole* pRole = Cast<ARole>(pGen);
	if (!pRole || !pRole->GetDamageCollision() || pRole->IsDeath()) {
		return;
	}
	pRole->SetAIMoveAvodiance(true, 500.f);
	for (auto&& monster : monsters)
	{
		if (pRole->IsCanAttackTarget(monster.second))
		{
			pRole->AddTarget(monster.second);
		}
	}

	pGen->SetFollowPlayer(bFollow);
	pRole->GetDamageCollision()->MonsterRefocusRole(fDetRange);
}

void AInstanceLevel::InitSoliderTargets(ASoldier* pSolider)
{
	auto&& monsters = AMonsterMgr::getInstance().GetMonsters();

	AHero* pHero = AIPlayerMgr::getInstance().GetHero();

	bool bFollow = false;
	auto&& pMapCfg = MapSettingCfg::getInstance().findMap(MapDataMgr::getInstance().getMapId());
	float fDetRange = 200.f;
	if (pMapCfg && pMapCfg->generaltype == MapSettingCfg::DunGeneral_Type_General) {
		bFollow = true;
		fDetRange = 10;
	}

	ARole* pRole = Cast<ARole>(pSolider);
	if (!pRole || !pRole->GetDamageCollision() || pRole->IsDeath()) {
		return;
	}
	pRole->SetAIMoveAvodiance(true, 500.f);
	for (auto&& monster : monsters)
	{
		if (pRole->IsCanAttackTarget(monster.second))
		{
			pRole->AddTarget(monster.second);
		}
	}

	pRole->GetDamageCollision()->MonsterRefocusRole(fDetRange);
}

//****************************************************************************************
//重置所有怪物的目标，AI系统不靠谱
//****************************************************************************************
void AInstanceLevel::InitAllTargets()
{
	auto&& monsters = AMonsterMgr::getInstance().GetMonsters();
	auto&& generals = AGeneralsMgr::getInstance().GetMapGenerals();
	auto&& soliders = ASoldierMgr::getInstance().GetSoliders();

	for (auto&& monster : monsters)
	{
		InitMonsterTargets(monster.second);
	}

	for (auto&& gen : generals)
	{
		InitGeneralsTargets(gen.second);
	}

	for (auto&& solider : soliders)
	{
		InitSoliderTargets(solider.second);
	}
}

void AInstanceLevel::SetStartBattle()
{
	InitAllTargets();
	m_bStartBattle = true;
	auto&& hero = AIPlayerMgr::getInstance().GetHero();
	if (!hero)
	{
		return;
	}
	int nTpy = hero->GetTouchSight();
	if (nTpy != TouchSightTpy_A)
	{
		hero->ResetTouchSight(nTpy, false);
	}
	
	if (m_bBuffStart) {
		int nBattleBuf = hero->GetBattleGenBuff();
		if (hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Infantry) && 
			hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Cavalry) &&
			hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Archer) &&
			hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Blade) &&
			hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Magic))
		{
			GeneralsSkillEvent evt;
			evt.m_type = GeneralsSkillEvent::EventType::Gen_StartBuff;
			UI_DISPATCH_MYEVENT(GeneralsSkillEvent, evt);
			ResetInfDefence();
			SetCavTime(Heartbeat::getInstance().getServerTimeMs() + 60 * 1000);
			SetArcTime(Heartbeat::getInstance().getServerTimeMs() + 60 * 1000);
			SetBlaTime(Heartbeat::getInstance().getServerTimeMs() + 60 * 1000);
			SetMagTime(Heartbeat::getInstance().getServerTimeMs() + 60 * 1000);
		}
		else {
			if (hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Infantry)) {
				GeneralsSkillEvent evt;
				evt.m_type = GeneralsSkillEvent::EventType::Gen_INF;
				UI_DISPATCH_MYEVENT(GeneralsSkillEvent, evt);
				ResetInfDefence();
			}
			if (hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Cavalry)) {
				GeneralsSkillEvent evt;
				evt.m_type = GeneralsSkillEvent::EventType::Gen_CAV;
				UI_DISPATCH_MYEVENT(GeneralsSkillEvent, evt);
				SetCavTime(Heartbeat::getInstance().getServerTimeMs() + 60 * 1000);
			}
			if (hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Archer)) {
				GeneralsSkillEvent evt;
				evt.m_type = GeneralsSkillEvent::EventType::Gen_ARC;
				UI_DISPATCH_MYEVENT(GeneralsSkillEvent, evt);
				SetArcTime(Heartbeat::getInstance().getServerTimeMs() + 60 * 1000);
			}
			if (hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Blade)) {
				GeneralsSkillEvent evt;
				evt.m_type = GeneralsSkillEvent::EventType::Gen_BLA;
				UI_DISPATCH_MYEVENT(GeneralsSkillEvent, evt);
				SetBlaTime(Heartbeat::getInstance().getServerTimeMs() + 60 * 1000);
			}
			if (hero->IsGenBuffState(NpcTypeCfg::NpcSubType_Magic)) {
				GeneralsSkillEvent evt;
				evt.m_type = GeneralsSkillEvent::EventType::Gen_MAG;
				UI_DISPATCH_MYEVENT(GeneralsSkillEvent, evt);
				SetMagTime(Heartbeat::getInstance().getServerTimeMs() + 60 * 1000);
			}
		}

		m_bBuffStart = false;
	}

	//auto&& pMapCfg = MapSettingCfg::getInstance().findMap(MapDataMgr::getInstance().getMapId());
	//if (pMapCfg && pMapCfg->dungeontype == MapSettingCfg::Convoy_Dungeon) {
	//	GeneralsArmySkillEvent evt1;
	//	UI_DISPATCH_MYEVENT(GeneralsArmySkillEvent, evt1);
	//}
}

void AInstanceLevel::SetEndBattle()
{
	InsEndEvent evt1;
	UI_DISPATCH_MYEVENT(InsEndEvent, evt1);
	m_bEndBattle = true;
}

void AInstanceLevel::DestroyDoorsAndTowers()
{
	auto&& hero = AIPlayerMgr::getInstance().GetHero();
	if (!hero)
	{
		return;
	}
	auto&& monsters = AMonsterMgr::getInstance().GetMonsters();
	for (auto&& monster : monsters)
	{
		AMonster* pMonster = Cast<AMonster>(monster.second);
		if (!pMonster || pMonster->IsDeath()) {
			continue;
		}

		if (!pMonster->m_pMonsterData || !pMonster->m_pMonsterData->getMonsterType()
			|| pMonster->m_pMonsterData->getMonsterType()->n_nTowerTpy != MonsterCfg::MonsterType::MobTowerTypeDefTower)
		{
			continue;
		}

		if (pMonster->m_pMonsterData) {
			pMonster->m_pMonsterData->setHp(0);
		}

		MsgReportKillMobRequest reportMsg;
		reportMsg.set_iggid(hero->m_playerData->getIggId());
		reportMsg.set_mob_type_id(pMonster->GetObjectId());
		reportMsg.set_map_type_id(hero->GetObjectId());
		GameAppExtern::sendMsg(reportMsg.msgtype(), reportMsg);
	}
}


//****************************************************************************************
//
//****************************************************************************************
void AInstanceLevel::onMsgDungeonBossChest_MS2C(const MsgDungeonBossChest_MS2C& msg)
{
	if (UIManager::getInstance().IsUIShowByType(WindowEvent::EventType::Window_InsChestSelect) ||
		m_bEndBattle) {
		return;
	}
	WindowEvent evt;
	evt.m_type = WindowEvent::EventType::Window_InsChestSelect;
	UI_DISPATCH_MYEVENT(WindowEvent, evt);

	SetEndBattle();
}

void AInstanceLevel::onMsgDungeonBossOpenChestRs_MS2C(const MsgDungeonBossOpenChestRs_MS2C& msg)
{

}

void AInstanceLevel::onMsgDungeonNextMob_S2C(const MsgDungeonNextMob_S2C& msg)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AInstanceLevel::InitAllTargets, 1.0f, false);

	GeneralsSkillEvent evt;
	evt.m_type = GeneralsSkillEvent::EventType::Gen_INF_C;
	UI_DISPATCH_MYEVENT(GeneralsSkillEvent, evt);
	ResetInfDefence();
}
