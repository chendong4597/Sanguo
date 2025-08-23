// Fill out your copyright notice in the Description page of Project Settings.


#include "Generals.h"
#include "config/EquipTypeCfg.h"
#include "config/WeaponTypeCfg.h"
#include "config/WeaponInfoTypeCfg.h"
#include "config/RoleCreateCfg.h"
#include "config/WeaponAniEffectCfg.h"
#include "config/SkillTypeCfg.h"
#include "config/SkillCfg.h"
#include "config/BuffCfg.h"

#include "Engine/Blueprint.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

#include "data/ItemData.h"
#include "data/NpcProtectorData.h"
#include "config/NpcTypeCfg.h"
#include "config/NpcBadgeCfg.h"

#include "../Scene/LevelBase.h"
#include "../Scene/InstanceLevel.h"
#include "../Scene/MainCityLevel.h"
#include "ReichGameInstance.h"

#include "msg/MsgItem.pb.h"

#include "Weapon.h"
#include "WeaponEffect.h"
#include "BuffEffect.h"
#include "Skill/SkillEffect.h"
#include "Monster.h"
#include "IPlayer.h"
#include "Hero.h"
#include "HorseInstance.h"

extern UWorld* gp_UWorld;

void AGenerals::BeginPlay()
{
	Super::BeginPlay();
	if (UReichGameInstance::IsEditorMode()) return;

	CreateMyCollision(100);
	UI_REGISTER_MYEVENT(GeneralsSkillEvent, &AGenerals::onGeneralsSkillEvent);

	RG_REGISTER_MYEVENT(HandlerNpcEvent, &AGenerals::onHandlerNpcEvent);
	RG_REGISTER_MYEVENT(HandlerProtectorEvent, &AGenerals::onHandlerProtectorEvent);
	RG_REGISTER_MYEVENT(HandlerNpcAliveEvent, &AGenerals::onHandlerNpcAliveEvent);
}

void AGenerals::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UReichGameInstance::IsEditorMode()) return;
	UNRG_REGISTER_MYEVENT(HandlerNpcEvent, this);
}

void AGenerals::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDeath() || !m_npcData || !m_protectData) {
		return;
	}

	if (IsInMainCity()) {
		if (m_npcData->getHp() < m_npcData->getMaxHp())
		{
			m_fLifeInterTm += DeltaTime;
			if (m_fLifeInterTm >= 1.f) {     //恢复血量
				m_fLifeInterTm -= 1.f;
				m_npcData->setHp(m_npcData->getHp() + m_npcData->getMaxHp() / 5);
			}
		}


		if (GetFocusRole() == nullptr &&
			m_protectData->status == MsgPB::PROTECTOR_STATUS::BADGE_STATUS_PATROL)   //AI找怪有时找不到，暂时用其他办法解决下 每10秒找下附加怪物
		{
			m_fFindMonsterInterTm += DeltaTime;
			if (m_fFindMonsterInterTm >= 10.f) {
				auto&& monsters = AMonsterMgr::getInstance().GetMonsters();
				for (auto&& monster : monsters)
				{
					ARole* pRole = Cast<ARole>(monster.second);
					if (!pRole) {
						continue;
					}
					if (!IsCanAttackTarget(pRole)) {
						continue;
					}
					FVector pos1 = pRole->GetActorLocation();
					FVector pos2 = GetActorLocation();
					if (FVector::Distance(pos1, pos2) > 1500.f)
					{
						continue;
					}

					AddTarget(pRole);
				}
				m_fFindMonsterInterTm = 0.f;
			}
		}
	} else {
		m_cavInterPlay += DeltaTime;
		if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry && 
			m_npcData && m_npcData->findBuffById(BuffType::BUFF_CAV) && GetFocusRole() != nullptr &&
			m_cavInterPlay >= 1.5f)
		{
			// 获取动画实例
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			// 检查是否正在播放任何蒙太奇
			if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
			{
				PlayAttack();
				m_cavInterPlay = 0.f;
			}
		}
	}
}

void AGenerals::SetMapObjectId(NS::int32 MapObjectId)
{
	m_MapObjectId = MapObjectId;
	m_npcData = NpcMgr::getInstance().getNpc(m_MapObjectId);
	if (m_npcData) {
		m_npcData->calcAttr();
	}
}

