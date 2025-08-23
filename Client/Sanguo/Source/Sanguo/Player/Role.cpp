// Fill out your copyright notice in the Description page of Project Settings.


#include "Role.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Blueprint.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

//#include "Cpp11EventSystem.h"
#include "Engine/World.h"

#include "common/Heartbeat.h"

#include "../Scene/LevelBase.h"
#include "config/BuffTypeCfg.h"
#include "BuffEffect.h"
#include "WeaponEffect.h"
#include "Hero.h"
#include "IPlayer.h"
#include "Monster.h"
#include "Generals.h"

#include "../ReichGameInstance.h"


unsigned int g_nRoleId = 100000;

extern UWorld* gp_UWorld;

ARole* g_clickRole = NULL;

// Sets default values
ARole::ARole()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_bFighting = false;
}

// Called when the game starts or when spawned
void ARole::BeginPlay()
{
	Super::BeginPlay();

	UI_REGISTER_MYEVENT(DeathEvent, &ARole::onDeathEvent);
	UI_REGISTER_MYEVENT(BeatBackEvent, &ARole::onBeatBackEvent);

	RG_REGISTER_MYEVENT(HandlerRoleBuffEvent, &ARole::onHandlerRoleBuffEvent);
	RG_REGISTER_MYEVENT(HandlerRoleStatusEvent, &ARole::onHandlerRoleStatusEvent);

	RG_REGISTER_MYEVENT(MsgMapObjectUnderAttackG2C, &ARole::onMsgMapObjectUnderAttackG2C);
	RG_REGISTER_MYEVENT(MsgPlayerNormalAttackResS2C, &ARole::onMsgPlayerNormalAttackResS2C);
	RG_REGISTER_MYEVENT(MsgSkillEffectDotDmgMS2C, &ARole::onMsgSkillEffectDotDmgMS2C);
	RG_REGISTER_MYEVENT(MsgSkillEffectDmgMS2C, &ARole::onMsgSkillEffectDmgMS2C);
	RG_REGISTER_MYEVENT(MsgCharacterDeadS2C, &ARole::onMsgCharacterDeadS2C);
	RG_REGISTER_MYEVENT(MsgSkillEffectRestoreMS2C, &ARole::onMsgSkillEffectRestoreMS2C);
	m_pAIControll = Cast<ARoleAIController>(GetController());
}

void ARole::Reset()
{
	UI_UN_REGISTER_MYEVENT(DeathEvent, this);
	UI_UN_REGISTER_MYEVENT(BeatBackEvent, this);
	if (m_pAIControll) {
		m_pAIControll->SetBlackBoolValue(ISDEAD, true);
		m_pAIControll->StopBehaviorTree();
	}

	if (m_aRangeCollion) {
		m_aRangeCollion->Reset();
		m_aRangeCollion->Destroy();
		m_aRangeCollion = nullptr;
	}
}

void ARole::DestroyRole()
{
	Reset();
	m_fDieTm = 1.5f;
}

// Called every frame
void ARole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (std::map<int, NS::I64>::iterator iter = m_mapSkillCoolTime.begin(); iter != m_mapSkillCoolTime.end(); iter++)
	{
		if (iter->second <= Heartbeat::getInstance().getServerTimeMs()) {
			m_mapSkillCoolTime.erase(iter);
			break;
		}
	}
	if (m_fDieTm > 0.000001f) {
		m_fDieTm -= DeltaTime;
		if (m_fDieTm <= 0.000001f) {
			ClearData();
			Destroy();
		}
	}
}

// Called to bind functionality to input
void ARole::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//****************************************************************************************
//
//****************************************************************************************
void ARole::Init()
{
	m_campState = CampState::CAMP_MIDDLE;
}

bool ARole::IsInMainCity()
{
	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return false;
	}

	ALevelBase* pLevel = GameInstance->GetCurrentLevel();
	if (!pLevel || pLevel->GetLevelTpy() != LevelType::LevelType_MainCity)
	{
		return false;
	}
	return true;
}

