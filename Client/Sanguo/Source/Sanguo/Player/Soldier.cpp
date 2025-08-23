// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"
#include "config/EquipTypeCfg.h"
#include "config/WeaponTypeCfg.h"
#include "config/WeaponInfoTypeCfg.h"
#include "config/RoleCreateCfg.h"
#include "config/WeaponAniEffectCfg.h"
#include "config/SkillTypeCfg.h"
#include "config/BuffCfg.h"

#include "Engine/Blueprint.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

#include "data/ItemData.h"
#include "data/NpcProtectorData.h"
#include "config/NpcTypeCfg.h"
#include "config/NpcBadgeCfg.h"

#include "msg/MsgItem.pb.h"

#include "Weapon.h"
#include "WeaponEffect.h"
#include "BuffEffect.h"
#include "Skill/SkillEffect.h"
#include "IPlayer.h"
#include "Generals.h"
#include "Monster.h"
#include "Hero.h"

#include "../ReichGameInstance.h"
#include "HorseInstance.h"
#include "../Scene/InstanceLevel.h"

extern UWorld* gp_UWorld;

void ASoldier::BeginPlay()
{
	SetCamp(CampState::CAMP_HERO);
	Super::BeginPlay();

	CreateMyCollision(100);

	//InitLOD();

	UI_REGISTER_MYEVENT(GeneralsSkillEvent, &ASoldier::onGeneralsSkillEvent);
}

void ASoldier::SetHideWeapon(bool bHideWeapon)
{
	m_bHideWeapon = bHideWeapon;
}