void AGenerals::InitSkin()
{
	if (!m_protectData) {
		return;
	}

	auto&& genType = NpcTypeCfg::getInstance().GetGenelType(m_protectData->templateid);
	if (!genType)
	{
		return;
	}

	//皮肤
	USkeletalMeshComponent* mesh = GetMesh();
	if (!mesh) {
		return;
	}

	for (int i = 0; i < mesh->GetNumMaterials() && i < 10 && i < genType->vecTxtures.size(); i++)
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

		std::string strT = NpcTypeCfg::getInstance().GetBpMatTexture(m_protectData->templateid,
			genType->vecTxtures[i]);
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

bool AGenerals::IsWaitBegin()
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

int AGenerals::GetObjectId()
{
	return m_MapObjectId;
}

bool AGenerals::IsDeath()
{
	if (!m_npcData || !m_protectData) {
		return false;
	}
	if (!m_npcData->isInitialized()) {
		return true;
	}
	if (m_npcData->getHp() <= 0) {
		return true;
	}
	if (m_protectData->status == MsgPB::PROTECTOR_STATUS::BADGE_STATUS_DEATH && IsInMainCity()) {
		return true;
	}
	return false;
}

int AGenerals::GetHp()
{
	if (!m_npcData) {
		return 0;
	}
	return m_npcData->getHp();
}

int AGenerals::GetMaxHp()
{
	if (!m_npcData) {
		return 1;
	}
	return m_npcData->getMaxHp();
}

int AGenerals::GetRoleCtrlStatus()
{
	if (m_npcData)
	{
		return m_npcData->getCtrlState();
	}
	return 0;
}

//******************************************************************************************************************************
//
//******************************************************************************************************************************
void AGenerals::ResetAttr()
{
	ResetSpeed();
	ResetAtkSpeed();

	if (m_npcData && m_npcData->findBuffById(BuffType::BUFF_INF))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(200.f, 0.f, 0.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
		GetController()->StopMovement();
	}
	else if (m_npcData && m_npcData->findBuffById(BuffType::BUFF_CAV))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(200.f, 200, 0.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
	}
	else if (m_npcData && m_npcData->findBuffById(BuffType::BUFF_ARC))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(0.f, 200.f, 0.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
	}
	else if (m_npcData && m_npcData->findBuffById(BuffType::BUFF_BLA))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(200.f, 0, 0.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
		SetAttackDistance(1500);
	}
	else if (m_npcData && m_npcData->findBuffById(BuffType::BUFF_MAG))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(0.f, 200.f, 200.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
	}
	else {
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 0.f);
		if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Blade) {
			SetAttackDistance(250);
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::ResetSpeed()
{
	if (!m_protectData)
	{
		return;
	}
	float fSpeed = 300.f;
	auto&& pBadgeCfg = NpcBadgeCfg::getInstance().findNpcBadgeCfg(m_protectData->itemid, m_protectData->level);
	if (pBadgeCfg) {
		fSpeed = pBadgeCfg->n_Speed;
	}

	
	if (m_npcData) {
		float addPhase = m_npcData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CHASE);
		float addRate = 60.f;
		if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry)
		{
			addRate = 300.f;
		}
		fSpeed += (addPhase - 1.f) * addRate;
	}

	fSpeed = std::max(50.f, std::min(1000.f, fSpeed));

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent) {
		// 设置角色的移动速度
		MovementComponent->MaxWalkSpeed = fSpeed; // 设置为300米/秒
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::ResetAtkSpeed()
{
	SetAtkSpeed(100.f);
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::SetWeaponId(int nWeaponId)
{
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(nWeaponId);
	if (!pCfg)
	{
		return;
	}
	auto&& pTpyCfg = WeaponTypeCfg::getInstance().GetWeaponType(pCfg->n_Type);
	if (!pTpyCfg)
	{
		return;
	}
	m_nWeaponId = nWeaponId;
	m_nWeaponTpy = pCfg->n_Type;
	AttachWeaponById(nWeaponId);
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::AttachWeaponById(int nWeaponId)
{
	m_AWeapon = NULL;
	auto aniMota = this->GetCurrentMontage();
	if (aniMota)
	{
		this->StopAnimMontage(aniMota);
	}

	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MainMesh"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById meshs"));
	auto mesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (!mesh)
	{
		return;
	}

	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById mesh"));
	auto&& pWeapon = WeaponInfoTypeCfg::getInstance().GetWeaponById(nWeaponId);
	if (pWeapon == nullptr)
	{
		return;
	}
	auto&& pWeaponTpy = WeaponTypeCfg::getInstance().GetWeaponType(m_nWeaponTpy);
	if (!pWeaponTpy)
	{
		return;
	}

	std::string strPath = WeaponInfoTypeCfg::getInstance().GetBPWeaponPath(nWeaponId);
	FString strWeaponPath = FString(strPath.c_str());

	FString strSockName = FString("Sock_Spear");
	SetAttackDistance(80);
	if (m_nWeaponTpy == Weapon_Tpy_Knife)
	{
		SetAttackDistance(160);
		strSockName = FString("Sock_Knife");
	}
	else if (m_nWeaponTpy == Weapon_Tpy_Bow)
	{
		if (IsInMainCity()) {
			SetAttackDistance(800);
		}
		else {
			SetAttackDistance(2000);
		}
		
		strSockName = FString("Sock_Bow");
	} else if (m_nWeaponTpy == Weapon_Tpy_Blade)
	{
		SetAttackDistance(250);
		strSockName = FString("Sock_Blade");
	}
	else if (m_nWeaponTpy == Weapon_Tpy_Staff)
	{
		if (IsInMainCity()) {
			SetAttackDistance(800);
		}
		else {
			SetAttackDistance(2900);
		}
		strSockName = FString("Sock_Staff");
	}
	AWeapon* aWeapon = LoadWeapon(strWeaponPath);
	if (aWeapon)
	{
		UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById L aWeapon is not null"));
		aWeapon->SetOwnerRole(this);
		if (pWeaponTpy->n_HitHurt == 1)
		{
			aWeapon->RegisterOverlayEvents();
		}
		aWeapon->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*strSockName));
		aWeapon->SetWeaponId(nWeaponId);
		m_AWeapon = aWeapon;
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::AttackEffect()
{
	if (!GetFocusRole() || GetObjectId() <= 0 || !m_npcData)
	{
		return;
	}
	if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry || 
		m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry ||
		(m_nGenelTpy == NpcTypeCfg::NpcSubType_Blade && !m_npcData->findBuffById(BuffType::BUFF_BLA))) {
		if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry && m_npcData->findBuffById(BuffType::BUFF_CAV))
		{
			GetDamageCollision()->SectorDamage(300.f, 90.f, 600.f, 0);
		}
		else if (GetDamageCollision() && GetFocusRole())
		{
			GetDamageCollision()->SingleDamage(GetFocusRole()->GetObjectId(), 0, 300);
		}		
	} else {
		if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Archer) {
			int nSkillId = 21001;
			if (m_npcData->findBuffById(BuffType::BUFF_ARC))
			{
				nSkillId = 21002;
			}
			SkillMgr::getInstance().CreateSkill(nSkillId, this, GetFocusRole(), GetFocusRole()->GetActorLocation(), FVector::ZeroVector);
		}
		else if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Blade) {
			SkillMgr::getInstance().CreateSkill(21003, this, GetFocusRole(), GetFocusRole()->GetActorLocation(), FVector::ZeroVector);
		}
		else if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Magic) {
			int nSkillId = 21004;
			if (m_npcData->findBuffById(BuffType::BUFF_MAG))
			{
				nSkillId = 21005;
			}
			SkillMgr::getInstance().CreateSkill(nSkillId, this, GetFocusRole(), GetFocusRole()->GetActorLocation(), FVector::ZeroVector);
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
float AGenerals::GetNormalAttackInterTm()
{
	return 1.0f;
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::SetSubWeaponId(int nWeaponId)
{
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(nWeaponId);
	if (!pCfg)
	{
		return;
	}
	auto&& pTpyCfg = WeaponTypeCfg::getInstance().GetWeaponType(pCfg->n_Type);
	if (!pTpyCfg)
	{
		return;
	}
	m_nSubWeaponId = nWeaponId;
	m_nSubWeaponTpy = pCfg->n_Type;
	AttachSubWeaponById(nWeaponId);
}

//********************************************************************************************************************************************************************************
//设置状态材质参数
//********************************************************************************************************************************************************************************
void AGenerals::SetMaterialScalerParamer(FName name, float fValue)
{
	if (m_AWeapon)
	{
		m_AWeapon->SetMeshMaterialScalerParamer(name, fValue);
	}
	if (m_BWeapon)
	{
		m_BWeapon->SetMeshMaterialScalerParamer(name, fValue);
	}


	ARole::SetMaterialScalerParamer(name, fValue);
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::AttachSubWeaponById(int nWeaponId)
{
	m_BWeapon = NULL;
	auto aniMota = this->GetCurrentMontage();
	if (aniMota)
	{
		this->StopAnimMontage(aniMota);
	}

	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MainMesh"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById meshs"));
	auto mesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (!mesh)
	{
		return;
	}

	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById mesh"));
	auto&& pWeapon = WeaponInfoTypeCfg::getInstance().GetWeaponById(nWeaponId);
	if (pWeapon == nullptr)
	{
		return;
	}
	auto&& pWeaponTpy = WeaponTypeCfg::getInstance().GetWeaponType(m_nSubWeaponTpy);
	if (!pWeaponTpy)
	{
		return;
	}

	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById strWeaponPath = %s"), *FString(pWeapon->s_Path.c_str()));
	std::string strPath = WeaponInfoTypeCfg::getInstance().GetBPWeaponPath(nWeaponId);
	FString strWeaponPath = FString(strPath.c_str());

	FString strSockName = FString("Sock_Shield");
	if (m_nSubWeaponTpy == Weapon_Tpy_Quiver) {
		strSockName = FString("Sock_Quiver");
	}else if (m_nSubWeaponTpy == Weapon_Tpy_Book) {
		strSockName = FString("Sock_Book");
	}
	if (m_nSubWeaponTpy == Weapon_Tpy_Horse && !IsDeath()) {
		RiderHorse();
	}
	else {
		AWeapon* aWeapon = LoadWeapon(strWeaponPath);
		if (aWeapon)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById L aWeapon is not null"));
			aWeapon->SetOwnerRole(this);
			if (pWeaponTpy->n_HitHurt == 1)
			{
				aWeapon->RegisterOverlayEvents();
			}
			aWeapon->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*strSockName));
			aWeapon->SetWeaponId(nWeaponId);
			m_BWeapon = aWeapon;
		}
	}
	//BTSetHorse(m_nSubWeaponTpy == Weapon_Tpy_Horse);
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::RemoveWeapons()
{
	while (true)
	{
		TArray<AActor*> arrayActors;
		this->GetAttachedActors(arrayActors);
		if (arrayActors.Num() <= 0)
		{
			break;
		}
		bool bBreak = true;
		for (int i = 0; i < arrayActors.Num(); i++)
		{
			AWeapon* weapon = Cast<AWeapon>(arrayActors[i]);
			if (weapon)
			{
				weapon->Destroy();
				bBreak = false;
				break;
			}
		}
		if (bBreak)
		{
			break;
		}
	}
	m_AWeapon = nullptr;
}