void ARole::CreateMyCollision(int nR)
{
	UCapsuleComponent* mesh = Cast<UCapsuleComponent>(this->GetComponentByClass(UCapsuleComponent::StaticClass()));
	if (!mesh)
	{
		return;
	}

	UClass* colliClass = LoadObject<UClass>(NULL, TEXT("Blueprint'/Game/Models/BP_MyCollicosion.BP_MyCollicosion_C'"));
	if (colliClass) {
		AMyCollision* spawnActor = GetWorld()->SpawnActor<AMyCollision>(colliClass);
		if (spawnActor) {
			spawnActor->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			spawnActor->SetOwnerRole(this);
			spawnActor->Sight(nR);
			m_aRangeCollion = spawnActor;
		}
	}
}

void ARole::onDeathEvent(const DeathEvent& evt)
{
	if (evt.m_nId == GetObjectId())
	{
		return;
	}
	if (m_aRangeCollion && m_aRangeCollion->RemoveId(evt.m_nId)) {
		AHero* pHero = Cast<AHero>(this);
		if (pHero)
		{
			pHero->ResetFocusObject();
		}
	}
}

void ARole::onBeatBackEvent(const BeatBackEvent& evt)
{
	BeatBack(evt.m_X, evt.m_Y);
}

//****************************************************************************************
//
//****************************************************************************************
FTransform ARole::GetHitWorldTransform()
{
	USkeletalMeshComponent* mesh = GetMesh();
	if (mesh){
		if (mesh->GetSocketByName(TEXT("Hit_pos"))  == NULL)
		{
			return GetActorTransform();
		}
		return mesh->GetSocketTransform(TEXT("Hit_pos"));
	}
	return GetActorTransform();
}

//****************************************************************************************
//
//****************************************************************************************
bool ARole::GetIsFighting()
{
	return m_bFighting;
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::SetIsFighting(bool bFighting)
{
	m_bFighting = bFighting;
}


//测试怪物和玩家的视野角度，目前不是很何时
bool ARole::IsInCameraView()
{
	if (this == AIPlayerMgr::getInstance().GetHero()) {
		return true;
	}

	FVector vec = GetActorTransform().GetLocation() - AIPlayerMgr::getInstance().GetHero()->GetActorTransform().GetLocation();
	vec.Normalize(0.0001);

	FVector vecCamera = AIPlayerMgr::getInstance().GetHero()->GetActorTransform().GetLocation() -
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();

	vecCamera.Normalize(0.0001);

	float fCose = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(vec, vecCamera));
	float fCameraAngle = 65;//默认65度算 GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetFOVAngle() / 2.f;
	if (fCose <= fCameraAngle)
	{
		return true;
	}

	return false;
}

//****************************************************************************************
// bDot:是否是Buff伤害
//****************************************************************************************
void ARole::Damage(int castId, int nHurtDamage, bool bCrit, bool bDot)
{
	int nHeroObjectId = 0;
	auto&& hero = AIPlayerMgr::getInstance().GetHero();
	if (hero)
	{
		nHeroObjectId = hero->GetObjectId();
	}

	BeAttackAnger(nHurtDamage);

	if (IsDeath()) {
		return;
	}
}

//****************************************************************************************
// 设置状态
//****************************************************************************************
void ARole::SetFreeze(bool bFreeze)
{
	if (bFreeze == m_bFreeze) {
		return;
	}

	m_bFreeze = bFreeze;
	USkeletalMeshComponent* mesh = GetMesh();
	if (mesh) {
		mesh->bPauseAnims = m_bFreeze;
		StopAnimMontage();
	}
}

bool ARole::IsFreeze() const
{
	return m_bFreeze;
}

void ARole::SetDizziess(bool bDizziess)
{
	if (m_bDizziness == bDizziess)
	{
		return;
	}
	m_bDizziness = bDizziess;
	StopAnimMontage();
}

