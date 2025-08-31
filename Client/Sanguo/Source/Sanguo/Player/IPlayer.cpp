// Fill out your copyright notice in the Description page of Project Settings.


#include "IPlayer.h"
#include "Engine/Blueprint.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "data/ItemData.h"
#include "msg/MsgItem.pb.h"

#include "Weapon.h"
#include "WeaponEffect.h"
#include "BuffEffect.h"
#include "Skill/SkillEffect.h"
#include "config/SkillTypeCfg.h"
#include "HorseInstance.h"

#include "config/WeaponTypeCfg.h"
#include "config/WeaponInfoTypeCfg.h"
#include "config/RoleCreateCfg.h"
#include "config/WeaponAniEffectCfg.h"
#include "config/GlobalCfg.h"
#include "../ReichGameInstance.h"
#include "../UI/UILobby.h"
#include "../UI/UIManager.h"
#include "Hero.h"

#define COMBO_DELAY_TM 1.5f


//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::Init()
{
	m_nComboIdx = 0;
	m_nComboNum = 0;
	m_bMirror = false;
	m_bBlend = false;
	m_idWeapon = 0;
	m_EquipInfo.nWeaponId = 110001;  //110001为无武器状态

	//
	m_EquipInfo.nEquipBodyId = 0;
	m_EquipInfo.nEquipHeadId = 0;
	m_EquipInfo.nEquipLegsId = 0;
	m_EquipInfo.nEquipCapeId = 0;
	m_EquipInfo.nEquipWrapId = 0;
	m_PlayerInfo.nSkinColor = 7;
	m_PlayerInfo.nHairColor = 4;
	SetAttacking(false);

	ARole::Init();
}

void AIPlayer::BeginPlay()
{
	Super::BeginPlay();
	//AttachWeaponById();
	//ResetAttr();
	//CreateMyCollision(100);

	//RG_REGISTER_MYEVENT(MsgInteractionS2C, &AIPlayer::onMsgInteractionS2C);
	//RG_REGISTER_MYEVENT(MsgMagicEffectAppenceS2C, &AIPlayer::onMsgMagicEffectAppenceS2C);
	//RG_REGISTER_MYEVENT(MsgPlayerInfoGS2C, &AIPlayer::onMsgPlayerInfoGS2C);
}

void AIPlayer::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//UNRG_REGISTER_MYEVENT(MsgMapObjectUnderAttackG2C, this);
	//UNRG_REGISTER_MYEVENT(MsgInteractionS2C, this);
	//UNRG_REGISTER_MYEVENT(MsgMagicEffectAppenceS2C, this);
	//UNRG_REGISTER_MYEVENT(MsgPlayerInfoGS2C, this);
	m_EquipInfo.nWeaponId = 110001;
}

int AIPlayer::GetObjectId()
{ 
	if (m_playerData) {
		return m_playerData->getId();
	}
	return 0;
}

int AIPlayer::GetRoleCtrlStatus()
{
	if (m_playerData)
	{
		return m_playerData->getCtrlState();
	}
	return 0;
}


int AIPlayer::GetHp()
{
	if (!m_playerData) {
		return 0;
	}
	return m_playerData->getHp();
}

int AIPlayer::GetMaxHp()
{
	if (!m_playerData) {
		return 1;
	}
	return m_playerData->getMaxHp();
}

// Called every frame
void AIPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//****************************************************************************************
//
//****************************************************************************************
std::string AIPlayer::GetBattleSound(int nSoundTpy)
{
	if (m_playerData) {
		return m_playerData->GetBattleSound(nSoundTpy);
	}
	return "";
}