//****************************************************************************************
//horse
//****************************************************************************************
void AGenerals::RiderHorse()
{
	if (!m_protectData) {
		return;
	}
	int nWeaponId = 0;
	std::shared_ptr<ItemData> pItemDataSub = ItemPacketMgr::getInstance().findItemDataByUuid(m_protectData->weaponId2);
	if (pItemDataSub) {
		nWeaponId = pItemDataSub->getItemId();
	}

	if (nWeaponId == 0) {
		return;
	}

	auto&& pWeapon = WeaponInfoTypeCfg::getInstance().GetWeaponById(nWeaponId);
	if (pWeapon == nullptr)
	{
		return;
	}
	auto&& pWeaponTpy = WeaponTypeCfg::getInstance().GetWeaponType(m_nSubWeaponTpy);
	if (!pWeaponTpy)
	{
		return;
	}

	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MainMesh"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	auto mesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (!mesh)
	{
		return;
	}

	mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	UCapsuleComponent* capsuleComp = Cast<UCapsuleComponent>(this->GetComponentByClass(UCapsuleComponent::StaticClass()));
	if (capsuleComp) {
		mesh->AttachToComponent(capsuleComp, FAttachmentTransformRules::KeepRelativeTransform, FName("None"));
	}

	meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MOUNT"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	USkeletalMeshComponent* mountMesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (!mountMesh)
	{
		return;
	}
	std::string strPathMesh = WeaponInfoTypeCfg::getInstance().GetSkeshWeaponPath(nWeaponId);
	FString setPathM = FString(strPathMesh.c_str());
	USkeletalMesh* tmpMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *(setPathM)));
	if (!tmpMesh)
	{
		UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::SetMeshParts false"));
		return;
	}
	if (pWeapon->m_vecTextures.size() > 0)
	{
		for (int i = 0; i < pWeapon->m_vecTextures.size(); i++)
		{
			auto skinMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
				TEXT("Material'/Game/Weapon/Weapon_M/Com_Weapon_M.Com_Weapon_M'")));
			if (!skinMaterial) {
				UE_LOG(LogOutputDevice, Log, TEXT("AWeapon::SetWeaponId false"));
				return;
			}

			std::string strDifuse = WeaponInfoTypeCfg::getInstance().GetTexture(nWeaponId, i);
			if (strDifuse.empty())
			{
				return;
			}
			FString strText = UReichGameInstance::TransTexturePath(strDifuse);

			auto dynaMaterial = UMaterialInstanceDynamic::Create(skinMaterial, this);
			if (!dynaMaterial)
			{
				UE_LOG(LogOutputDevice, Log, TEXT("AWeapon::SetWeaponId CreateDynamicMaterialInstance false"));
				return;
			}

			//漫反射贴图
			auto tmpTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(strText)));
			if (!tmpTexture)
			{
				UE_LOG(LogOutputDevice, Log, TEXT("AWeapon::SetWeaponId CreateDynamicMaterialInstance tmpTexture false"));
				return;
			}
			dynaMaterial->SetTextureParameterValue(FName("Defuse"), tmpTexture);
			mountMesh->SetMaterial(i, dynaMaterial);
		}
	}
	mountMesh->SetSkeletalMesh(tmpMesh);
	mountMesh->SetRelativeScale3D(FVector(pWeapon->n_RangeParam1*0.8f / 100.f, pWeapon->n_RangeParam1*0.8f / 100.f, pWeapon->n_RangeParam1*0.8f / 100.f));
	mountMesh->SetHiddenInGame(false);
	std::string strPath = WeaponInfoTypeCfg::getInstance().GetAnimHorse(nWeaponId);
	FString setAnimPath = FString(strPath.c_str());
	UClass* horseClass = LoadObject<UClass>(NULL, *setAnimPath);
	if (!horseClass) {
		return;
	}
	mountMesh->SetAnimClass(horseClass);
	mesh->AttachToComponent(mountMesh, FAttachmentTransformRules::KeepRelativeTransform, FName("SockRider_G"));
}