void ASoldier::InitSkin()
{
	USkeletalMeshComponent* mesh = GetMesh();
	if (!mesh) {
		return;
	}

	for (int i = 0; i < mesh->GetNumMaterials() && i < 10 ; i++)
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

		std::string strT = "Texture2D'/Game/Models/Soldier/entity_s/soldier/defuse.defuse'";
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

void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsInMainCity()) {
		m_cavInterPlay += DeltaTime;
		if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry &&
			m_pMonsterData && m_pMonsterData->findBuffById(BuffType::BUFF_CAV) && GetFocusRole() != nullptr &&
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

//****************************************************************************************
//
//****************************************************************************************
void ASoldier::SetWeaponId(int nWeaponId)
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
void ASoldier::AttachWeaponById(int nWeaponId)
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

	SetAttackDistance(240);
	FString strSockName = FString("Sock_Knife");
	if (m_nWeaponTpy == Weapon_Tpy_Spear)
	{
		SetAttackDistance(120);
		strSockName = FString("Sock_Spear");
	}
	else if (m_nWeaponTpy == Weapon_Tpy_Bow) {
		SetAttackDistance(2500);
		strSockName = FString("Sock_Bow");
	}
	else if (m_nWeaponTpy == Weapon_Tpy_Blade) {
		SetAttackDistance(250);
		strSockName = FString("Sock_Blade");
	}
	else if (m_nWeaponTpy == Weapon_Tpy_Staff) {
		SetAttackDistance(3000);
		strSockName = FString("Sock_Staff");
	}

	if (m_bHideWeapon) {
		return;
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
void ASoldier::SetSubWeaponId(int nWeaponId)
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

//****************************************************************************************
//
//****************************************************************************************
void ASoldier::AttachSubWeaponById(int nWeaponId)
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

	std::string strPath = WeaponInfoTypeCfg::getInstance().GetBPWeaponPath(nWeaponId);
	FString strWeaponPath = FString(strPath.c_str());

	FString strSockName = FString("Sock_Shield");
	if (m_nSubWeaponTpy == Weapon_Tpy_Quiver) {
		strSockName = FString("Sock_Quiver");
	}
	else if (m_nSubWeaponTpy == Weapon_Tpy_Book) {
		return;
	}

	if (m_nSubWeaponTpy == Weapon_Tpy_Horse)
	{
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
		strPath = WeaponInfoTypeCfg::getInstance().GetAnimHorse(nWeaponId);
		FString setAnimPath = FString(strPath.c_str());
		UClass* horseClass = LoadObject<UClass>(NULL, *setAnimPath);
		if (!horseClass) {
			return;
		}
		mountMesh->SetAnimClass(horseClass);
		mesh->AttachToComponent(mountMesh, FAttachmentTransformRules::KeepRelativeTransform, FName("SockRider_S"));
	}
	else if(!m_bHideWeapon) {
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
	
	BTSetHorse(m_nSubWeaponTpy == Weapon_Tpy_Horse);
}

//****************************************************************************************
//
//****************************************************************************************
void ASoldier::RemoveWeapons()
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
//
//****************************************************************************************
AWeapon* ASoldier::LoadWeapon(FString path)
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

void ASoldier::SetMonsterId(int mapObjId, int64 uuid, int nMonsterId)
{
	m_MapObjectId = mapObjId;
	SetProtectorUuid(uuid);
	m_nMonsterId = nMonsterId;

	m_pMonsterData = MonsterDataMgr::getInstance().getMonster(mapObjId);

	if (!m_pMonsterData) {
		return;
	}
	m_pMonsterData->calcAttr();

	auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(uuid);
	if (!data) {
		return;
	}
	auto&& npcCfg = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
	if (!npcCfg) {
		return;
	}
	m_nGenelTpy = npcCfg->n_SubType;
}


int ASoldier::GetObjectId()
{
	if (m_pMonsterData) {
		return m_pMonsterData->getId();
	}
	return 0;
}


bool ASoldier::IsDeath()
{
	if (!m_pMonsterData) {
		return false;
	}
	if (!m_pMonsterData->isInitialized()) {
		return true;
	}
	return m_pMonsterData->getHp() <= 0;
}

int ASoldier::GetHp()
{
	if (!m_pMonsterData) {
		return 0;
	}
	return m_pMonsterData->getHp();
}

int ASoldier::GetMaxHp()
{
	if (!m_pMonsterData) {
		return 1;
	}
	return m_pMonsterData->getMaxHp();
}

int ASoldier::GetRoleCtrlStatus()
{
	if (m_pMonsterData) {
		return m_pMonsterData->getCtrlState();
	}
	return 0;
}

bool ASoldier::IsShowCtrlStatus()
{
	return true;
}

void ASoldier::ClearData()
{
	if (m_pMonsterData) {
		m_pMonsterData->clearInit();
	}
}

bool ASoldier::IsDefenceing()
{
	if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry)
	{
		if (m_pMonsterData && m_pMonsterData->findBuffById(BuffType::BUFF_INF))
		{
			return true;
		}
	}
	return false;
}



//****************************************************************************************
//
//****************************************************************************************
bool ASoldier::PlayAttack()
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

	if (!m_pMonsterData)
	{
		return false;
	}
	bool bSecAni = false;
	if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry && m_pMonsterData && m_pMonsterData->findBuffById(BuffType::BUFF_CAV))
	{
		bSecAni = true;
	}

	std::string strAniPath;
	if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry)
	{
		strAniPath = "AnimMontage'/Game/Models/Soldier/entity_s/soldier/infantry_attack_01_Montage.infantry_attack_01_Montage'";
	}
	else if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Archer) {
		strAniPath = "AnimMontage'/Game/Models/Soldier/entity_s/soldier/archer_attack_01_Montage.archer_attack_01_Montage'";
	}
	else if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry) {
		if (!bSecAni)
		{
			strAniPath = "AnimMontage'/Game/Models/Soldier/entity_s/soldier/cavalry_attack_01_Montage.cavalry_attack_01_Montage'";
		}
		else {
			strAniPath = "AnimMontage'/Game/Models/Soldier/entity_s/soldier/cavalry_attack_02_Montage.cavalry_attack_02_Montage'";
		}
	} 
	else if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Blade) {
		strAniPath = "AnimMontage'/Game/Models/Soldier/entity_s/soldier/blade_attack_01_Montage.blade_attack_01_Montage'";
	} 
	else if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Magic) {
		strAniPath = "AnimMontage'/Game/Models/Soldier/entity_s/soldier/magic_attack_01_Montage.magic_attack_01_Montage'";
	}

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