//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::InitSkin(int nProfessIdx)
{
	//皮肤
	USkeletalMeshComponent* mesh = GetMesh();
	if (!mesh) {
		return;
	}

	FString strFoder[4] = { FString("Warrior"),FString("Musha"),FString("Mage"),FString("Bravo") };

	FString strTexture[4][10] = {
		{FString("chest_01_10"),FString("chest_01_10"),FString("shoulder_01_10"),FString("chest_01_10"),FString("chest_01_10"),FString("chest_01_10"),FString("hair_01_40"),FString("chest_01_10"),FString(""),FString("")},
		{FString("headwear_06_50"),FString("chest_02_50"),FString("chest_02_50"),FString("headwear_06_50"),FString("chest_02_50"),FString("chest_02_50"),FString("chest_02_50"),FString("shoulder_06_50"),FString("chest_02_50"),FString("")},
		{FString("mage_fameale_01_20"),FString("hair_01_30"),FString("mage_fameale_01_20"),FString("mage_fameale_01_20"),FString("mage_fameale_01_20"),FString("mage_fameale_01_20"),FString("mage_fameale_01_20"),FString("shouder_01"),FString(""),FString("")},
		{FString("chest"),FString("hair"),FString("guisha_headwear_05"),FString("shoulder_05"),FString("chest"),FString("chest"),FString("chest"),FString("chest"),FString("chest"),FString("")} };

	for (int i = 0 ; i < mesh->GetNumMaterials() && i < 10 ; i++)
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

		FString strDifuse;

		strDifuse = FString::Printf(TEXT("%s/%s/Mesh/%s.%s"), *FString("/Game/Player"), *strFoder[nProfessIdx], *strTexture[nProfessIdx][i], *strTexture[nProfessIdx][i]);


		//漫反射贴图
		FString strText = UReichGameInstance::TransTexturePath(TCHAR_TO_UTF8(*strDifuse));
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
AWeapon* AIPlayer::LoadWeapon(FString path)
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
AWeaponEffect* AIPlayer::LoadWeaponEffect(FString path, FTransform trans)
{
	UClass* generalClass = LoadObject<UClass>(NULL, *path);
	if (!generalClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::LoadWeaponEffect failed"));
		return NULL;
	}
	AWeaponEffect* spawnActor = GetWorld()->SpawnActor<AWeaponEffect>(generalClass, trans);
	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::LoadWeaponEffect Success"));
	if (spawnActor)
	{
		return spawnActor;
	}
	return NULL;
}

//****************************************************************************************
//
//****************************************************************************************
UAnimMontage* AIPlayer::LoadHorseMontage(int weaponId)
{
	std::string strPath = WeaponInfoTypeCfg::getInstance().GetAttackAniPath(m_idHorseId);
	if (strPath.empty())
	{
		return NULL;
	}

	FString strAnim = UReichGameInstance::TransAniMotoAssertPath(strPath);
	UObject* loadObj = StaticLoadObject(UAnimMontage::StaticClass(), NULL, strAnim.GetCharArray().GetData());//);//TEXT("Blueprint'/Game/BluePrint/TestObj.TestObj'")
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
UAnimMontage* AIPlayer::LoadAniMontage(int weaponTpy, int comboIdx)
{
	std::string strPath = WeaponTypeCfg::getInstance().GetAniPath(m_PlayerInfo.nProfessIdx, weaponTpy, comboIdx);
	FString strAnim = UReichGameInstance::TransAniMotoAssertPath(strPath);
	UObject* loadObj = StaticLoadObject(UAnimMontage::StaticClass(), NULL, strAnim.GetCharArray().GetData());//);//TEXT("Blueprint'/Game/BluePrint/TestObj.TestObj'")
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
UAnimMontage* AIPlayer::LoadAniByWeaponId(bool bContinue, int& nContinueIdx, bool& bAttackStopMove)
{
	auto pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pCfg || pCfg->n_SkillId == 0)
	{
		return NULL;
	}
	auto pSkillCfg = SkillTypeCfg::getInstance().GetSkillType(pCfg->n_SkillId);
	if (!pSkillCfg)
	{
		return NULL;
	}
	if (pSkillCfg->n_vecAniIdx.size() <= 0)
	{
		return NULL;
	}
	bAttackStopMove = pSkillCfg->n_AttackStopMove == 1;
	if (!bContinue && pSkillCfg->n_AniTpy == Weapon_AniTpy_Normal)
	{
		int nRandIdx = std::rand() % pSkillCfg->n_vecAniIdx.size();
		int nIdx = pSkillCfg->n_vecAniIdx[nRandIdx];
		return LoadAniMontage(GetWeaponTpy(), nIdx);
	}
	else if(bContinue && pSkillCfg->n_AniTpy == Weapon_AniTpy_Continue)
	{
		int nIdx = pSkillCfg->n_vecAniIdx[0];
		if (nIdx < Ani_Continue_Idx1 || nIdx >= Ani_Continue_Num)
		{
			return NULL;
		}
		auto pTpyCfg = WeaponTypeCfg::getInstance().GetWeaponType(GetWeaponTpy());
		if (!pTpyCfg)
		{
			return NULL;
		}
		std::string strAni = WeaponTypeCfg::getInstance().GetContinueAniPath(m_PlayerInfo.nProfessIdx, GetWeaponTpy() , nIdx);
		FString strAnim = UReichGameInstance::TransAniMotoAssertPath(strAni);
		UObject* loadObj = StaticLoadObject(UAnimMontage::StaticClass(), NULL, strAnim.GetCharArray().GetData());//);//TEXT("Blueprint'/Game/BluePrint/TestObj.TestObj'")
		if (loadObj != nullptr)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::LoadAniByWeaponId success"));
			UAnimMontage* ubp = Cast<UAnimMontage>(loadObj);
			if (ubp)
			{
				nContinueIdx = nIdx;
			}
			return ubp;
		}
	}
	return NULL;
}

//****************************************************************************************
//
//****************************************************************************************
int AIPlayer::GetComboNum()
{
	return m_nComboNum;
}

//****************************************************************************************
//
//****************************************************************************************
const FStuPlayerBaseInfo& AIPlayer::GetPlayerBaseInfo()
{
	return m_PlayerInfo;
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::SetPlayerBaseInfo(int nSex, int nHairStyle, int nHairColor, int nSkinColor)
{
	m_PlayerInfo.nProfessIdx = nSex;
	m_PlayerInfo.nHairStyle = nHairStyle;
	m_PlayerInfo.nHairColor = nHairColor;
	m_PlayerInfo.nSkinColor = nSkinColor;
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::ResetPlayerMesh()
{
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MainMesh"));
	if (meshs.Num() <= 0)
	{
		return;
	}
	
	auto mesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (mesh)
	{
		std::string strMainMesh = RoleCreateCfg::getInstance().GetMainSkeletalMesh(m_PlayerInfo.nProfessIdx);
		FString strMesh(strMainMesh.c_str());
		USkeletalMesh* tmpMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *(strMesh)));
		if (tmpMesh)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::ResetPlayerMesh success"));
			mesh->SetSkeletalMesh(tmpMesh);
		}

		std::string strBPAniPath = RoleCreateCfg::getInstance().GetBPAniPath(m_PlayerInfo.nProfessIdx);
		FString strAniPath(strBPAniPath.c_str());
		UAnimBlueprintGeneratedClass* MeshAnim = LoadObject<UAnimBlueprintGeneratedClass>(NULL, *(strAniPath));
		if (MeshAnim)
		{
			mesh->SetAnimInstanceClass((UClass*)(MeshAnim));
		}
	}
}




//****************************************************************************************
//
//****************************************************************************************
FString AIPlayer::GetLWeaponSockName()
{
	auto&& pCfg = WeaponTypeCfg::getInstance().GetWeaponType(GetWeaponTpy());
	if (!pCfg)
	{
		return FString("");
	}
	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById s_SocketName1 = %s weaponTpy = %d"), *FString(pCfg->s_SocketName1.c_str()), GetWeaponTpy());
	return FString(pCfg->s_SocketName1.c_str());
}

//****************************************************************************************
//
//****************************************************************************************
FString AIPlayer::GetRWeaponSockName()
{
	auto&& pCfg = WeaponTypeCfg::getInstance().GetWeaponType(GetWeaponTpy());
	if (!pCfg)
	{
		return FString("");
	}
	UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById s_SocketName2 = %s weaponTpy = %d"), *FString(pCfg->s_SocketName2.c_str()), GetWeaponTpy());
	return FString(pCfg->s_SocketName2.c_str());
}

//****************************************************************************************
//
//****************************************************************************************
bool AIPlayer::IsHero()
{
	AHero* hero = Cast<AHero>(this);
	if (hero)
	{
		return true;
	}
	return false;
}