void AGenerals::UnRiderHorse()
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

	TArray<UActorComponent*> meshMains = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MainMesh"));
	if (meshMains.Num() <= 0)
	{
		return;
	}

	USkeletalMeshComponent* meshMain = Cast<USkeletalMeshComponent>(meshMains[0]);
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

//****************************************************************************************
//
//****************************************************************************************
AWeapon* AGenerals::LoadWeapon(FString path)
{
	// load the class
	UClass* weaponClass = LoadObject<UClass>(NULL, *path);
	if (!weaponClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::LoadWeapon failed"));
		return NULL;
	}
	AWeapon* spawnActor = GetWorld()->SpawnActor<AWeapon>(weaponClass);
	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::LoadWeapon Success"));
	if (spawnActor)
	{
		return spawnActor;
	}
	return NULL;
}

//****************************************************************************************
//
//****************************************************************************************
bool AGenerals::IsCanAttackTarget(ARole* pRole, bool bPassive)
{
	if (IsDeath() || !pRole || pRole->IsDeath())
	{
		return false;
	}
	if (pRole->GetCamp() == CampState::CAMP_OthPlayer) {
		return true;
	}
	if (pRole->GetCamp() == CampState::CAMP_MONSTER) {
		AMonster* pMonster = Cast<AMonster>(pRole);
		if (!pMonster) {
			return false;
		}

		auto&& pMonsterTpy = MonsterCfg::getInstance().findMonsterType(pMonster->GetMonsterId());
		if (!pMonsterTpy || pMonsterTpy->n_nTowerTpy == MonsterCfg::MonsterType::MobTowerTypeDefTower) {
			return false;
		}

		//弓兵不攻击防御塔
		//if (pMonsterTpy->n_nTowerTpy == MonsterCfg::MonsterType::MobTowerTypeCrystal && 
		//	(m_nGenelTpy == NpcTypeCfg::NpcSubType_Archer || m_nGenelTpy == NpcTypeCfg::NpcSubType_Magic))
		//{
		//	return false;
		//}

		if (pMonster->IsForbidAttack()) {
			return false;
		}

		if (!m_protectData)
		{
			return false;
		}

		if (!IsInMainCity()) {
			return true;
		}

		UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
		if (!GameInstance)
		{
			return false;
		}

		AMainCityLevel* pLevel = Cast<AMainCityLevel>(GameInstance->GetCurrentLevel());
		if (!pLevel)
		{
			return false;
		}

		return pLevel->IsSameBattleLevel(m_protectData->level, pMonsterTpy->n_MobLv);
	}
	else if (pRole->GetCamp() == CampState::CAMP_GEN_GUARD
		|| pRole->GetCamp() == CampState::CAMP_SOL_GUARD) {
		return true;
	}
	return false;
}

