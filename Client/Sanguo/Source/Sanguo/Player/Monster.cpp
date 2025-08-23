// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Animation/AnimMontage.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "config/SkillTypeCfg.h"
#include "config/MonsterCfg.h"

#include "../AI/MonsterAI/MonsterAIController.h"
#include "IPlayer.h"
#include "Generals.h"
#include "Hero.h"
#include "IPlayer.h"
#include "Skill/SkillEffect.h"
#include "../Scene/InstanceLevel.h"
#include "../Scene/MainCityLevel.h"
#include "HorseInstance.h"
#include "../ReichGameInstance.h"


extern UWorld* gp_UWorld;

#define STATE_HOME_SPEED 1500

void AMonster::BeginPlay()
{
	m_bFocusPlayer = false;
	m_vecBornPos = GetActorLocation();
	SetCamp(CampState::CAMP_MONSTER);
	RegisterRoleClick();
	Super::BeginPlay();

	CreateMyCollision(100);

	UI_REGISTER_MYEVENT(SightEvent, &AMonster::onSightEvent);
	RG_REGISTER_MYEVENT(HandlerBattleEvent, &AMonster::onHandlerBattleEvent);
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::SetMonsterId(int mapObjId, int nInsId, int nMonsterId)
{
	m_nMapObjId = mapObjId;
	m_nInsId = nInsId;
	m_nMonsterId = nMonsterId;

	m_pMonsterData = MonsterDataMgr::getInstance().getMonster(mapObjId);

	if (!m_pMonsterData) {
		return;
	}

	if (m_pMonsterData->getMonsterType() && m_pMonsterData->getMonsterType()->n_rare == MonsterCfg::MonsterTypeRare::BOSS)
	{
		UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
		if (GameInstance) {
			AInstanceLevel* pLevel = Cast<AInstanceLevel>(GameInstance->GetCurrentLevel());
			if (pLevel) {
				pLevel->SetFocusBossId(mapObjId);
			}
		}
	}

	m_pMonsterData->calcAttr();
	ResetHud();
}

int AMonster::GetObjectId()
{
	if (m_pMonsterData) {
		return m_pMonsterData->getId();
	}
	return 0;
}


bool AMonster::IsDeath()
{
	if (!m_pMonsterData) {
		return false;
	}
	if (!m_pMonsterData->isInitialized()) {
		return true;
	}
	return m_pMonsterData->getHp() <= 0;
}

int AMonster::GetHp() 
{ 
	if (!m_pMonsterData) {
		return 0;
	}
	return m_pMonsterData->getHp();
}

int AMonster::GetMaxHp()
{
	if (!m_pMonsterData) {
		return 1;
	}
	return m_pMonsterData->getMaxHp();
}

int AMonster::GetRoleCtrlStatus()
{
	if (m_pMonsterData) {
		return m_pMonsterData->getCtrlState();
	}
	return 0;
}

bool AMonster::IsShowCtrlStatus()
{
	if (m_pMonsterData && m_pMonsterData->getMonsterType() &&
	   (m_pMonsterData->getMonsterType()->n_rare == MonsterCfg::MonsterTypeRare::BOSS
	   || m_pMonsterData->getMonsterType()->n_MoveType == MonsterCfg::MonsterType::MoveTypeNone))
	{
		return false;
	}
	return true;
}

void AMonster::ClearData()
{
	if (m_pMonsterData) {
		m_pMonsterData->clearInit();
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::StartAI()
{
	auto&& pMonCfg = MonsterCfg::getInstance().findMonsterType(m_nMonsterId);
	if (!pMonCfg)
	{
		return;
	}
	m_nIdleSpeed = pMonCfg->n_PartalSpeed;
	m_nIAlertSpeed = pMonCfg->n_FocusSpeed;
	m_nAttackDis = pMonCfg->n_AttackDis;
}

//****************************************************************************************
//
//****************************************************************************************
int AMonster::GetWalkSpeed()
{
	if (m_bReturnBornPos)
	{
		return m_nIAlertSpeed * 10;
	}
	return GetFocusRole() ? m_nIAlertSpeed : m_nIdleSpeed;
}

//****************************************************************************************
//
//****************************************************************************************
float AMonster::GetNormalAtkDistance()
{
	if (m_nAttackDis == 0) {
		return 200;
	}
	return m_nAttackDis;
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::onSightEvent(const SightEvent& evt)
{
	float value = evt.value;
	Sight(400 + value * 1500);
}

//****************************************************************************************
//
//****************************************************************************************
UAnimMontage* AMonster::LoadMonsterAniMontage(ControllerState nBehivor)
{
	auto&& pMonCfg = MonsterCfg::getInstance().findMonsterType(m_nMonsterId);
	if (!pMonCfg)
	{
		return NULL;
	}
	std::string path;
	if (nBehivor == ControllerState::E_ATTACK_RAND)
	{
		if (pMonCfg->n_AttackNum > 0)
		{
			int idx = std::rand() % pMonCfg->n_AttackNum;
			path = MonsterCfg::getInstance().GetAttackPath(m_nMonsterId , idx);
		}
	}
	else if (nBehivor == ControllerState::E_HURT)
	{
		path = MonsterCfg::getInstance().GetHitPath(m_nMonsterId);
	}
	else if (nBehivor == ControllerState::E_DEAD)
	{
		path = MonsterCfg::getInstance().GetDeathPath(m_nMonsterId);
	}
	else if (nBehivor >= ControllerState::E_S_ATTACK1 && nBehivor <= ControllerState::E_S_ATTACK5)
	{
		int nIdx = 0;
		if (nBehivor == ControllerState::E_S_ATTACK1) {
			nIdx = 0;
		}
		else if (nBehivor == ControllerState::E_S_ATTACK2) {
			nIdx = 1;
		}
		else if (nBehivor == ControllerState::E_S_ATTACK3) {
			nIdx = 2;
		}
		else if (nBehivor == ControllerState::E_S_ATTACK4) {
			nIdx = 3;
		}
		else if (nBehivor == ControllerState::E_S_ATTACK5) {
			nIdx = 4;
		}
		path = MonsterCfg::getInstance().GetAttackPath(m_nMonsterId, nIdx);
	}

	if (path.empty())
	{
		return NULL;
	}

	FString strPath(path.c_str());
	UObject* loadObj = StaticLoadObject(UAnimMontage::StaticClass(), NULL, strPath.GetCharArray().GetData());//);//TEXT("Blueprint'/Game/BluePrint/TestObj.TestObj'")
	if (loadObj != nullptr)
	{
		UAnimMontage* ubp = Cast<UAnimMontage>(loadObj);
		return ubp;
	}
	return NULL;
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::GetAight(float& fAtkSight, float& fLoseSight, float& fAtkAngle)
{
	if (!m_pMonsterData || !m_pMonsterData->getMonsterType()) {
		return;
	}
	//pMonCfg->n_AtkSight, pMonCfg->n_LoseSight, pMonCfg->n_AtkAngle
	fAtkSight = m_pMonsterData->getMonsterType()->n_AtkSight;
	fLoseSight = m_pMonsterData->getMonsterType()->n_LoseSight;
	fAtkAngle = m_pMonsterData->getMonsterType()->n_AtkAngle;

	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return;
	}

	ALevelBase* pLevel = GameInstance->GetCurrentLevel();
	if (!pLevel || pLevel->GetLevelTpy() != LevelType::LevelType_Dungeon)
	{
		return;
	}

	if (m_pMonsterData->getMonsterType()->n_nTowerTpy != MonsterCfg::MonsterType::MobTowerTypeDefTower) {
		return;
	}
	FString LevelName = pLevel->GetOutermost()->GetName();
	FString CleanLevelName = FPaths::GetCleanFilename(LevelName);
	if (CleanLevelName.Contains("south_general")) {
		fAtkSight = 25000.f;
		fLoseSight = 30000.f;
		fAtkAngle = 360.f;
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::PlayHit()
{
	auto aniMonta = LoadMonsterAniMontage(ControllerState::E_HURT);
	if (!aniMonta)
	{
		return;
	}
	StopAnimMontage(aniMonta);
	if (PlayAnimMontage(aniMonta) > 0.001f) {
		SetHitPlaying(true);
	}
}

//****************************************************************************************
//
//****************************************************************************************
bool AMonster::PlayAttack()
{
	ControllerState state = ControllerState::E_ATTACK_RAND;
	if (!m_bNormalAttack) {
		state = static_cast<ControllerState>(m_nSkillAniIdx);
	}
	auto aniMonta = LoadMonsterAniMontage(state);
	if (!aniMonta) {
		if (m_nPrepareSkillId > 0) {
			MagicAttack();
		}
		else {
			NormalAttack();
		}
		
		return false;
	}
	aniMonta->BlendIn.SetBlendTime(0.f);
	aniMonta->BlendOut.SetBlendTime(0.05f);
	StopAnimMontage();
	if (PlayAnimMontage(aniMonta, GetSpeedRate()) > 0.001f) {
		SetAttacking(true);
	} else {
		if (m_nPrepareSkillId > 0) {
			MagicAttack();
		}
		else {
			NormalAttack();
		}
	}

	if (!m_bNormalAttack && m_nPrepareSkillId > 0) {
		auto&& pSkill = SkillTypeCfg::getInstance().GetSkillType(m_nPrepareSkillId);
		if (pSkill && pSkill->f_CoolTime > 0.000001f) {
			AddSkillTime(m_nPrepareSkillId, pSkill->f_CoolTime);
		}
		else {
			AddSkillTime(m_nPrepareSkillId, 10.f);
		}
	}
	
	return true;
}

void AMonster::Death()
{
	if (!IsInMainCity())
	{
		if (m_pMonsterData && m_pMonsterData->getMonsterType() &&
			m_pMonsterData->getMonsterType()->n_nTowerTpy == MonsterCfg::MonsterType::MobTowerTypeCrystal) { //水晶死亡，其他的塔也要阵亡
			UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
			if (!GameInstance) {
				return;
			}

			AInstanceLevel* pLevel = Cast<AInstanceLevel>(GameInstance->GetCurrentLevel());
			if (pLevel) {
				pLevel->DestroyDoorsAndTowers();
			}
		}
		else if (m_pMonsterData && m_pMonsterData->getMonsterType() &&
			m_pMonsterData->getMonsterType()->n_nTowerTpy == MonsterCfg::MonsterType::MobTowerTypeNone) {
			AMonsterMgr::getInstance().RemoveMonster(GetObjectId());
		}
	}
	UnRiderHorse();
	FTimerHandle MyTimerHand;
	GetWorldTimerManager().SetTimer(MyTimerHand, this, &AMonster::DalayDeathDestroy, 5.0f, false);
	return Super::Death();
}

void AMonster::DalayDeathDestroy()
{
	AMonsterMgr::getInstance().RemoveMonster(this->GetObjectId());
}

void AMonster::UnRiderHorse()
{
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MOUNT"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (!mesh)
	{
		return;
	}

	USkeletalMeshComponent* meshMain = GetMesh();
	if (!meshMain)return;
	mesh->SetHiddenInGame(true);
	meshMain->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	UCapsuleComponent* capsuleComp = Cast<UCapsuleComponent>(this->GetComponentByClass(UCapsuleComponent::StaticClass()));
	if (capsuleComp) {
		meshMain->AttachToComponent(capsuleComp, FAttachmentTransformRules::KeepRelativeTransform, FName("None"));
	}
	auto&& horseAnim = Cast<UHorseInstance>(mesh->GetAnimInstance());
	if (horseAnim) {
		horseAnim->StopAudio();
	}
}


void AMonster::PlayDeath()
{
	auto aniMonta = LoadMonsterAniMontage(ControllerState::E_DEAD);
	if (!aniMonta)
	{
		return;
	}
	StopAnimMontage();
	PlayAnimMontage(aniMonta);
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::InitSkin()
{
	if (!m_pMonsterData || !m_pMonsterData->getMonsterType()) {
		return;
	}

	//皮肤
	USkeletalMeshComponent* mesh = GetMesh();
	if (!mesh) {
		return;
	}

	for (int i = 0; i < mesh->GetNumMaterials() && i < 10 && i < m_pMonsterData->getMonsterType()->vecTxtures.size(); i++)
	{
		auto skinMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
			TEXT("Material'/Game/Player/CommonMetrials/M_Player.M_Player'")));
		if (!skinMaterial) {
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::InitSkin equipMaterial false"));
			return;
		}
		auto dynaMaterial = UMaterialInstanceDynamic::Create(skinMaterial, this);
		if (!dynaMaterial)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::InitSkin CreateDynamicMaterialInstance false"));
			return;
		}

		std::string strT = MonsterCfg::getInstance().GetMonsterMatTxt(m_nMonsterId,
			m_pMonsterData->getMonsterType()->vecTxtures[i]);
		FString strText(strT.c_str());

		auto tmpTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(strText)));
		if (!tmpTexture)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::InitSkin CreateDynamicMaterialInstance tmpTexture false"));
			return;
		}
		dynaMaterial->SetTextureParameterValue(FName("Defuse"), tmpTexture);
		mesh->SetMaterial(i, dynaMaterial);
	}
}


//****************************************************************************************
//
//****************************************************************************************
void AMonster::InitMeshScale()
{
	if (!m_pMonsterData || !m_pMonsterData->getMonsterType() || m_pMonsterData->getMonsterType()->f_scale < 0.0001f) {
		return;
	}

	USkeletalMeshComponent* mesh = GetMesh();
	if (!mesh)
	{
		return;
	}
	FVector vecScale = mesh->GetRelativeScale3D();
	vecScale *= m_pMonsterData->getMonsterType()->f_scale;
	mesh->SetRelativeScale3D(vecScale);
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::ResetAttr()
{
	ResetSpeed();
	ResetAtkSpeed();
}

void AMonster::ResetSpeed()
{
	float speed = GetFocusRole() == nullptr ? m_nIdleSpeed : m_nIAlertSpeed;
	if (m_bStateHome) {
		speed = STATE_HOME_SPEED;
	} else if(GetFocusRole() && m_pMonsterData){
		float addPhase = m_pMonsterData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CHASE);
		speed += (addPhase - 1.f) * 180;
	}

	speed = std::max(50.f, std::min(1000.f, speed));

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent) {
		// 设置角色的移动速度
		MovementComponent->MaxWalkSpeed = speed; // 设置为300米/秒
	}
}