void AIPlayer::SetLobbyAttackTpy(int nTpy)
{
	if (IsHero()) {
		UUserWidget* pWidget = UIManager::getInstance().GetWidget(UI_TPY_Lobby);
		if (pWidget)
		{
			UUILobby* pLobby = Cast<UUILobby>(pWidget);
			if (pLobby)
			{
				pLobby->SetAttackHitType(nTpy);
			}
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::AttachWeaponById(int nWeaponId)
{
	m_LeftAWeapon = NULL;
	m_RightAWeapon = NULL;
	m_nComboIdx = 0;
	SetAttacking(false);
	KillAttackTimer();
	RemoveWeapons();
	SetLobbyAttackTpy(0);
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
	FString strLSockName = GetLWeaponSockName();
	FString strRSockName = GetRWeaponSockName();
	auto&& pWeapon = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (pWeapon == nullptr)
	{
		return;
	}
	auto&& pWeaponTpy = WeaponTypeCfg::getInstance().GetWeaponType(GetWeaponTpy());
	if (!pWeaponTpy)
	{
		return;
	}

	if (pWeapon->n_isCombo == 1)
	{
		m_nComboNum = pWeaponTpy->n_Combo;
	}
	else {
		m_nComboNum = 0;
	}

	if (pWeapon->n_SkillId > 0)
	{
		auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(pWeapon->n_SkillId);
		if (pCfg)
		{
			SetLobbyAttackTpy(pCfg->n_ScrollHit);
		}
	}

	FVector vecRelativeScale = mesh->GetRelativeTransform().GetScale3D();

	if (pWeapon->n_Flip == 1)
	{
		mesh->SetRelativeScale3D(FVector(-vecRelativeScale.X, vecRelativeScale.Y, vecRelativeScale.Z));
	}
	else {
		mesh->SetRelativeScale3D(FVector(vecRelativeScale.X, vecRelativeScale.Y, vecRelativeScale.Z));
	}
	std::string strPath = WeaponInfoTypeCfg::getInstance().GetBPWeaponPath(GetWeaponId());
	FString strWeaponPath = FString(strPath.c_str());
	if (!strLSockName.IsEmpty())
	{
		AWeapon* aWeapon = LoadWeapon(strWeaponPath);
		if (aWeapon)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById L aWeapon is not null"));
			aWeapon->SetOwnerRole(this);
			if (pWeaponTpy->n_HitHurt == 1)
			{
				aWeapon->RegisterOverlayEvents();
			}
			aWeapon->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*strLSockName));
			aWeapon->SetWeaponId(GetWeaponId());
			m_LeftAWeapon = aWeapon;
		}
	}
	if (!strRSockName.IsEmpty())
	{
		AWeapon* aWeapon = LoadWeapon(strWeaponPath);
		if (aWeapon)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttachWeaponById R aWeapon is not null"));
			aWeapon->SetOwnerRole(this);
			if (pWeaponTpy->n_HitHurt == 1)
			{
				aWeapon->RegisterOverlayEvents();
			}
			aWeapon->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*strRSockName));
			aWeapon->SetWeaponId(GetWeaponId());
			m_RightAWeapon = aWeapon;
		}
	}
	if (IsHero())
	{
		WeaponChangeEvent evt;
		evt.nWeaponId = nWeaponId;
		UI_DISPATCH_MYEVENT(WeaponChangeEvent, evt);
	}
}

void AIPlayer::RideHorse(int nHorseId)
{
	if (m_idHorseId == nHorseId) {
		return;
	}
	if (m_idHorseId > 0 && nHorseId == 0) {
		UnRideHorse();
		return;
	}

	auto&& pWeapon = WeaponInfoTypeCfg::getInstance().GetWeaponById(nHorseId);
	if (pWeapon == nullptr)
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

	auto&& horseAnim = Cast<UHorseInstance>(mountMesh->GetAnimInstance());
	if (horseAnim) {
		horseAnim->StopAudio();
	}

	std::string strPath = WeaponInfoTypeCfg::getInstance().GetSkeshWeaponPath(nHorseId);
	FString setPathM = FString(strPath.c_str());
	USkeletalMesh* tmpMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *(setPathM)));
	if (!tmpMesh)
	{
		UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::RideHorse false"));
		return;
	}
	if (pWeapon->m_vecTextures.size() > 0)
	{
		for (int i = 0 ; i < pWeapon->m_vecTextures.size(); i++)
		{
			auto skinMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
				TEXT("Material'/Game/Weapon/Weapon_M/Com_Weapon_M.Com_Weapon_M'")));
			if (!skinMaterial) {
				UE_LOG(LogOutputDevice, Log, TEXT("AWeapon::SetWeaponId false"));
				return;
			}

			std::string strDifuse = WeaponInfoTypeCfg::getInstance().GetTexture(nHorseId, i);
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
	mountMesh->SetRelativeScale3D(FVector(pWeapon->n_RangeParam1/100.f, pWeapon->n_RangeParam1 / 100.f, pWeapon->n_RangeParam1 / 100.f));
	mountMesh->SetHiddenInGame(false);
	strPath = WeaponInfoTypeCfg::getInstance().GetAnimHorse(nHorseId);
	FString setAnimPath = FString(strPath.c_str());
	UClass* horseClass = LoadObject<UClass>(NULL, *setAnimPath);
	if (!horseClass) {
		return;
	}
	m_idHorseId = nHorseId;
	mountMesh->SetAnimClass(horseClass);
	SetRiding(true);
	if (IsHero())
	{
		HorseChangeEvent evt;
		evt.m_nSkill = pWeapon->n_SkillId;
		UI_DISPATCH_MYEVENT(HorseChangeEvent, evt);
	}
	ResetRunAudio();
}