//****************************************************************************************
//
//****************************************************************************************
bool AGenerals::PlayAttack()
{
	if (IsDeath() || IsDefenceing()) {
		return false;
	}

	if ((m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry || m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry
		|| m_nGenelTpy == NpcTypeCfg::NpcSubType_Blade)
		&& !CanAttack()) {
		return false;
	}

	if ((m_nGenelTpy == NpcTypeCfg::NpcSubType_Archer || m_nGenelTpy == NpcTypeCfg::NpcSubType_Magic) && !CanSkillAttack()) {
		return false;
	}

	if (!m_protectData)
	{
		return false;
	}
	bool bSecAni = false;
	if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry && m_npcData && m_npcData->findBuffById(BuffType::BUFF_CAV))
	{
		bSecAni = true;
	}

	std::string strAniPath = NpcTypeCfg::getInstance().GetAttackPath(m_protectData->itemid, m_protectData->templateid, bSecAni);
	if (strAniPath.empty())
	{
		return false;
	}

	LookAtTarget();

	FString strAnim(strAniPath.c_str());
	
	UObject* loadObj = StaticLoadObject(UAnimMontage::StaticClass(), NULL, strAnim.GetCharArray().GetData());
	if (loadObj == nullptr)
	{
		return false;
	}
	UAnimMontage* ubp = Cast<UAnimMontage>(loadObj);
	if (!ubp)
	{
		return false;
	}
	auto aniMota = this->GetCurrentMontage();
	if (aniMota)
	{
		this->StopAnimMontage(aniMota);
	}
	ubp->BlendIn.SetBlendTime(0.f);
	PlayAnimMontage(ubp, GetSpeedRate());
	return true;
}

