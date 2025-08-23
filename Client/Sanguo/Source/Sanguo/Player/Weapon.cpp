// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Monster.h"

#include "config/WeaponTypeCfg.h"
#include "config/WeaponInfoTypeCfg.h"
#include "../ReichGameInstance.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//****************************************************************************************
//
//****************************************************************************************
void AWeapon::RegisterOverlayEvents()
{
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(UBoxComponent::StaticClass(), TEXT("HitBox"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	auto StaticMesh = Cast<UBoxComponent>(meshs[0]);
	if (!StaticMesh)
	{
		return;
	}
	FScriptDelegate OverlayBegin;
	OverlayBegin.BindUFunction(this, "OnOverlayBegin");
	StaticMesh->OnComponentBeginOverlap.Add(OverlayBegin);

	FScriptDelegate OverlayEnd;
	OverlayEnd.BindUFunction(this, "OnOverlayEnd");
	StaticMesh->OnComponentEndOverlap.Add(OverlayEnd);
}

//****************************************************************************************
//
//****************************************************************************************
void AWeapon::SetWeaponId(int nId)
{
	m_nWeaponId = nId;
	auto&& pWeapon = WeaponInfoTypeCfg::getInstance().GetWeaponById(m_nWeaponId);
	if (pWeapon == nullptr)
	{
		return;
	}
	auto skinMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
		TEXT("Material'/Game/Weapon/Weapon_M/Com_Weapon_M.Com_Weapon_M'")));
	if (!skinMaterial) {
		UE_LOG(LogOutputDevice, Log, TEXT("AWeapon::SetWeaponId false"));
		return;
	}

	std::string strDifuse = WeaponInfoTypeCfg::getInstance().GetWeaponTexture(m_nWeaponId);
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

	//Âþ·´ÉäÌùÍ¼
	auto tmpTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(strText)));
	if (!tmpTexture)
	{
		UE_LOG(LogOutputDevice, Log, TEXT("AWeapon::SetWeaponId CreateDynamicMaterialInstance tmpTexture false"));
		return;
	}
	dynaMaterial->SetTextureParameterValue(FName("Defuse"), tmpTexture);

	TArray<UActorComponent*> meshs = K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	for (int i = 0; i < meshs.Num(); i++)
	{
		auto mesh = Cast<USkeletalMeshComponent>(meshs[i]);
		if (!mesh)
		{
			continue;
		}
		mesh->SetMaterial(0, dynaMaterial);
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AWeapon::OnOverlayBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!m_ownerRole)return;
	//ARole* monster = Cast<ARole>(OtherActor);
	//if (!monster)
	//{
	//	return;
	//}

	//if (monster != m_ownerRole && m_ownerRole->IsCanAttackTarget(monster))
	//{
	//	if (m_arrayRoles.Find(monster) == -1)
	//	{
	//		m_arrayRoles.Add(monster);
	//	}
	//	if (m_bHurting && m_hurtRoles.Find(monster) == -1)
	//	{
	//		m_hurtRoles.Add(monster);
	//	}
	//}
}

//****************************************************************************************
//
//****************************************************************************************
void AWeapon::OnOverlayEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!m_ownerRole)return;
	//ARole* monster = Cast<ARole>(OtherActor);
	//if (!monster)
	//{
	//	return;
	//}
	//int nIdx = m_arrayRoles.Find(monster);
	//if (m_arrayRoles.Find(monster) != -1)
	//{
	//	m_arrayRoles.RemoveAt(nIdx);
	//}
}

//****************************************************************************************
//
//****************************************************************************************
void AWeapon::StartPreHurt()
{
	m_bHurting = true;
	m_hurtRoles = m_arrayRoles;
}

void AWeapon::EndHurt()
{
	//m_bHurting = false;
	//std::vector<int> vecTargets;
	//for (int i = 0 ; i < m_hurtRoles.Num() ; i++)
	//{
	//	ARole* aRole = m_hurtRoles[i];
	//	if (!aRole)
	//	{
	//		continue;
	//	}
	//	AMonster* monster = Cast<AMonster>(aRole);
	//	if (monster && monster->m_pMonsterData) {
	//		vecTargets.push_back(monster->m_pMonsterData->getId());
	//	}
	//}
	//m_hurtRoles.Empty();

	//if (!vecTargets.empty()) {
	//	GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
	//}
}

//****************************************************************************************
//
//****************************************************************************************
void AWeapon::SetOwnerRole(ARole* aRole)
{
	m_ownerRole = aRole;
}

//****************************************************************************************
//
//****************************************************************************************
void AWeapon::SetMeshMaterialScalerParamer(FName name, float fValue)
{
	TArray<UActorComponent*> meshs = K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	for (int i = 0; i < meshs.Num(); i++)
	{
		auto mesh = Cast<USkeletalMeshComponent>(meshs[i]);
		if (!mesh)
		{
			continue;
		}
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