bool ARole::IsDizziess() const
{
	return m_bDizziness;
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::Death()
{
	if (m_bHasPlayDeath) {
		return;
	}
	PlayDeath();
	ClearBuff();
	
	if (m_aRangeCollion)
	{
		m_aRangeCollion->Reset();
	}
	if (m_pAIControll) {
		m_pAIControll->SetBlackBoolValue(ISDEAD, true);
		m_pAIControll->SetBlackBoolValue(FIND_TARGET, false);
		m_pAIControll->StopMovement();
	}
	DeathEvent evt;
	evt.m_nId = GetObjectId();
	UI_DISPATCH_MYEVENT(DeathEvent, evt);

	if (Cast<AMonster>(this)) {
		UNRG_REGISTER_MYEVENT(MsgMapObjectUnderAttackG2C, this);
		UNRG_REGISTER_MYEVENT(MsgPlayerNormalAttackResS2C, this);
		UNRG_REGISTER_MYEVENT(MsgSkillEffectDotDmgMS2C, this);
		UNRG_REGISTER_MYEVENT(MsgSkillEffectDmgMS2C, this);
		UNRG_REGISTER_MYEVENT(MsgCharacterDeadS2C, this);
		UNRG_REGISTER_MYEVENT(MsgSkillEffectRestoreMS2C, this);
	}
	
	ShowHighLightMesh(false);

	std::string strSound = GetBattleSound(SOUND_TPY_DEAD);
	if (!strSound.empty()) {
		FString strPath = strSound.c_str();
		UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
		if (soundObj)
		{
			UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
		}
	}

	m_bHasPlayDeath = true;
}

void ARole::Revive()
{
	m_bHasPlayDeath = false;
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::ShowHighLightMesh(bool bShow)
{
	TArray<USkeletalMeshComponent*> meshs;
	this->GetComponents<USkeletalMeshComponent>(meshs);
	if (meshs.Num() <= 0)
	{
		return;
	}

	for (int i = 0 ; i < meshs.Num() ; i++)
	{
		if (meshs[i])
		{
			meshs[i]->SetRenderCustomDepth(bShow);
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
bool ARole::CanMove()
{
	if (m_bAttackStopMove) {
		return false;
	}
	int nStatus = GetRoleCtrlStatus();
	if ((nStatus & SKILL_STATUS_CANT_MOVE) || (nStatus & SKILL_STATUS_FREEZE) || (nStatus & SKILL_STATUS_CONFUSION)
		|| (nStatus & SKILL_STATUS_PETRIFACTION) || (nStatus & SKILL_STATUS_BLIND) || (nStatus & SKILL_STATUS_DREAD))
	{
		return false;
	}
	return true;
}

//****************************************************************************************
//
//****************************************************************************************
bool ARole::CanAttack()
{
	int nStatus = GetRoleCtrlStatus();

	if ((nStatus & SKILL_STATUS_CANT_ATTACK) || (nStatus & SKILL_STATUS_FREEZE) || (nStatus & SKILL_STATUS_CONFUSION)
		|| (nStatus & SKILL_STATUS_PETRIFACTION) || (nStatus & SKILL_STATUS_BLIND) || (nStatus & SKILL_STATUS_DREAD))
	{
		return false;
	}
	return true;
}

bool ARole::CanSkillAttack()
{
	int nStatus = GetRoleCtrlStatus();

	if ((nStatus & SKILL_STATUS_CANT_CASTSKILL) || (nStatus & SKILL_STATUS_FREEZE) || (nStatus & SKILL_STATUS_CONFUSION)
		|| (nStatus & SKILL_STATUS_PETRIFACTION) || (nStatus & SKILL_STATUS_BLIND) || (nStatus & SKILL_STATUS_DREAD))
	{
		return false;
	}
	return true;
}

void ARole::HitEffect(const char* pszEffect)
{
	if (!pszEffect) {
		return;
	}

	FTransform trans = GetTransform();
	FString strPath(pszEffect);

	UClass* generalClass = LoadObject<UClass>(NULL, *strPath);
	if (!generalClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttackEffect failed"));
		return;
	}

	AWeaponEffect* pActEff = GetWorld()->SpawnActor<AWeaponEffect>(generalClass, trans);
	if (pActEff)
	{
		pActEff->DieDelay(0.5f);
	}
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::AddBuff(int nBuffId)
{
	auto&& mapBuffs = GetBuffs();
	if (mapBuffs.find(nBuffId) != mapBuffs.end())
	{
		//if (mapBuffs[nBuffId]) {
		//	ABuffEffect* pEffect = Cast<ABuffEffect>(mapBuffs[nBuffId]);
		//	if (pEffect)
		//	{
		//	}
		//	pEffect->RestStatus();
		//}
		return;
	}
	auto&& pCfg = BuffTypeCfg::getInstance().GetBuffType(nBuffId);
	if (!pCfg)
	{
		return;
	}

	if (pCfg->s_MeshTag.empty())
	{
		return;
	}

	auto mesh = GetMesh();
	if (!mesh)return;

	FString strSock(pCfg->s_SockName.c_str());
	std::string strBuff = BuffTypeCfg::getInstance().GetBuffPath() + "/" + pCfg->s_path;
	FString path = UReichGameInstance::TransClassAssertPath(strBuff);
	UClass* buffClass = LoadObject<UClass>(NULL, *path);
	if (!buffClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::buffclass failed"));
		return;
	}

	ABuffEffect* effctActor = GetWorld()->SpawnActor<ABuffEffect>(buffClass);
	if (!effctActor)
	{
		return;
	}

	if (mesh->GetSocketByName(FName(*strSock)) == NULL)
	{
		effctActor->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
		effctActor->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	} else {
		effctActor->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*strSock));
	}
	
	effctActor->StartBuff(this, nBuffId);
	AddBuffEff(nBuffId, effctActor);
}

void ARole::AddBuffEff(int nBuffId, AActor* eff)
{
	if (m_mapBuffEffects.find(nBuffId) == m_mapBuffEffects.end() && eff)
	{
		m_mapBuffEffects[nBuffId] = eff;
	}
}

void ARole::RemoveBuff(int nBuffId)
{
	auto&& iter = m_mapBuffEffects.find(nBuffId);
	if (iter != m_mapBuffEffects.end()) {
		iter->second->Destroy();
		m_mapBuffEffects.erase(nBuffId);
	}
}

void ARole::ClearBuff()
{
	auto&& iter = m_mapBuffEffects.begin();
	for (; iter != m_mapBuffEffects.end(); iter++)
	{
		iter->second->Destroy();
	}
	m_mapBuffEffects.clear();
}

FTransform ARole::GetWeaponTransByName(FString strSockName)
{
	FTransform trans;
	return trans;
}

//****************************************************************************************
//
//****************************************************************************************
FTransform ARole::GetRoleTransByName(const char* pszSockName)
{
	if (pszSockName == NULL)
	{
		return GetActorTransform();
	}
	FString strSockName(pszSockName);
	if (strSockName.IsEmpty())
	{
		return GetActorTransform();
	}

	FTransform transWeapon = GetWeaponTransByName(strSockName);
	if (transWeapon.GetLocation() != FVector::ZeroVector) {
		auto roleTrans = GetActorTransform();
		transWeapon.SetRotation(roleTrans.GetRotation());
		return transWeapon;
	}

	TArray<UActorComponent*> meshs = this->K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	if (meshs.Num() > 0)
	{
		for (int i = 0; i < meshs.Num(); i++)
		{
			auto mesh = Cast<USkeletalMeshComponent>(meshs[i]);
			if (mesh && mesh->GetSocketByName(*strSockName))
			{
				auto trans = mesh->GetSocketTransform(*strSockName);
				auto roleTrans = GetActorTransform();;
				trans.SetRotation(roleTrans.GetRotation());
				return trans;
			}
		}
	}

	TArray<UActorComponent*> staMeshs = this->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
	if (staMeshs.Num() > 0)
	{
		for (int i = 0; i < staMeshs.Num(); i++)
		{
			auto mesh = Cast<UStaticMeshComponent>(staMeshs[i]);
			if (mesh && mesh->GetSocketByName(*strSockName))
			{
				auto trans = mesh->GetSocketTransform(*strSockName);
				auto roleTrans = GetActorTransform();;
				trans.SetRotation(roleTrans.GetRotation());
				return trans;
			}
		}
	}
	
	return GetActorTransform();
}


//****************************************************************************************
//
//****************************************************************************************
void ARole::SetFocusRole(ARole* aRole)
{ 
	if (!m_aRangeCollion) {
		return;
	}
	m_aRangeCollion->AddTargetRole(aRole);
}

ARole* ARole::GetFocusRole()
{
	if (!m_aRangeCollion) {
		return nullptr;
	}
	return m_aRangeCollion->GetFocusRole();
}

void ARole::AddTarget(ARole* pRole, bool bFocus)
{
	if (!pRole || pRole->IsDeath() || !m_aRangeCollion){
		return;
	}
	m_aRangeCollion->AddTargetRole(pRole);
	if (GetFocusRole() == NULL || bFocus) {
		SetFocusRole(pRole);
	}
	//ResetAttr();
	if (m_pAIControll)
	{
		if (GetFocusRole() == nullptr) {
			m_pAIControll->SetBlackBoolValue(FIND_TARGET, false);
		}
		else {
			m_pAIControll->SetBlackBoolValue(FIND_TARGET, true);
		}
	}
}

bool ARole::HasTarget(ARole* pRole)
{
	if (!m_aRangeCollion || !pRole) {
		return false;
	}
	return m_aRangeCollion->HasTargetRole(pRole);
}

bool ARole::RemoveTarget(ARole* pRole)
{
	if (!m_aRangeCollion || !pRole) {
		return false;
	}
	m_aRangeCollion->RemoveTargetRole(pRole);
	return true;
}

bool ARole::ClearTargets()
{
	if (!m_aRangeCollion) {
		return false;
	}
	bool bPreFocus = GetFocusRole() != nullptr;
	m_aRangeCollion->Reset();
	if (m_pAIControll) {
		m_pAIControll->SetBlackBoolValue(FIND_TARGET, false);
	}
	if (bPreFocus) {
		StateHome();
	}
	ResetAttr();
	return true;
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::RegisterRoleClick()
{
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(UBoxComponent::StaticClass(), TEXT("TouchBox"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	auto StaticMesh = Cast<UBoxComponent>(meshs[0]);
	if (!StaticMesh)
	{
		return;
	}

	if (StaticMesh) {
		FScriptDelegate sgbDelegate;
		sgbDelegate.BindUFunction(this, "RoleClicked");
		StaticMesh->OnClicked.Add(sgbDelegate);

		FScriptDelegate sgbDelegate1;
		sgbDelegate1.BindUFunction(this, "RoleTouchBegin");
		StaticMesh->OnInputTouchBegin.Add(sgbDelegate1);

		FScriptDelegate sgbDelegate2;
		sgbDelegate2.BindUFunction(this, "RoleTouchEnd");
		StaticMesh->OnInputTouchEnd.Add(sgbDelegate2);
	}
}

void ARole::RoleClicked()
{
	UE_LOG(LogOutputDevice, Log, TEXT("AMonster::RoleClicked ---------------"));
	HitTest();
}

void ARole::RoleTouchBegin()
{
	UE_LOG(LogOutputDevice, Log, TEXT("AMonster::RoleTouchBegin ---------------"));
	g_clickRole = this;
}

void ARole::RoleTouchEnd()
{
	UE_LOG(LogOutputDevice, Log, TEXT("AMonster::RoleTouchEnd ---------------"));
	if (g_clickRole == this)
	{
		HitTest();
	}
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::SetMaterialScalerParamer(FName name, float fValue)
{
	USkeletalMeshComponent* mesh = GetMesh();
	if (!mesh) {
		return;
	}
	for (int i = 0 ; i < mesh->GetNumMaterials() ; i++)
	{
		auto meterial = mesh->GetMaterial(i);
		if (!meterial)
		{
			return;
		}
		UMaterialInstanceDynamic* dynaMeterial = Cast<UMaterialInstanceDynamic>(meterial);
		if (dynaMeterial)
		{
			dynaMeterial->SetScalarParameterValue(name, fValue);
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::SetMaterialVectorParamer(FName name, FLinearColor colorParamer)
{
	USkeletalMeshComponent* mesh = GetMesh();
	if (!mesh) {
		return;
	}
	for (int i = 0; i < mesh->GetNumMaterials(); i++)
	{
		auto meterial = mesh->GetMaterial(i);
		if (!meterial)
		{
			return;
		}
		UMaterialInstanceDynamic* dynaMeterial = Cast<UMaterialInstanceDynamic>(meterial);
		if (dynaMeterial)
		{
			dynaMeterial->SetVectorParameterValue(name, colorParamer);
		}
	}
}



float ARole::GetSpeedRate() const
{
	float fRate = m_nAtkSpeedRate * 1.f / 100;
	fRate = fRate < 0.1f ? 0.1f : fRate > 10.f ? 10.f : fRate;
	return fRate;
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::AddSkillTime(int nSkillId, float tm)
{
	if (tm <= 0.000001f)
	{
		return;
	}
	NS::I64 nHH = Heartbeat::getInstance().getServerTimeMs();

	m_mapSkillCoolTime[nSkillId] = nHH + (int)(tm*1000);
	if (AIPlayerMgr::getInstance().GetHero() == this) {
		SkillCoolTimeEvent evt;
		UI_DISPATCH_MYEVENT(SkillCoolTimeEvent, evt);
	}
}

float ARole::GetSkillCoolTime(int nSkillId)
{
	if (m_mapSkillCoolTime.find(nSkillId) != m_mapSkillCoolTime.end())
	{
		NS::I64 n64Less = m_mapSkillCoolTime[nSkillId];
		NS::I64 nHH = Heartbeat::getInstance().getServerTimeMs();
		NS::I64 n = n64Less - nHH;
		return n * 1.f/1000;
	}
	return 0.f;
}

int ARole::GetCurrentLevelTpy()
{
	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return (int)LevelType::LevelType_MainCity;
	}
	ALevelBase* pLevel = GameInstance->GetCurrentLevel();
	if (!pLevel)
	{
		return (int)LevelType::LevelType_MainCity;
	}
	return (int)pLevel->GetLevelTpy();
}


//****************************************************************************************
//
//****************************************************************************************
void ARole::LookHero()
{
	auto&& aHero = AIPlayerMgr::getInstance().GetHero();
	if (!aHero || aHero == this)
	{
		return;
	}

	bool bMeshLook = false;
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MainMesh"));
	if (meshs.Num() > 0)
	{
		USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(meshs[0]);
		if (mesh) { // && mesh->GetSocketByName(TEXT("S_OriLook")) != nullptr
			auto lookRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), aHero->GetActorLocation());
			m_rotOri = mesh->GetRelativeRotation();
			auto preRot = m_rotOri;

			preRot.Yaw = lookRot.Yaw - (m_rotOri.Yaw + GetActorRotation().Yaw) + 180;
			mesh->SetRelativeRotation(preRot);
			bMeshLook = true;
		}
	}
	if(!bMeshLook) {
		auto lookRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), aHero->GetActorLocation());
		m_rotOri = this->GetActorRotation();
		auto preRot = m_rotOri;

		preRot.Yaw = lookRot.Yaw;
		this->SetActorRotation(preRot);
	}
	m_bTalking = true;
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::ResetLookForward()
{
	bool bMeshLook = false;
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MainMesh"));
	if (meshs.Num() > 0)
	{
		USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(meshs[0]);
		if (mesh) {
			mesh->SetRelativeRotation(m_rotOri);
			bMeshLook = true;
		}
	} 
	if(!bMeshLook) {
		this->SetActorRotation(m_rotOri);
	}

	m_bTalking = false;
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::LookAtTarget()
{
	if (GetFocusRole() != nullptr)
	{
		auto from = GetActorLocation();
		auto to = GetFocusRole()->GetActorLocation();
		auto toRota = UKismetMathLibrary::FindLookAtRotation(from, to);
		FRotator NewRotation(GetActorRotation().Pitch, toRota.Yaw, GetActorRotation().Roll);
		SetActorRotation(NewRotation);
	}
}

//****************************************************************************************
//
//****************************************************************************************
bool ARole::IsTalking()
{
	return m_bTalking;
}

void ARole::DestroyAttach()
{
	TArray<AActor*> arrayActors;
	this->GetAttachedActors(arrayActors);
	if (arrayActors.Num() <= 0)
	{
		return;
	}
	for (int i = 0; i < arrayActors.Num(); i++)
	{
		arrayActors[i]->Destroy();
	}
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::onHandlerRoleBuffEvent(const HandlerRoleBuffEvent& evt)
{
	if (evt.objId == GetObjectId()) {
		if (evt.m_type == HandlerRoleBuffEvent::EventType::EventType_Buff_Add)
		{
			AddBuff(evt.buffId);
		}
		else if (evt.m_type == HandlerRoleBuffEvent::EventType::EventType_Buff_Del) {
			RemoveBuff(evt.buffId);
		}
	}
	ResetAttr();
}

void ARole::onHandlerRoleStatusEvent(const HandlerRoleStatusEvent& evt)
{
	if (evt.objId != GetObjectId() || !IsShowCtrlStatus()) {
		return;
	}

	int nStatus = GetRoleCtrlStatus();

	if ((nStatus & SKILL_STATUS_FREEZE) || (nStatus & SKILL_STATUS_PETRIFACTION)) {
		SetMaterialScalerParamer(FName(TEXT("FreezeValue")), 0.5f);
		SetFreeze(true);
	}
	else {
		SetMaterialScalerParamer(FName(TEXT("FreezeValue")), 0.f);
		SetFreeze(false);
	}

	if ((nStatus & SKILL_STATUS_CANT_ATTACK) || (nStatus & SKILL_STATUS_CANT_CASTSKILL)  || (nStatus & SKILL_STATUS_DREAD)) {
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(0.f, 200, 0.f);
		if (nStatus & SKILL_STATUS_CANT_CASTSKILL) {
			color = FLinearColor(200.f, 0.f, 0.f);
		}else if (nStatus & SKILL_STATUS_CANT_ATTACK) {
			color = FLinearColor(200.f, 200.f, 0.f);
		}
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
	}
	else {
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 0.f);
	}

	if (nStatus & SKILL_STATUS_CONFUSION) {
		AddBuff(1003);
	}
	else {
		RemoveBuff(1003);
	}

	if (nStatus & SKILL_STATUS_CANT_MOVE) {
		AddBuff(1008);
	}
	else {
		RemoveBuff(1008);
	}
	ResetAttr();
	if (!CanMove())
	{
		GetController()->StopMovement();
	}
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::onMsgMapObjectUnderAttackG2C(const MsgMapObjectUnderAttackG2C& msg)
{
	if (GetObjectId() == msg.sourceobjid()) {
		for (auto&& target : msg.targetinfo())
		{
			if (target.mapobjectid() != GetObjectId() && GetDamageCollision()) {
				GetDamageCollision()->AddTargetId(target.mapobjectid());
			}
		}

		std::string strSound = GetBattleSound(SOUND_TPY_ATK);
		if (!strSound.empty()) {
			FString strPath = strSound.c_str();
			UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
			if (soundObj)
			{
				UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
			}
		}
	}

	bool bBeAtk = false;
	for (auto&& target : msg.targetinfo())
	{
		if (target.mapobjectid() == GetObjectId()) {
			Damage(msg.sourceobjid(), target.damage(), target.criticl() == 1);
			if (this == AIPlayerMgr::getInstance().GetHero()) {
				GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_HP, 
					false, target.damage(), MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON_MSG_SYN_BASE_TPY_REASON_BE_ATTACK, false);
			}
			bBeAtk = true;
			std::string strSound = GetBattleSound(SOUND_TPY_HIT);
			if (!strSound.empty()) {
				FString strPath = strSound.c_str();
				UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
				if (soundObj)
				{
					UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
				}
			}
		}
	}
	if (!bBeAtk || IsDeath()) {
		return;
	}

	if (msg.sourceobjid() > 0 && m_aRangeCollion && m_aRangeCollion->AddTargetId(msg.sourceobjid(), true)) {
		if (m_pAIControll) {
			m_pAIControll->SetBlackBoolValue(FIND_TARGET, GetFocusRole() != nullptr);
		}
	}
	ResetAttr();
}

void ARole::onMsgSkillEffectDotDmgMS2C(const MsgSkillEffectDotDmgMS2C& msg)
{
	bool bBeAtk = false;
	if (msg.target_mapobjectid() == GetObjectId()) {
		Damage(msg.caster_mapobjectid(), msg.dmg_val(), msg.criticl() == 1, true);
		if (this == AIPlayerMgr::getInstance().GetHero()) {
			GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_HP,
				false, msg.dmg_val(), MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON_MSG_SYN_BASE_TPY_REASON_BE_ATTACK, false);
		}
		bBeAtk = true;
	}
	if (!bBeAtk || IsDeath()) {
		return;
	}
	ResetAttr();
}

void ARole::onMsgSkillEffectDmgMS2C(const MsgSkillEffectDmgMS2C& msg)
{
	if (msg.caster_mapobjectid() > 0 && msg.caster_mapobjectid() == GetObjectId() && GetDamageCollision()) {
		GetDamageCollision()->AddTargetId(msg.target_mapobjectid());
	}

	if (AIPlayerMgr::getInstance().GetHero()->GetObjectId() == GetObjectId()
		&& GetObjectId() == msg.caster_mapobjectid()) {
		auto&& hero = Cast<AHero>(this);
		if (hero) {
			hero->LockFocusObject();
		}
	}

	bool bBeAtk = msg.target_mapobjectid() == GetObjectId();
	if (!bBeAtk) {
		return;
	}

	std::string strSound = GetBattleSound(SOUND_TPY_HIT);
	if (!strSound.empty()) {
		FString strPath = strSound.c_str();
		UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
		if (soundObj)
		{
			UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
		}
	}

	Damage(msg.caster_mapobjectid(), msg.dmg_val(), msg.criticl() == 1);
	if (this == AIPlayerMgr::getInstance().GetHero()) {
		GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_HP,
			false, msg.dmg_val(), MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON_MSG_SYN_BASE_TPY_REASON_BE_ATTACK, false);
	}

	if (IsDeath()) {
		return;
	}

	if (msg.caster_mapobjectid() > 0 && m_aRangeCollion && m_aRangeCollion->AddTargetId(msg.caster_mapobjectid(), true)) {
		if (m_pAIControll) {
			m_pAIControll->SetBlackBoolValue(FIND_TARGET, GetFocusRole() != nullptr);
		}
	}

	ResetAttr();
}

void ARole::onMsgSkillEffectRestoreMS2C(const MsgSkillEffectRestoreMS2C& msg)
{
	if (IsDeath()) {
		return;
	}
}

void ARole::onMsgPlayerNormalAttackResS2C(const MsgPlayerNormalAttackResS2C& msg)
{
	int nObjectId = AIPlayerMgr::getInstance().GetHero()->GetObjectId();
	std::string strHitPath = AIPlayerMgr::getInstance().GetHero()->GetHitEffectPath();
	
	bool bBeAtk = false;
	for (auto&& target : msg.targetinfo())
	{
		if (target.mapobjectid() == GetObjectId()) {
			Damage(nObjectId, target.damage(), target.criticl() == 1);
			if (!strHitPath.empty()) {
				HitEffect(strHitPath.c_str());
			}
			bBeAtk = true;
		}
	}

	if (nObjectId == GetObjectId()) {
		auto&& hero = Cast<AHero>(this);
		if (hero)
		{
			hero->LockFocusObject();
		}

		std::string strSound = GetBattleSound(SOUND_TPY_ATK);
		if (!strSound.empty()) {
			FString strPath = strSound.c_str();
			UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
			if (soundObj)
			{
				UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
			}
		}
	}

	if (!bBeAtk || IsDeath()) {
		return;
	}

	std::string strSound = GetBattleSound(SOUND_TPY_HIT);
	if (!strSound.empty()) {
		FString strPath = strSound.c_str();
		UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
		if (soundObj)
		{
			UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
		}
	}

	if (nObjectId > 0 && m_aRangeCollion && m_aRangeCollion->AddTargetId(nObjectId, true)) {
		if (m_pAIControll) {
			m_pAIControll->SetBlackBoolValue(FIND_TARGET, GetFocusRole() != nullptr);
		}
	}
	ResetAttr();
}

void ARole::onMsgCharacterDeadS2C(const MsgCharacterDeadS2C& msg)
{
	if (msg.mapobjectid() == GetObjectId()) {
		Death();
	}
	else {
		bool bPreFocus = GetFocusRole() != nullptr;
		if (m_aRangeCollion && m_aRangeCollion->RemoveId(msg.mapobjectid()))
		{
			if (m_pAIControll) {
				m_pAIControll->SetBlackBoolValue(FIND_TARGET, GetFocusRole() != nullptr);
			}
			if (bPreFocus && GetFocusRole() == nullptr) {
				StateHome();
			}
			ResetAttr();
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
void ARole::SetAtkSpeed(int nSpeed)
{
	m_nAtkSpeedRate = nSpeed;
}

int ARole::GetAtkSpeed() const
{
	return m_nAtkSpeedRate;
}

void ARole::SetAIMoveAvodiance(bool bAvo, float fDis)
{
	// 1. 获取胶囊体组件（以Character为例）
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if (!CapsuleComp) {
		return;
	}


	// 2. 设置碰撞预设（可选，确保使用正确的预设）
	CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));

	// 3. 设置对特定通道的响应为 Block
	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CapsuleComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);   // 阻挡其他Pawn
	CapsuleComp->SetCollisionResponseToChannel(ECC_GameTraceChannel9, ECR_Block); // 阻挡静态物体
}