void AGenerals::Death()
{
	if (m_nSubWeaponTpy == Weapon_Tpy_Horse) {  //死亡下马
		UnRiderHorse();
	}

	AGeneralsMgr::getInstance().DestroyGenelIns(GetObjectId());
	
	return Super::Death();
}

void AGenerals::Revive()
{
	ARoleAIController* pAIController = Cast<ARoleAIController>(GetController());
	if (pAIController)
	{
		pAIController->SetBlackBoolValue(ISDEAD, false);
	}
	if (m_nSubWeaponTpy == Weapon_Tpy_Horse) {  //上马
		RiderHorse();
	}
	if (IsInMainCity()) {
		SetActorTransform(m_oriTrans);
	}
	
	Super::Revive();
}

bool AGenerals::CanMove()
{
	//if (IsDefenceing())
	//{
	//	return false;
	//}
	return Super::CanMove();
}

bool AGenerals::IsDefenceing()
{
	if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry)
	{
		if (m_npcData && m_npcData->findBuffById(BuffType::BUFF_INF))
		{
			return true;
		}
	}
	return false;
}

void AGenerals::ResortAttackTargets()
{
	if (m_nGenelTpy != NpcTypeCfg::NpcSubType_Cavalry || !m_npcData || !m_npcData->findBuffById(BuffType::BUFF_CAV))
	{
		return;
	}
	if (GetFocusRole() != nullptr && IsCanAttackTarget(GetFocusRole()) && 
		FVector::Distance(GetFocusRole()->GetActorLocation(), GetActorLocation()) > 200.f)
	{
		return;
	}
	if (GetDamageCollision())
	{
		GetDamageCollision()->ResortCavNextTarget();
	}
}