void AIPlayer::UnRideHorse()
{
	SetRiding(false);
	m_idHorseId = 0;
	ResetRunAudio();
	if (IsHero())
	{
		HorseChangeEvent evt;
		evt.m_nSkill = 0;
		UI_DISPATCH_MYEVENT(HorseChangeEvent, evt);
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::RemoveWeapons()
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
		for (int i = 0 ; i < arrayActors.Num() ; i++)
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
	m_LeftAWeapon = nullptr;
	m_RightAWeapon = nullptr;
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::ShowDarts(bool bShow)
{
	TArray<AActor*> arrayActors;
	this->GetAttachedActors(arrayActors);
	if (m_LeftAWeapon)
	{
		TArray<UActorComponent*> meshs = m_LeftAWeapon->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("DartMesh"));
		if (meshs.Num() > 0)
		{
			for (int i = 0; i < meshs.Num(); i++)
			{
				auto mesh = Cast<USkeletalMeshComponent>(meshs[i]);
				if (mesh)
				{
					mesh->SetVisibility(bShow);
				}
			}
		}

		TArray<UActorComponent*> staticMeshs = m_LeftAWeapon->GetComponentsByTag(UStaticMeshComponent::StaticClass(), TEXT("DartMesh"));
		if (staticMeshs.Num() > 0)
		{
			for (int i = 0; i < staticMeshs.Num(); i++)
			{
				auto mesh = Cast<UStaticMeshComponent>(staticMeshs[i]);
				if (mesh)
				{
					mesh->SetVisibility(bShow);
				}
			}
		}
	}

	if (bShow) {
		auto aniMota = this->GetCurrentMontage();
		if (aniMota)
		{
			this->StopAnimMontage(aniMota);
		}
	}	
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::Death()
{
	UnRideHorse();
	auto aniMota = this->GetCurrentMontage();
	if (aniMota)
	{
		this->StopAnimMontage(aniMota);
	}
	std::string strPath = WeaponTypeCfg::getInstance().GetDeathPath(m_PlayerInfo.nProfessIdx, GetWeaponTpy());
	FString strAnim = UReichGameInstance::TransAniMotoAssertPath(strPath);
	UObject* loadObj = StaticLoadObject(UAnimMontage::StaticClass(), NULL, strAnim.GetCharArray().GetData());//);//TEXT("Blueprint'/Game/BluePrint/TestObj.TestObj'")
	if (loadObj == nullptr)
	{
		return;
	}
	UAnimMontage* ubp = Cast<UAnimMontage>(loadObj);
	if (!ubp)
	{
		return;
	}
	this->PlayAnimMontage(ubp);
	ARole::Death();
}

//****************************************************************************************
//
//****************************************************************************************
bool AIPlayer::IsCanAttack()
{
	if (IsDeath()) {
		return false;
	}
	auto&& pWeaponCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pWeaponCfg)
	{
		return false;
	}
	auto&& pCfgSkill = SkillTypeCfg::getInstance().GetSkillType(pWeaponCfg->n_SkillId);
	if (pCfgSkill && pCfgSkill->n_AniTpy == ENU_ATK_ANI_TPY_CONTINUE)
	{
		return false;
	}

	if (!m_playerData) {
		return false;
	}

	if (IsAttacking())
	{
		return false;
	}
	return true;
}

//****************************************************************************************
//
//****************************************************************************************
bool AIPlayer::PlayAttack()
{
	if (!IsCanAttack()) {
		return false;
	}

	if (m_nComboNum > 0 && !CanAttack()) {  //不能使用普通攻击
		return false;
	}

	if (m_nComboNum == 0 && !CanSkillAttack()) { //不能使用技能攻击
		return false;
	}

	KillAttackTimer();
	auto aniMota = this->GetCurrentMontage();
	if (aniMota)
	{
		this->StopAnimMontage(aniMota);
	}
	if (m_nComboIdx >= m_nComboNum)
	{
		m_nComboIdx = 0;
	}
	UAnimMontage* aniMotaToPlay = NULL;
	int nContinueIdx = 0;
	bool bAttackStopMove = false;
	if (m_nComboNum > 0)
	{
		//连招动画
		aniMotaToPlay = LoadAniMontage(GetWeaponTpy(), m_nComboIdx);
	}
	else {
		//技能攻击动画
		aniMotaToPlay = LoadAniByWeaponId(false , nContinueIdx , bAttackStopMove);
	}
	
	if (!aniMotaToPlay)
	{
		m_nComboIdx = 0;
		return false;
	}

	bool bForwardFocus = false;
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (pCfg&& pCfg->n_SkillId > 0)
	{
		auto&& skillCfg = SkillTypeCfg::getInstance().GetSkillType(pCfg->n_SkillId);
		if (skillCfg)
		{
			bForwardFocus = skillCfg->n_AttackTarget == 1;
		}
	}

	if (GetFocusRole() != nullptr && bForwardFocus)
	{
		auto from = GetActorLocation();
		auto to = GetFocusRole()->GetActorLocation();
		auto toRota = UKismetMathLibrary::FindLookAtRotation(from , to);
		FRotator NewRotation(GetActorRotation().Pitch , toRota.Yaw , GetActorRotation().Roll);
		SetActorRotation(NewRotation);
	}
	aniMotaToPlay->BlendIn.SetBlendTime(0.f);
	aniMotaToPlay->BlendOut.SetBlendTime(0.05f);
	float tm = PlayAnimMontage(aniMotaToPlay, GetSpeedRate());
	if (m_nComboNum > 0)
	{
		PlayWeaponSoundEffect(m_nComboIdx);
		m_nComboIdx++;
		CalDamageRange();
		tm += COMBO_DELAY_TM;
		if (pCfg && pCfg->n_normalStrenth > 0 && this == AIPlayerMgr::getInstance().GetHero()) {
			GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_STRENTH, false, pCfg->n_normalStrenth);
		}
	}
	GetWorldTimerManager().SetTimer(MyTimerHand, this, &AIPlayer::DelayReset, tm, false);
	SetAttacking(true);
	SetAttackStopMove(bAttackStopMove);
	return true;
}

void AIPlayer::PlaySkillAttack(int m_nSkillId)
{
	if (m_nSkillId == 0 || IsAttacking())
	{
		return;
	}
	KillAttackTimer();
	auto aniMota = this->GetCurrentMontage();
	if (aniMota)
	{
		this->StopAnimMontage(aniMota);
	}
	
	auto pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pCfg || pCfg->n_exSkillId != m_nSkillId)
	{
		return;
	}
	auto pSkillCfg = SkillTypeCfg::getInstance().GetSkillType(m_nSkillId);
	if (!pSkillCfg)
	{
		return ;
	}
	if (pSkillCfg->n_vecAniIdx.size() <= 0)
	{
		return;
	}

	if (pSkillCfg->f_CoolTime > 0.000001f) {
		if (IsSkillCoolTime(m_nSkillId)) {
			return;
		}
		AddSkillTime(m_nSkillId, pSkillCfg->f_CoolTime);
	}

	bool bAttackStopMove = pSkillCfg->n_AttackStopMove == 1;
	int nRandIdx = std::rand() % pSkillCfg->n_vecAniIdx.size();
	int nIdx = pSkillCfg->n_vecAniIdx[nRandIdx];
	UAnimMontage* aniMotaToPlay = LoadAniMontage(GetWeaponTpy(), nIdx);
	
	if (!aniMotaToPlay)
	{
		return;
	}
	bool bForwardFocus = true;
	if (pSkillCfg->n_ScrollHit != SkillEffectHitTpy_None)
	{
		bForwardFocus = false;
	}

	if (GetFocusRole() != nullptr && bForwardFocus)
	{
		auto from = GetActorLocation();
		auto to = GetFocusRole()->GetActorLocation();
		auto toRota = UKismetMathLibrary::FindLookAtRotation(from, to);
		FRotator NewRotation(GetActorRotation().Pitch, toRota.Yaw, GetActorRotation().Roll);
		SetActorRotation(NewRotation);
	}
	aniMotaToPlay->BlendIn.SetBlendTime(0.f);
	float tm = PlayAnimMontage(aniMotaToPlay, GetSpeedRate());
	GetWorldTimerManager().SetTimer(MyTimerHand, this, &AIPlayer::DelayReset, tm, false);
	SetAttacking(true);
	SetAttackStopMove(bAttackStopMove);
}