//******************************************************************************************************************************
//
//******************************************************************************************************************************
void ASoldier::ResetAttr()
{
	if (m_pMonsterData && m_pMonsterData->findBuffById(BuffType::BUFF_INF))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(200.f, 0.f, 0.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
		GetController()->StopMovement();
	}
	else if (m_pMonsterData && m_pMonsterData->findBuffById(BuffType::BUFF_CAV))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(200.f, 200, 0.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
	}
	else if (m_pMonsterData && m_pMonsterData->findBuffById(BuffType::BUFF_ARC))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(0.f, 200.f, 0.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
	}
	else if (m_pMonsterData && m_pMonsterData->findBuffById(BuffType::BUFF_BLA))
	{
		SetMaterialScalerParamer(FName(TEXT("EdgeValue")), 1.f);
		FLinearColor color = FLinearColor(200.f, 0, 0.f);
		SetMaterialVectorParamer(FName(TEXT("edgeColor")), color);
		SetAttackDistance(1500);
	}
	else if (m_pMonsterData && m_pMonsterData->findBuffById(BuffType::BUFF_MAG))
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
void ASoldier::AttackEffect()
{
	if (!m_pMonsterData) {
		return;
	}
	if (!GetFocusRole() || GetObjectId() <= 0)
	{
		return;
	}
	if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry ||
		m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry ||
		(m_nGenelTpy == NpcTypeCfg::NpcSubType_Blade && !m_pMonsterData->findBuffById(BuffType::BUFF_BLA))) {

		if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry && m_pMonsterData->findBuffById(BuffType::BUFF_CAV)) {
			GetDamageCollision()->SectorDamage(200.f, 90.f, 500.f, 0);
		} else if (GetDamageCollision() && GetFocusRole()) {
			GetDamageCollision()->SingleDamage(GetFocusRole()->GetObjectId(), 0, 200);
		}
	}
	else {
		if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Archer) {
			int nSkillId = 21001;
			if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Archer && m_pMonsterData->findBuffById(BuffType::BUFF_ARC))
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
			if (m_pMonsterData->findBuffById(BuffType::BUFF_MAG))
			{
				nSkillId = 21005;
			}
			SkillMgr::getInstance().CreateSkill(nSkillId, this, GetFocusRole(), GetFocusRole()->GetActorLocation(), FVector::ZeroVector);
		}
	}
}

void ASoldier::UnRiderHorse()
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