void AGenerals::SetProtectorUuid(NS::I64 ProtectorUuid)
{
	m_ProtectorUuid = ProtectorUuid;
	auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(m_ProtectorUuid);
	if (!data) {
		return;
	}
	m_protectData = data;

	m_bBattleing = m_protectData->status == MsgPB::PROTECTOR_STATUS::BADGE_STATUS_PATROL;

	auto&& npcCfg = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
	if (!npcCfg) {
		return;
	}
	m_nGenelTpy = npcCfg->n_SubType;
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::onGeneralsSkillEvent(const GeneralsSkillEvent& evt)
{
	if (IsDeath())
	{
		return;
	}
	if ((evt.m_type == GeneralsSkillEvent::EventType::Gen_INF ||
		evt.m_type == GeneralsSkillEvent::EventType::Gen_StartBuff) && m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry)
	{
		GameAppExtern::c2cSkillAttack(SkillType::SKILL_INF, GetObjectId(), GetObjectId());
	}
	else if (evt.m_type == GeneralsSkillEvent::EventType::Gen_INF_C && m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry)
	{
		GameAppExtern::c2cNormalDelBuff(BuffType::BUFF_INF, GetObjectId());
	}
	else if ((evt.m_type == GeneralsSkillEvent::EventType::Gen_CAV ||
		evt.m_type == GeneralsSkillEvent::EventType::Gen_StartBuff) && m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry)
	{
		GameAppExtern::c2cSkillAttack(SkillType::SKILL_CAV, GetObjectId(), GetObjectId());
	}
	else if ((evt.m_type == GeneralsSkillEvent::EventType::Gen_BLA ||
		evt.m_type == GeneralsSkillEvent::EventType::Gen_StartBuff) && m_nGenelTpy == NpcTypeCfg::NpcSubType_Blade)
	{
		GameAppExtern::c2cSkillAttack(SkillType::SKILL_BLA, GetObjectId(), GetObjectId());
	}
	else if ((evt.m_type == GeneralsSkillEvent::EventType::Gen_MAG ||
		evt.m_type == GeneralsSkillEvent::EventType::Gen_StartBuff) && m_nGenelTpy == NpcTypeCfg::NpcSubType_Magic)
	{
		GameAppExtern::c2cSkillAttack(SkillType::SKILL_MAG, GetObjectId(), GetObjectId());
	}
	if (m_npcData)
	{
		m_npcData->calcAttr();
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AGenerals::onHandlerNpcEvent(const HandlerNpcEvent& evt)
{
	if (evt.m_type == HandlerNpcEvent::EventNpc::EventNpc_Equip
		&& m_ProtectorUuid == evt.uuid) {
		m_protectData = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(m_ProtectorUuid);
		if (!m_protectData)
		{
			return;
		}
		RemoveWeapons();
		std::shared_ptr<ItemData> pItemData = ItemPacketMgr::getInstance().findItemDataByUuid(m_protectData->weaponId1);
		if (pItemData)
		{
			SetWeaponId(pItemData->getItemId());
		}
		std::shared_ptr<ItemData> pItemDataSub = ItemPacketMgr::getInstance().findItemDataByUuid(m_protectData->weaponId2);
		if (pItemDataSub)
		{
			SetSubWeaponId(pItemDataSub->getItemId());
		}
		if (m_npcData) {
			m_npcData->calcAttr();
		}
	}
}

void AGenerals::onHandlerProtectorEvent(const HandlerProtectorEvent& evt)
{
	if (evt.m_type == HandlerProtectorEvent::EventProtector::EventProtector_Update &&
		evt.uuid == m_ProtectorUuid)
	{
		m_protectData = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(m_ProtectorUuid);
		if (m_protectData) {
			m_bBattleing = m_protectData->status == MsgPB::PROTECTOR_STATUS::BADGE_STATUS_PATROL;
		}
	}
}

void AGenerals::onHandlerNpcAliveEvent(const HandlerNpcAliveEvent& evt)
{
	if (evt.m_uuid == m_ProtectorUuid) {
		m_protectData = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(m_ProtectorUuid);
		Revive();
	}
}

bool AGenerals::IsTooFarToPlayer()
{
	if (IsInMainCity()) {
		return false;
	}

	return false;
}

bool AGenerals::IsRest()
{
	if (!IsInMainCity()) {
		return false;
	}
	return !m_bBattleing;
}

FVector AGenerals::GetMainCityBattlePos()
{
	if (!m_protectData)
	{
		return FVector::ZeroVector;
	}

	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return FVector::ZeroVector;
	}

	AMainCityLevel* pLevel = Cast<AMainCityLevel>(GameInstance->GetCurrentLevel()) ;
	if (!pLevel)
	{
		return FVector::ZeroVector;
	}

	FVector vec = pLevel->GetBattlePos(m_protectData->level);
	if (vec == FVector::ZeroVector)
	{
		return vec;
	}
	vec.X = vec.X - 1000 + std::rand() % 1000;
	vec.Y = vec.Y - 1000 + std::rand() % 1000;
	return vec;
}


//****************************************************************************************
//
//****************************************************************************************
std::string AGenerals::GetBattleSound(int nSoundTpy)
{
	if (!AIPlayerMgr::getInstance().GetHero()) {
		return "";
	}
	FVector pos = AIPlayerMgr::getInstance().GetHero()->GetActorLocation();
	if (FVector::Distance(GetActorLocation(), pos) >= 2000) {
		return "";
	}

	if (m_npcData) {
		return m_npcData->GetBattleSound(nSoundTpy);
	}
	return "";
}

//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
bool AGeneralsMgr::initialize()
{
	return true;
}

void AGeneralsMgr::terminate()
{
}

void AGeneralsMgr::PreReset()
{
	auto iter = m_mapGenerals.begin();
	for (; iter != m_mapGenerals.end(); iter++)
	{
		iter->second->Reset();
	}
}

bool AGeneralsMgr::reset()
{
	m_mapGenerals.clear();
	return true;
}

void AGeneralsMgr::update(float dt)
{
}

AGeneralsMgr::AGeneralsMgr()
{
	initialize();
}

AGeneralsMgr& AGeneralsMgr::getInstance()
{
	static AGeneralsMgr generalsMgr;
	return generalsMgr;
}

bool AGeneralsMgr::GetNpcByType(int nGenTpy, FVector& vec, NS::I64& proUuid)
{
	for (auto&& gen : m_mapGenerals)
	{
		if (!gen.second)
		{
			continue;
		}

		auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(gen.second->GetProtectorUuid());
		if (!data)
		{
			continue;
		}
		vec = gen.second->GetActorLocation();
		proUuid = data->uuid;
		return true;
	}
	return false;
}

void AGeneralsMgr::DestroyGenels(int nMapObjId)
{
	auto iter = m_mapGenerals.find(nMapObjId);
	if (iter == m_mapGenerals.end()) {
		return;
	}
	AGenerals* pGeneral = iter->second;
	if (pGeneral)
	{
		NpcDispearEvent evt;
		evt.m_uuid = pGeneral->GetProtectorUuid();
		UI_DISPATCH_MYEVENT(NpcDispearEvent, evt);
		pGeneral->DestroyAttach();
		pGeneral->Destroy();
	}
	m_mapGenerals.erase(iter);
}

void AGeneralsMgr::DestroyGenelIns(int nMapObjId)
{
	auto iter = m_mapGenerals.find(nMapObjId);
	if (iter == m_mapGenerals.end()) {
		return;
	}
	AGenerals* pGeneral = iter->second;
	if (pGeneral) {
		pGeneral->DestroyAttach();
		pGeneral->DestroyRole();
	}
	m_mapGenerals.erase(iter);
}

AGenerals* AGeneralsMgr::GetGeneral(int nObjId)
{
	if (m_mapGenerals.find(nObjId) == m_mapGenerals.end()) {
		return nullptr;
	}
	return m_mapGenerals[nObjId];
}

AGenerals* AGeneralsMgr::GetGeneralByUuid(int64 uuid)
{
	for (auto&& gen : m_mapGenerals)
	{
		if (gen.second->m_protectData && gen.second->m_protectData->uuid == uuid)
		{
			return gen.second;
		}
	}
	return nullptr;
}

void AGeneralsMgr::CreateGenels(int nMapObjectId, int64 nProtectId, int mMonsterId)
{
	if (!gp_UWorld)
	{
		return;
	}

	if (m_mapGenerals.find(nMapObjectId) != m_mapGenerals.end()) {
		return;
	}

	auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(nProtectId);
	if (!data)
	{
		return;
	}

	std::string strBP = NpcTypeCfg::getInstance().GetBpGenelPath(data->templateid);
	if (strBP.empty())
	{
		return;
	}

	FString strPath(strBP.c_str());

	UE_LOG(LogOutputDevice, Log, TEXT("ASoldierMgr::CreateGenels strPath = %s"), *strPath);

	UClass* generalClass = LoadObject<UClass>(NULL, *strPath);
	if (!generalClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("ASoldierMgr::CreateGenels failed"));
		return;
	}

	FTransform trans = AIPlayerMgr::getInstance().GetHero()->GetTransform();

	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return;
	}

	ALevelBase* pLevel = GameInstance->GetCurrentLevel();
	AInstanceLevel* pInsLevel = Cast<AInstanceLevel>(pLevel);
	if (pInsLevel) {
		trans = pInsLevel->GetTransByTroop(nProtectId);
	}

	UE_LOG(LogOutputDevice, Log, TEXT("ASoldierMgr::CreateGenels begin"));
	trans.SetScale3D(FVector::OneVector);
	FActorSpawnParameters params = FActorSpawnParameters();
	params.bNoFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGenerals* pGeneral = gp_UWorld->SpawnActor<AGenerals>(generalClass, trans, params);
	if (!pGeneral) {
		return;
	}
	pGeneral->m_oriTrans = trans;
	pGeneral->SetProtectorUuid(nProtectId);
	float fSpeed = 400.f;
	auto&& pBadgeCfg = NpcBadgeCfg::getInstance().findNpcBadgeCfg(data->itemid, data->level);
	if (pBadgeCfg) {
		fSpeed = pBadgeCfg->n_Speed;
	}

	pGeneral->SetCamp(CampState::CAMP_HERO);
	pGeneral->StartAI();
	pGeneral->SetMapObjectId(nMapObjectId);
	std::shared_ptr<ItemData> pItemData = ItemPacketMgr::getInstance().findItemDataByUuid(data->weaponId1);
	if (pItemData)
	{
		pGeneral->SetWeaponId(pItemData->getItemId());
	}
	std::shared_ptr<ItemData> pItemDataSub = ItemPacketMgr::getInstance().findItemDataByUuid(data->weaponId2);
	if (pItemDataSub)
	{
		pGeneral->SetSubWeaponId(pItemDataSub->getItemId());
	}
	pGeneral->InitSkin();
	pGeneral->ResetAttr();
	m_mapGenerals[nMapObjectId] = pGeneral;
}