void AIPlayer::KillAttackTimer()
{
	GetWorldTimerManager().ClearTimer(MyTimerHand);
}

void AIPlayer::DelayReset()
{
	m_nComboIdx = 0;
	SetAttacking(false);
	SetAttackStopMove(false);
}


//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayAttackContinue()
{
	if (IsAttacking())
	{
		return;
	}
	KillAttackTimer();
	auto aniMota = this->GetCurrentMontage();
	if (aniMota)
	{
		this->StopAnimMontage(aniMota);
	}
	if (m_nComboIdx >= m_nComboNum)
	{
		m_nComboIdx = 0;
	}
	int nContinueIdx = 0;
	bool bAttackStopMove = false;
	UAnimMontage* aniMotaToPlay = LoadAniByWeaponId(true , nContinueIdx , bAttackStopMove);
	if (!aniMotaToPlay)
	{
		return;
	}
	m_nContinueIdx = nContinueIdx;
	if (GetFocusRole() != nullptr)
	{
		auto from = GetActorLocation();
		auto to = GetFocusRole()->GetActorLocation();
		auto toRota = UKismetMathLibrary::FindLookAtRotation(from, to);
		FRotator NewRotation(GetActorRotation().Pitch, toRota.Yaw, GetActorRotation().Roll);
		SetActorRotation(NewRotation);
	}
	PlayAnimMontage(aniMotaToPlay, GetSpeedRate());
	SetAttacking(true);
	SetAttackStopMove(bAttackStopMove);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::StopAttackContinue()
{
	if (m_nContinueIdx > 0)
	{
		SetAttacking(false);
		SetAttackStopMove(false);
		SkillStopContinueEvent evt;
		evt.m_nId = GetObjectId();
		UI_DISPATCH_MYEVENT(SkillStopContinueEvent, evt);
		m_nContinueIdx = 0;
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::AttackEffect()
{
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
	if (m_nComboNum > 0)  //连招动画
	{
		int nComboIdx = m_nComboIdx - 1 < 0 ? m_nComboNum - 1 : m_nComboIdx - 1;
		auto pCfg = WeaponAniEffectCfg::getInstance().GetWeaponAniEffect(GetWeaponId(), m_nComboIdx);
		if (!pCfg)
		{
			pCfg = WeaponAniEffectCfg::getInstance().GetWeaponAniEffect(GetWeaponTpy(), m_nComboIdx);
			if (!pCfg)
			{
				return;
			}
		}
		FString strSock(pCfg->s_SockName.c_str());
		TArray<FName> arr;
		arr = mesh->GetAllSocketNames();
		FTransform trans = this->GetTransform();
		if (arr.Contains(*strSock)) {
			trans = mesh->GetSocketTransform(*strSock);
			trans.SetRotation(this->GetTransform().GetRotation());
		}
		
		FString strPath(pCfg->s_AssertPath.c_str());

		UClass* generalClass = LoadObject<UClass>(NULL, *strPath);
		if (!generalClass) {
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::AttackEffect failed"));
			return;
		}
		
		AWeaponEffect* pActEff = GetWorld()->SpawnActor<AWeaponEffect>(generalClass, trans);
		if (pActEff)
		{
			pActEff->DieDelay(pCfg->f_DisplayTm);
		}
	}
}

std::string AIPlayer::GetHitEffectPath()
{
	if (m_nComboNum > 0)  //连招动画
	{
		int nComboIdx = m_nComboIdx - 1 < 0 ? m_nComboNum - 1 : m_nComboIdx - 1;
		auto pCfg = WeaponAniEffectCfg::getInstance().GetWeaponAniEffect(GetWeaponId(), m_nComboIdx);
		if (!pCfg)
		{
			pCfg = WeaponAniEffectCfg::getInstance().GetWeaponAniEffect(GetWeaponTpy(), m_nComboIdx);
			if (!pCfg)
			{
				return "";
			}
		}
		return pCfg->s_HitAssertPath;
	}
	return "";
}

//****************************************************************************************
//
//****************************************************************************************
bool AIPlayer::IsRiding()
{
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MOUNT"));
	if (meshs.Num() <= 0)
	{
		return false;
	}

	USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (!mesh)
	{
		return false;
	}
	return mesh->bHiddenInGame == 0;
}

void AIPlayer::SetRiding(bool bRiding)
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
	mesh->SetHiddenInGame(!bRiding);
	if (bRiding) {
		meshMain->AttachToComponent(mesh, FAttachmentTransformRules::KeepRelativeTransform, FName("SockRider"));
	}
	else {
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
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::SetWeaponId(int nWeaponId)
{
	if (nWeaponId == 0)
	{
		nWeaponId = 1300000;
	}
	if (m_EquipInfo.nWeaponId == nWeaponId)
	{
		return;
	}
	
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

	int nSkillId = pCfg->n_SkillId;
	if (nSkillId == 0)
	{
		nSkillId = pCfg->n_PreSkillId;
	}
	SetAttackDistance(100);

	if (nSkillId > 0)
	{
		auto pSkillCfg = SkillTypeCfg::getInstance().GetSkillType(nSkillId);
		if (pSkillCfg)
		{
			SetAttackDistance(pSkillCfg->n_length);
		}
	}
	
	m_nComboIdx = 0;
	StopAttackContinue();
	SetAttackStopMove(false);
	m_EquipInfo.nWeaponId = nWeaponId;
	m_EquipInfo.nWeaponTpy = pCfg->n_Type;
	AttachWeaponById(nWeaponId);
}


//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayHorseAttack()
{
	if (m_idHorseId == 0)
	{
		return;
	}
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(m_idHorseId);
	if (!pCfg)
	{
		return;
	}
	if (pCfg->n_SkillId == 0)
	{
		return;
	}
	bool bAttackStopMove = false;
	auto pSkillCfg = SkillTypeCfg::getInstance().GetSkillType(pCfg->n_SkillId);
	if (!pSkillCfg)
	{
		return;
	}
	if (pSkillCfg->f_CoolTime > 0.000001f) {
		if (IsSkillCoolTime(pCfg->n_SkillId))
		{
			return;
		}
		AddSkillTime(pCfg->n_SkillId, pSkillCfg->f_CoolTime);
	}
	
	bAttackStopMove = pSkillCfg->n_AttackStopMove == 1;
	UAnimMontage* aniMotaToPlay = LoadHorseMontage(m_idHorseId);
	if (!aniMotaToPlay)
	{
		StartSkill(pCfg->n_SkillId, this, GetFocusRole(), m_vecScrollPos, m_rotScroll);
		return;
	}
	
	StartSkill(pCfg->n_SkillId, this, GetFocusRole(), m_vecScrollPos, m_rotScroll);
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MOUNT"));
	if (meshs.Num() <= 0)
	{
		return;
	}
	USkeletalMeshComponent* mountMesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (!mountMesh)
	{
		return;
	}
	float tm = 0.f;
	aniMotaToPlay->BlendIn.SetBlendTime(0.f);
	UAnimInstance * AnimInstance = mountMesh->GetAnimInstance();
	if (AnimInstance)
	{
		tm = AnimInstance->Montage_Play(aniMotaToPlay, GetSpeedRate());
	}
	if (bAttackStopMove && tm > 0.1f)
	{
		KillAttackTimer();
		GetWorldTimerManager().SetTimer(MyTimerHand, this, &AIPlayer::DelayReset, tm, false);
		SetAttackStopMove(bAttackStopMove);
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::HorseEffect()
{
	if (m_idHorseId == 0)
	{
		return;
	}
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(m_idHorseId);
	if (!pCfg)
	{
		return;
	}
	if (pCfg->n_SkillId == 0)
	{
		return;
	}
	StartSkill(pCfg->n_SkillId, this, GetFocusRole(), m_vecScrollPos, m_rotScroll);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::AddBuff(int nBuffId)
{
	ARole::AddBuff(nBuffId);
}


//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::RemoveBuff(int nBuffId)
{
	ARole::RemoveBuff(nBuffId);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayWeaponSkillEffect()
{
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pCfg || pCfg->n_SkillId == 0)
	{
		return;
	}
	StartSkill(pCfg->n_SkillId, this, GetFocusRole() , m_vecScrollPos, m_rotScroll);
}

void AIPlayer::StartSkill(int nSkillId, ARole* attackRole, ARole* targetRole, FVector vec, FVector rator)
{
	if (!GameAppExtern::IsClientMode() && !GameAppExtern::IsServer()) {
		return;
	}
	SkillMgr::getInstance().CreateSkill(nSkillId, this, targetRole, vec, rator);
	if (GameAppExtern::IsServer()) {
		MsgMagicEffectAppenceS2C msg;
		msg.set_skillid(nSkillId);
		msg.set_player_iggid(this->m_playerData->getIggId());
		msg.set_objid(this->m_playerData->getId());
		if (GetFocusRole()) {
			//msg.set_target_objid(GetFocusRole()->m_playerData->getId());
			//msg.set_target_player_iggid(GetFocusRole()->m_playerData->getId());
		}
		auto& pos = *msg.mutable_p();
		pos.set_x(vec.X * 1000000);
		pos.set_y(vec.Y * 1000000);
		pos.set_z(vec.Z * 1000000);
		auto& rot = *msg.mutable_r();
		rot.set_r(rator.X * 1000000);
		rot.set_p(rator.Y * 1000000);
		rot.set_y(rator.Z * 1000000);
		GameAppExtern::sendMsg(msg.msgtype(), msg);
	}
}


//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayWeaponComboEffect()
{
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pCfg)
	{
		return;
	}
	if (pCfg->n_ComboSkill == 0)
	{
		return;
	}
	auto&& hero = AIPlayerMgr::getInstance().GetHero();

	if (hero == this && !hero->CheckSkillConsume(0, AHero::ATK_TPY_SKILL_COMBO, false)) {
		return;
	}

	StartSkill(pCfg->n_ComboSkill, this, GetFocusRole(), m_vecScrollPos, m_rotScroll);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayTriggerSkillEffect()
{
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pCfg)
	{
		return;
	}
	if (pCfg->n_exSkillId == 0)
	{
		return;
	}
	StartSkill(pCfg->n_exSkillId, this, GetFocusRole(), m_vecScrollPos, m_rotScroll);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayWeaponPreSkillEffect()
{
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pCfg)
	{
		return;
	}
	if (pCfg->n_PreSkillId == 0)
	{
		return;
	}
	auto&& hero = AIPlayerMgr::getInstance().GetHero();

	if (hero == this && !hero->CheckSkillConsume(0, AHero::ATK_TPY_SKILL_PERSKILL, false)) {
		return;
	}

	StartSkill(pCfg->n_PreSkillId, this, GetFocusRole(), m_vecScrollPos, m_rotScroll);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayWeaponPreHurt(int weaponIdx)
{
	if (m_LeftAWeapon && (weaponIdx == 0 || weaponIdx == 2))
	{
		m_LeftAWeapon->StartPreHurt();
	}
	if (m_RightAWeapon && (weaponIdx == 1 || weaponIdx == 2))
	{
		m_RightAWeapon->StartPreHurt();
	}
}

//伤害后
//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayWeaponHurt(int weaponIdx)
{
	if (m_LeftAWeapon && (weaponIdx == 0 || weaponIdx == 2))
	{
		m_LeftAWeapon->EndHurt();
	}
	if (m_RightAWeapon && (weaponIdx == 1 || weaponIdx == 2))
	{
		m_RightAWeapon->EndHurt();
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::CalDamageRange()
{
	AMyCollision* pMyCollision = GetDamageCollision();
	if (!pMyCollision)
	{
		return;
	}
	auto&& pCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(GetWeaponId());
	if (!pCfg)
	{
		return;
	}
	if (pCfg->n_HurtRangeTpy == 0)
	{
		return;
	}
	switch (pCfg->n_HurtRangeTpy)
	{
		case 1:
			pMyCollision->CircleDamage(pCfg->n_RangeParam1);
			break;
		case 2:
			pMyCollision->SectorDamage(pCfg->n_RangeParam1, pCfg->n_RangeAngle, pCfg->n_RangeParam2);
			break;
		case 3:
			pMyCollision->RectangleDamage(pCfg->n_RangeParam1, pCfg->n_RangeParam2, pCfg->n_RangeAngle);
			break;
		default:
			break;
	}
}


//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
FVector AIPlayer::GetMagicBeginLocationByName(const char* pszSockName)
{
	FTransform trans;
	if (pszSockName == nullptr)
	{
		return GetActorLocation();
	}
	FString strSockName(pszSockName);
	if (strSockName.IsEmpty())
	{
		return GetActorLocation();
	}
	if (m_LeftAWeapon)
	{
		TArray<UActorComponent*> meshs = m_LeftAWeapon->K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass());
		for (int i = 0; i < meshs.Num(); i++)
		{
			auto mesh = Cast<USkeletalMeshComponent>(meshs[i]);
			if (mesh && mesh->GetSocketByName(*strSockName))
			{
				return mesh->GetSocketTransform(*strSockName).GetLocation();
			}
		}
	}
	else if (m_RightAWeapon) {
		TArray<UActorComponent*> meshs = m_RightAWeapon->K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass());
		for (int i = 0; i < meshs.Num(); i++)
		{
			auto mesh = Cast<USkeletalMeshComponent>(meshs[i]);
			if (mesh && mesh->GetSocketByName(*strSockName))
			{
				return mesh->GetSocketTransform(*strSockName).GetLocation();
			}
		}
	}
	return GetRoleTransByName(pszSockName).GetLocation();
}


//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
//获取武器拖尾特效粒子地址
FString AIPlayer::GetTrailParticlePath()
{
	return FString("ParticleSystem'/Game/effect/player/sword_trail/swordtrail_P.swordtrail_P'");
}

//********************************************************************************************************************************************************************************
//设置状态材质参数
//********************************************************************************************************************************************************************************
void AIPlayer::SetMaterialScalerParamer(FName name, float fValue)
{
	if(m_LeftAWeapon)
	{
		m_LeftAWeapon->SetMeshMaterialScalerParamer(name , fValue);
	}
	if (m_RightAWeapon)
	{
		m_RightAWeapon->SetMeshMaterialScalerParamer(name, fValue);
	}
	
		
	ARole::SetMaterialScalerParamer(name , fValue);
}

bool AIPlayer::IsCanAttackTarget(ARole* pRole, bool bPassive)
{
	if (IsDeath() || !pRole || pRole->IsDeath())
	{
		return false;
	}
	if (!pRole->IsCanBeAttackTarget(this)) {
		return false;
	}
	if (pRole->GetCamp() == CampState::CAMP_MONSTER)
	{
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
void AIPlayer::ResetWeapon()
{
	if (!m_playerData) {
		return;
	}
	auto&& itemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::EQUIPMENT, EQUIP_PACKET_WEAPON);
	int nItemId = 1300000;
	if (itemData) {
		nItemId = itemData->getItemId();
	}
	MsgInteractionC2S msg;
	msg.set_player_iggid(m_playerData->getIggId());
	msg.set_action_type(ACTION_TYPE_WEAPON);
	msg.set_data1(nItemId);
	GameAppExtern::sendMsg(msg.msgtype(), msg);
}

void AIPlayer::ResetHorse()
{
	if (!m_playerData)
	{
		return;
	}
	auto&& itemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::EQUIPMENT, EQUIP_PACKET_HORSE);
	int nItemId = 0;
	if (itemData) {
		nItemId = itemData->getItemId();
	}
	MsgInteractionC2S msg;
	msg.set_player_iggid(m_playerData->getIggId());
	msg.set_action_type(ACTION_TYPE_HORSE);
	msg.set_data1(nItemId);
	GameAppExtern::sendMsg(msg.msgtype(), msg);
}

//******************************************************************************************************************************
//
//******************************************************************************************************************************
void AIPlayer::ResetAttr()
{
	ResetSpeed();
	ResetAtkSpeed();
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::ResetSpeed()
{
	if (!m_playerData) {
		return;
	}
	int nSpeed = 0;
	auto playerAttr = PlayerAttrCfg::getInstance().findPlayerAttr(m_playerData->getLevel());
	if (playerAttr) {
		nSpeed = playerAttr->n_Speed;
	}
	
	auto&& itemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::EQUIPMENT, EQUIP_PACKET_HORSE);
	if (itemData && itemData->m_pWeaponCfg) {
		nSpeed = itemData->m_pWeaponCfg->n_Speed;
	}

	if (nSpeed == 0) {
		nSpeed = GlobalCfg::getInstance().GetPlayerInitSpeed();
	}

	if (m_playerData) {
		float addPhase = m_playerData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CHASE);
		//float addSpeed = m_playerData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MV_SP);
		nSpeed += (addPhase - 1.f) * 60;

		if (m_playerData->getMaxFood() > 0) {
			float fPer = m_playerData->getFood()*1.f / m_playerData->getMaxFood();
			if (fPer < 0.3f)
			{
				nSpeed *= (0.5f + 0.5f*(fPer/0.3f));
			}
		}
	}

	nSpeed = std::max(50, std::min(1000, nSpeed));

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent) {
		// 设置角色的移动速度
		MovementComponent->MaxWalkSpeed = static_cast<float>(nSpeed); // 设置为300米/秒
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::ResetAtkSpeed()
{
	if (!m_playerData) {
		return;
	}

	float fAtkSpeed = m_playerData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ATK_SP);
	int nSpeed = 100 + (fAtkSpeed - 1.0f) * 20;
	nSpeed = std::min(500, std::max(50, nSpeed));
	SetAtkSpeed(nSpeed);
}

bool AIPlayer::IsDeath()
{
	if (!m_playerData) {
		return false;
	}
	return m_playerData->getHp() <= 0;
}

void AIPlayer::SetProfessIdx(int nProfessIdx)
{
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MainMesh"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(meshs[0]);
	if (!mesh)return;
	if (nProfessIdx >= 4) {
		return;
	}
	FString strMeshs[4] = { FString("SkeletalMesh'/Game/Player/Warrior/Mesh/warrior.warrior'"),
					   FString("SkeletalMesh'/Game/Player/Musha/Mesh/musha.musha'"),
					   FString("SkeletalMesh'/Game/Player/Mage/Mesh/Mage.Mage'"),
					   FString("SkeletalMesh'/Game/Player/Bravo/Mesh/Bravo.Bravo'") };
	FString strAni[4] = { FString("AnimBlueprint'/Game/Player/Warrior/ANI_Warrior.ANI_Warrior_C'"),
					   FString("AnimBlueprint'/Game/Player/Musha/ANI_Musha.ANI_Musha_C'"),
					   FString("AnimBlueprint'/Game/Player/Mage/ANI_Mage.ANI_Mage_C'"),
					   FString("AnimBlueprint'/Game/Player/Bravo/ANI_Bravo.ANI_Bravo_C'") };
	USkeletalMesh* tmpMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *(strMeshs[nProfessIdx])));
	//UAnimInstance* aniIns = Cast<UAnimInstance>(StaticLoadObject(UAnimInstance::StaticClass(), NULL, *(strAni[nProfessIdx])));
	UClass* aniClass = LoadObject<UClass>(NULL, *(strAni[nProfessIdx]));
	if (!tmpMesh || !aniClass)
	{
		return;
	}
	
	mesh->SetSkeletalMesh(tmpMesh);
	mesh->SetAnimInstanceClass(aniClass);
	m_PlayerInfo.nProfessIdx = nProfessIdx;
	InitSkin(nProfessIdx);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::ResetRunAudio()
{
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
	bool bRiding = m_idHorseId > 0;
	UHorseInstance* pIns = Cast<UHorseInstance>(mountMesh->GetAnimInstance());
	if (pIns) {
		if (!bRiding) {
			pIns->StopAudio();
		}
		else {
			pIns->StartAudio();
		}
	}
	UHorseInstance* pInsMain = Cast<UHorseInstance>(mesh->GetAnimInstance());
	if (pInsMain) {
		if (!bRiding) {
			pIns->StartAudio();
		} else {
			pIns->StopAudio();
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::SendInPlayerSight(NS::I64  iggId)
{
	if (iggId == 0 || iggId == m_playerData->getIggId()) {
		return;
	}
	AIPlayer* pCurPlayer = AIPlayerMgr::getInstance().GetPlayer(iggId);
	if (!pCurPlayer) {
		return;
	}
	NS::I64  serverIggId = AIPlayerMgr::getInstance().GetHero()->m_playerData->getIggId();
	MsgInteractionInWorldS2C msg;
	msg.set_objid(m_playerData->getId());
	msg.set_iggid(serverIggId);
	msg.set_player_iggid(m_playerData->getIggId());
	msg.set_target_iggid(iggId);
	msg.set_sex(pCurPlayer->m_playerData->getGender());
	msg.set_weaponid(pCurPlayer->GetWeaponId());
	msg.set_horseid(pCurPlayer->GetHorseId());

	FVector vec = GetActorLocation();
	FRotator rot = GetActorRotation();
	auto& P = *msg.mutable_p();
	P.set_x(vec.X * 1000000);
	P.set_y(vec.Y * 1000000);
	P.set_z(vec.Z * 1000000);

	auto& R = *msg.mutable_r();
	R.set_r(rot.Roll * 1000000);
	R.set_p(rot.Pitch * 1000000);
	R.set_y(rot.Yaw * 1000000);
	GameAppExtern::sendMsg(msg.msgtype(), msg);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::SendOutPlayerSight(NS::I64  iggId)
{
	MsgInteractionOutWorldC2S msg;
	GameAppExtern::sendMsg(msg.msgtype(), msg);
}

//****************************************************************************************
//
//****************************************************************************************
void AIPlayer::PlayWeaponSoundEffect(int nCombomIdx)
{
	std::string strSound = WeaponTypeCfg::getInstance().GetSoundPath(m_PlayerInfo.nProfessIdx, GetWeaponTpy(), nCombomIdx);
	if (strSound.empty()) {
		return;
	}
	FString setSoundPath = FString(strSound.c_str());
	UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, *(setSoundPath));
	if (!soundObj) {
		return;
	}
	UGameplayStatics::PlaySound2D(GetWorld(), Cast<USoundBase>(soundObj));
}

FTransform AIPlayer::GetWeaponTransByName(FString strSockName)
{
	FTransform oriTrans;
	if (strSockName.IsEmpty())
	{
		return oriTrans;
	}

	AWeapon* weapon = m_LeftAWeapon;
	if (!weapon) {
		weapon = m_RightAWeapon;
	}

	if (!weapon) {
		return oriTrans;
	}

	TArray<UActorComponent*> meshs = weapon->K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	if (meshs.Num() > 0)
	{
		for (int i = 0; i < meshs.Num(); i++)
		{
			auto mesh = Cast<USkeletalMeshComponent>(meshs[i]);
			if (mesh && mesh->GetSocketByName(*strSockName)) {
				auto trans = mesh->GetSocketTransform(*strSockName);
				return trans;
			}
		}
	}

	TArray<UActorComponent*> staMeshs = weapon->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
	if (staMeshs.Num() > 0)
	{
		for (int i = 0; i < staMeshs.Num(); i++)
		{
			auto mesh = Cast<UStaticMeshComponent>(staMeshs[i]);
			if (mesh && mesh->GetSocketByName(*strSockName)) {
				auto trans = mesh->GetSocketTransform(*strSockName);
				return trans;
			}
		}
	}

	return oriTrans;
}

void AIPlayer::onMsgInteractionS2C(const MsgInteractionS2C& msg)
{
	if (msg.player_iggid() != m_playerData->getIggId()) {
		return;
	}
	bool bSuc = true;
	switch (msg.action_type())
	{
	case ACTION_TYPE_SKILL_P_ATTACK:
		m_vecScrollPos = FVector(msg.data1()*1.f/ 10000, msg.data2()*1.f / 10000, msg.data3()*1.f / 10000);
		bSuc = PlayAttack();
		break;
	case ACTION_TYPE_SKILL_F_ATTACK:
		m_rotScroll = FVector(msg.data1()*1.f / 10000, msg.data2()*1.f / 10000, msg.data3()*1.f / 10000);
		bSuc = PlayAttack();
		break;
	case ACTION_TYPE_ATTACK:
		bSuc = PlayAttack();
		break;
	case ACTION_TYPE_WEAPON:
		SetWeaponId(msg.data1());
		m_vecScrollPos = FVector::ZeroVector;
		break;
	case ACTION_TYPE_HORSE:
		RideHorse(msg.data1());
		break;
	case ACTION_TYPE_SKILL_ATTACK:
		PlaySkillAttack(msg.data1());
		break;
	case ACTION_TYPE_HORSE_SKILL:
		PlayHorseAttack();
		break;
	case ACTION_TYPE_CONTINUE_S_START:
		PlayAttackContinue();
		break;
	case ACTION_TYPE_CONTINUE_S_END:
		StopAttackContinue();
		break;
	default:
		break;
	}
	if (GameAppExtern::IsServer() && bSuc) {
		GameAppExtern::sendMsg(msg.msgtype(), msg);
	}
}

void AIPlayer::onMsgMagicEffectAppenceS2C(const MsgMagicEffectAppenceS2C& msg)
{
	if (msg.player_iggid() != m_playerData->getIggId()) {
		return;
	}
	ARole* pTarget = nullptr;
	SkillMgr::getInstance().CreateSkill(msg.skillid(), this, pTarget,
		FVector(msg.p().x()*1.f/ 1000000, msg.p().y()*1.f / 1000000, msg.p().z()*1.f / 1000000), 
		FVector(msg.r().r()*1.f / 1000000, msg.r().p()*1.f / 1000000, msg.r().y()*1.f / 1000000));
}

void AIPlayer::onMsgPlayerInfoGS2C(const MsgPlayerInfoGS2C& msg)
{
	if (msg.iggid() != m_playerData->getIggId()) {
		return;
	}
	ResetAttr();
}