void AMonster::ResetAtkSpeed()
{

}


//****************************************************************************************
//
//****************************************************************************************
void AMonster::CalSkillCondition()
{
	m_bNormalAttack = true;
	m_nPrepareSkillId = 0;
	m_nSkillAniIdx = 0;

	if (!m_pMonsterData || !m_pMonsterData->getMonsterType()) {
		return;
	}

	m_nPrepareSkillId = m_pMonsterData->getMonsterType()->n_AttackSkill;

	for (int i = 0; i < m_pMonsterData->getMonsterType()->vecSkillCon.size(); i++)
	{
		auto con = m_pMonsterData->getMonsterType()->vecSkillCon[i];
		if (con.nSkillId == 0)
		{
			continue;
		}
		if (IsSkillCoolTime(con.nSkillId)) {
			continue;
		}
		if (con.nConTpy == static_cast<int>(MonsterCfg::SkillConType::SkillConType_HP) && m_pMonsterData->getMaxHp() > 0
			&& m_pMonsterData->getHp()*100.f / m_pMonsterData->getMaxHp() < con.nConValue)
		{
			int nRandVale = std::rand() % 100;
			if (nRandVale <= con.nConRate) {
				m_bNormalAttack = false;
				m_nPrepareSkillId = con.nSkillId;
				m_nSkillAniIdx = con.nAniIdx;
				return;
			}
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
bool AMonster::TestCanAttack()
{
	if (!GetFocusRole() || GetFocusRole()->IsDeath() || !CanActiveAttack())
	{
		return false;
	}

	if (m_pMonsterData && m_pMonsterData->getMonsterType()) {
		m_nAttackDis = m_pMonsterData->getMonsterType()->n_AttackDis;
	}
	

	CalSkillCondition();

	if (m_nPrepareSkillId > 0 && !CanSkillAttack()) { //无法技能攻击
		return false;
	}

	if (m_nPrepareSkillId == 0 && !CanAttack()) {  //无法主动攻击
		return false;
	}

	int nDis = 0;
	if (m_nPrepareSkillId > 0) {
		auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(m_nPrepareSkillId);
		if (pCfg && pCfg->n_nCastDis > 0) {
			nDis = pCfg->n_nCastDis;
		}
	} else {
		if (m_pMonsterData && m_pMonsterData->getMonsterType()) {
			nDis = m_pMonsterData->getMonsterType()->n_AttackDis;
		}
	}

	if (nDis == 0) {
		PlayAttack();
		return true;
	}

	FVector2D from = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	FVector2D to = FVector2D(GetFocusRole()->GetActorLocation().X, GetFocusRole()->GetActorLocation().Y);
	float dis = FVector2D::Distance(from, to);
	if (dis > nDis + 50.f) {
		m_nAttackDis = nDis;
		return false;
	}

	PlayAttack();
	return true;
}


//****************************************************************************************
//
//****************************************************************************************
void AMonster::SetReturnBorn(bool bRetBorn)
{
	if (bRetBorn)
	{
		ClearTargets();
		SetAttacking(false);
		SetHitPlaying(false);
		m_bReturnBornPos = true;
	}
	else {
		m_bReturnBornPos = false;
	}
}

//****************************************************************************************
//
//****************************************************************************************
bool AMonster::IsCanAttackTarget(ARole* pRole, bool bPassive)
{
	if (IsDeath() || !pRole || pRole->IsDeath())
	{
		return false;
	}
	if (pRole->GetCamp() == CampState::CAMP_HERO ||
		pRole->GetCamp() == CampState::CAMP_OthPlayer)
	{
		UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
		if (!GameInstance)
		{
			return false;
		}

		AMainCityLevel* pLevel = Cast<AMainCityLevel>(GameInstance->GetCurrentLevel());
		if (!pLevel)
		{
			return true;
		}

		auto&& pMonsterTpy = MonsterCfg::getInstance().findMonsterType(GetMonsterId());
		if (!pMonsterTpy) {
			return false;
		}

		if (!bPassive && pMonsterTpy->n_MobAI == MonsterCfg::PatrolBeAttack) {
			return false;
		}
		
		AGenerals* pGen = Cast<AGenerals>(pRole);
		if (pGen) {
			if (pGen->GetProtectorUuid() == 0)
			{
				return false;
			}

			auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(pGen->GetProtectorUuid());
			if (!data)
			{
				return false;
			}

			if (pMonsterTpy->n_MobAI == MonsterCfg::PatrolBeAttack && 
				!pLevel->IsSameBattleLevel(data->level, pMonsterTpy->n_MobLv))
			{
				return false;
			}
		}

		AIPlayer* pPlayer = Cast<AIPlayer>(pRole);
		if (pPlayer) {
			if (!pPlayer->m_playerData) {
				return false;
			}
		}
		return true;
	}
	return false;
}


//****************************************************************************************
//
//****************************************************************************************
bool AMonster::IsCanBeAttackTarget(ARole* pRole)
{
	if (IsDeath() || IsForbidAttack())
	{
		return false;
	}
	if (m_bStateHome)
	{
		return false;
	}
	return true;
}

bool AMonster::IsWaitBegin()
{
	if (IsInMainCity())
	{
		return false;
	}
	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return false;
	}

	AInstanceLevel* pLevel = Cast<AInstanceLevel>(GameInstance->GetCurrentLevel());
	if (!pLevel)
	{
		return false;
	}
	return !pLevel->IsStartBattle() || pLevel->IsEndBattle();
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::NormalAttack()
{
	ARole* pRole = GetFocusRole();
	if (!pRole)
	{
		return;
	}
	GameAppExtern::c2cNormalAttack(GetObjectId(), pRole->GetObjectId());
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::MagicAttack()
{
	if (!m_pMonsterData || !m_pMonsterData->getMonsterType()) {
		return;
	}
	if (m_nPrepareSkillId <= 0) {
		return;
	}
	SkillMgr::getInstance().CreateSkill(m_nPrepareSkillId, this,
		GetFocusRole(), FVector::ZeroVector, FVector::ZeroVector);
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::MagicAttackEnd()
{
	SkillStopContinueEvent evt;
	evt.m_nId = GetObjectId();
	UI_DISPATCH_MYEVENT(SkillStopContinueEvent, evt);
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::ResetWalkSpeed()
{
	if (m_bStateHome) {
		return;
	}
	auto speed = GetWalkSpeed();
	auto movement = GetCharacterMovement();
	if (movement) {
		movement->MaxWalkSpeed = speed;
	}
}

//****************************************************************************************
// 被动攻击的怪物在失去目标后，快速返回营地
//****************************************************************************************
void AMonster::StateHome()
{
	if (IsDeath()) {
		return;
	}

	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return;
	}

	AMainCityLevel* pLevel = Cast<AMainCityLevel>(GameInstance->GetCurrentLevel());
	if (!pLevel)
	{
		return;
	}

	auto&& pMonsterTpy = MonsterCfg::getInstance().findMonsterType(GetMonsterId());
	if (!pMonsterTpy) {
		return;
	}

	if (pMonsterTpy->n_MobAI != MonsterCfg::PatrolBeAttack) {
		return;
	}

	FVector vecTargetPos = pLevel->GetBattlePos(pMonsterTpy->n_MobLv);
	if (vecTargetPos == FVector::ZeroVector) {
		return;
	}

	float fDis = FVector::Distance(vecTargetPos, GetActorLocation());
	float fTm = fDis / STATE_HOME_SPEED;
	if (fTm < 0.001f) {
		return;
	}
	m_bStateHome = true;
	if (m_pMonsterData)  //打升级的怪不允许风筝
	{
		m_pMonsterData->setHp(m_pMonsterData->getMaxHp());
	}
	ResetAttr();
	FTimerHandle MyTimerHand;
	GetWorldTimerManager().SetTimer(MyTimerHand, this, &AMonster::HomeEnd, fTm + 1.5, false);
}

void AMonster::HomeEnd()
{
	if (!m_bStateHome) {
		return;
	}

	m_bStateHome = false;
	ResetAttr();
}

//****************************************************************************************
//
//****************************************************************************************
void AMonster::DestroyRole()
{
	DestroyAttach();
	return Super::DestroyRole();
}

//****************************************************************************************
//
//****************************************************************************************
bool AMonster::HitTest()
{
	AIPlayerMgr::getInstance().GetHero()->AddTarget(this, true);
	return true;
}

void AMonster::onHandlerBattleEvent(const HandlerBattleEvent& evt)
{
	//if (!m_pMonsterData)
	//{
	//	return;
	//}
	//if (evt.m_type == HandlerBattleEvent::EventType::BattleDamage)
	//{
	//	for (auto&& target : evt.targetInfo)
	//	{
	//		if (target.mapobjectid == m_pMonsterData->getId())
	//		{
	//			Damage(target.damage);
	//		}
	//	}
	//}
}

//****************************************************************************************
//
//****************************************************************************************
bool AMonster::CanMove()
{
	if (m_pMonsterData && m_pMonsterData->getMonsterType())
	{
		if (m_pMonsterData->getMonsterType()->n_MoveType == MonsterCfg::MonsterType::MoveTypeNone) {
			return false;
		}

		if (m_pMonsterData->getMonsterType()->n_rare == MonsterCfg::MonsterTypeRare::BOSS)
		{
			return true;
		}
	}
	return Super::CanMove();
}

//****************************************************************************************
//
//****************************************************************************************
bool AMonster::CanAttack()
{
	if (m_pMonsterData && m_pMonsterData->getMonsterType() &&
		m_pMonsterData->getMonsterType()->n_rare == MonsterCfg::MonsterTypeRare::BOSS)
	{
		return true;
	}
	return Super::CanAttack();
}

bool AMonster::CanSkillAttack()
{
	if (m_pMonsterData && m_pMonsterData->getMonsterType() &&
		m_pMonsterData->getMonsterType()->n_rare == MonsterCfg::MonsterTypeRare::BOSS)
	{
		return true;
	}
	return Super::CanSkillAttack();
}

bool AMonster::CanActiveAttack()
{
	if (m_pMonsterData && m_pMonsterData->getMonsterType() &&
		m_pMonsterData->getMonsterType()->n_CanAttack == 0)
	{
		return false;
	}
	return Super::CanActiveAttack();
}

bool AMonster::IsForbidAttack()
{
	if (m_pMonsterData && m_pMonsterData->getMonsterType() &&
		m_pMonsterData->getMonsterType()->n_ForbidAtk == 1)
	{
		return true;
	}
	return Super::IsForbidAttack();
}

bool AMonster::IsOnlyBeNormalAtk()
{
	if (m_pMonsterData && m_pMonsterData->getMonsterType() &&
		m_pMonsterData->getMonsterType()->n_IsOnlyNormalAtk == 1)
	{
		return true;
	}
	return Super::IsForbidAttack();
}

//****************************************************************************************
//
//****************************************************************************************
std::string AMonster::GetBattleSound(int nSoundTpy)
{
	if (!AIPlayerMgr::getInstance().GetHero()) {
		return "";
	}
	FVector pos = AIPlayerMgr::getInstance().GetHero()->GetActorLocation();
	if (FVector::Distance(GetActorLocation(), pos) >= 2000) {
		return "";
	}

	if (m_pMonsterData) {
		return m_pMonsterData->GetBattleSound(nSoundTpy);
	}
	return "";
}

//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
bool AMonsterMgr::initialize()
{
	return true;
}

void AMonsterMgr::terminate()
{

}


void AMonsterMgr::PreReset()
{
	auto iter = m_monsters.begin();
	for (; iter != m_monsters.end(); iter++)
	{
		iter->second->Reset();
	}
}

bool AMonsterMgr::reset()
{
	m_monsters.clear();
	return true;
}

void AMonsterMgr::update(float dt)
{

}

AMonsterMgr::AMonsterMgr()
{
	initialize();
}

AMonsterMgr& AMonsterMgr::getInstance()
{
	static AMonsterMgr playerMgr;
	return playerMgr;
}


void AMonsterMgr::CreateMonster(int nMapObjectId, int nInsId, int mMonsterId, FTransform trans)
{
	if (m_monsters.find(nMapObjectId) != m_monsters.end()){
		return;
	}
	//副本编辑时
	auto&& monsterData = MonsterDataMgr::getInstance().getMonster(nMapObjectId);
	if (!monsterData) {
		return;
	}
	std::string strBP = MonsterCfg::getInstance().GetBPMonsterPath(mMonsterId);
	if (strBP.empty())
	{
		return;
	}
	FString strPath(strBP.c_str());
	UClass* generalClass = LoadObject<UClass>(NULL, *strPath);
	if (!generalClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("MonsterMgr::CreateMonster failed"));
		return;
	}
	if (!gp_UWorld)
	{
		return;
	}
	FActorSpawnParameters params = FActorSpawnParameters();
	params.bNoFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AMonster* pMonster = gp_UWorld->SpawnActor<AMonster>(generalClass, trans, params);
	if (pMonster)
	{
		pMonster->SetMonsterId(nMapObjectId, nInsId, mMonsterId);
		pMonster->InitSkin();
		pMonster->InitMeshScale();
		m_monsters[nMapObjectId] = pMonster;
	}
}

void AMonsterMgr::RemoveMonster(int nMapObjId)
{
	auto&& iter = m_monsters.find(nMapObjId);
	if (iter != m_monsters.end() && iter->second != nullptr) {
		iter->second->DestroyRole();
	}
	m_monsters.erase(nMapObjId);
}

AMonster* AMonsterMgr::GetMonster(int nObjId)
{
	if (m_monsters.find(nObjId) == m_monsters.end()) {
		return nullptr;
	}
	return m_monsters[nObjId];
}