bool ASoldier::IsWaitBegin()
{
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

void ASoldier::Death()
{
	if (m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry || m_nGenelTpy == NpcTypeCfg::NpcSubType_Blade) {  //死亡下马
		UnRiderHorse();
	}
	if (!IsInMainCity())
	{
		ASoldierMgr::getInstance().RemoveSoldier(GetObjectId());
	}
	return Super::Death();
}


bool ASoldier::CanMove()
{
	//if (IsDefenceing())
	//{
	//	return false;
	//}
	return Super::CanMove();
}

ARole* ASoldier::GetParentGenerals()
{
	if (m_ProtectorUuid != 0) {
		AGenerals* pGen = AGeneralsMgr::getInstance().GetGeneralByUuid(m_ProtectorUuid);
		if (pGen && !pGen->IsDeath())
		{
			return pGen;
		}
	}
	return AIPlayerMgr::getInstance().GetHero();
}

void ASoldier::ResortAttackTargets()
{
	if (m_nGenelTpy != NpcTypeCfg::NpcSubType_Cavalry || !m_pMonsterData || !m_pMonsterData->findBuffById(BuffType::BUFF_CAV))
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

//****************************************************************************************
//
//****************************************************************************************
bool ASoldier::IsCanAttackTarget(ARole* pRole, bool bPassive)
{
	if (!pRole || pRole->IsDeath())
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

		//if (pMonsterTpy->n_nTowerTpy == MonsterCfg::MonsterType::MobTowerTypeCrystal 
		//	&& (m_nGenelTpy == NpcTypeCfg::NpcSubType_Archer || m_nGenelTpy == NpcTypeCfg::NpcSubType_Magic))
		//{
		//	return false;
		//}

		if (pMonster->IsForbidAttack()) {
			return false;
		}

		return true;
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
void ASoldier::onGeneralsSkillEvent(const GeneralsSkillEvent& evt)
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
	else if ((evt.m_type == GeneralsSkillEvent::EventType::Gen_CAV ||
		evt.m_type == GeneralsSkillEvent::EventType::Gen_StartBuff) && m_nGenelTpy == NpcTypeCfg::NpcSubType_Cavalry)
	{
		GameAppExtern::c2cSkillAttack(SkillType::SKILL_CAV, GetObjectId(), GetObjectId());
	}
	else if ((evt.m_type == GeneralsSkillEvent::EventType::Gen_ARC ||
		evt.m_type == GeneralsSkillEvent::EventType::Gen_StartBuff) && m_nGenelTpy == NpcTypeCfg::NpcSubType_Archer)
	{
		GameAppExtern::c2cSkillAttack(SkillType::SKILL_ARC, GetObjectId(), GetObjectId());
	}
	else if (evt.m_type == GeneralsSkillEvent::EventType::Gen_INF_C && m_nGenelTpy == NpcTypeCfg::NpcSubType_Infantry)
	{
		GameAppExtern::c2cNormalDelBuff(BuffType::BUFF_INF, GetObjectId());
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
}

//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
bool ASoldierMgr::initialize()
{
	return true;
}

void ASoldierMgr::terminate()
{
}

bool ASoldierMgr::reset()
{
	m_mapSoldier.clear();
	return true;
}

void ASoldierMgr::update(float dt)
{
}

ASoldierMgr::ASoldierMgr()
{
	initialize();
}

ASoldierMgr& ASoldierMgr::getInstance()
{
	static ASoldierMgr soldierMgr;
	return soldierMgr;
}


void ASoldierMgr::RemoveSoldier(int nMapObjId)
{
	auto&& iter = m_mapSoldier.find(nMapObjId);
	if (iter != m_mapSoldier.end() && iter->second != nullptr) {
		iter->second->DestroyAttach();
		iter->second->DestroyRole();
	}
	m_mapSoldier.erase(nMapObjId);
}

ASoldier* ASoldierMgr::GetSolider(int nObjId)
{
	if (m_mapSoldier.find(nObjId) == m_mapSoldier.end()) {
		return nullptr;
	}
	return m_mapSoldier[nObjId];
}

void ASoldierMgr::PreReset()
{
	auto iter = m_mapSoldier.begin();
	for (; iter != m_mapSoldier.end(); iter++)
	{
		iter->second->Reset();
	}
}


void ASoldierMgr::CreateSoldier(int nMapObjectId, NS::I64 nProtectId, int mMonsterId, FTransform trans)
{
	if (!gp_UWorld)
	{
		return;
	}

	if (m_mapSoldier.find(nMapObjectId) != m_mapSoldier.end()) {
		return;
	}

	auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(nProtectId);
	if (!data)
	{
		return;
	}

	//
	UClass* generalClass = LoadObject<UClass>(NULL, TEXT("Blueprint'/Game/Models/Soldier/entity_s/soldier/BP_Soldier.BP_Soldier_C'"));
	if (!generalClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("ASoldierMgr::CreateSoldier failed"));
		return;
	}

	FActorSpawnParameters params = FActorSpawnParameters();
	params.bNoFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASoldier* pSoldier = gp_UWorld->SpawnActor<ASoldier>(generalClass, trans, params);
	if (!pSoldier) {
		return;
	}
	pSoldier->SetProtectorUuid(nProtectId);
	float fSpeed = 400.f;
	auto&& pBadgeCfg = NpcBadgeCfg::getInstance().findNpcBadgeCfg(data->itemid, data->level);
	if (pBadgeCfg) {
		fSpeed = pBadgeCfg->n_Speed;
	}
	//pSoldier->SetNpcSpeed(fSpeed);
	std::shared_ptr<ItemData> pItemData = ItemPacketMgr::getInstance().findItemDataByUuid(data->weaponId1);
	if (pItemData)
	{
		pSoldier->SetWeaponId(pItemData->getItemId());
	}
	std::shared_ptr<ItemData> pItemDataSub = ItemPacketMgr::getInstance().findItemDataByUuid(data->weaponId2);
	if (pItemDataSub)
	{
		pSoldier->SetSubWeaponId(pItemDataSub->getItemId());
	}
	pSoldier->InitSkin();
	pSoldier->SetMonsterId(nMapObjectId, nProtectId, mMonsterId);
	m_mapSoldier[nMapObjectId] = pSoldier;

	UReichGameInstance* GameInstance = Cast<UReichGameInstance>(gp_UWorld->GetGameInstance());
	if (!GameInstance)
	{
		return;
	}

	AInstanceLevel* pLevel = Cast<AInstanceLevel>(GameInstance->GetCurrentLevel());
	if (!pLevel)
	{
		return;
	}
	if (pLevel->IsStartBattle()) {
		pLevel->InitSoliderTargets(pSoldier);
	}
}
