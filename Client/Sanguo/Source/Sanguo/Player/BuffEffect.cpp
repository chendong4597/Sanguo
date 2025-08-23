// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffEffect.h"
#include "Kismet/KismetMathLibrary.h"

#include "config/BuffTypeCfg.h"

// Sets default values
ABuffEffect::ABuffEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABuffEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuffEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_fCurTm += DeltaTime;
	if (m_fCurTm >= m_fAllTm)
	{
		BuffDestroy();
		return;
	}
	if (m_nFlllowRot == 0)
	{
		this->SetActorRotation(FRotator::ZeroRotator);
	} else if(m_nFlllowRot == 1 && m_pOwnerRole){
		this->SetActorRotation(m_pOwnerRole->GetActorRotation());
	}
	else if (m_nFlllowRot == 2 && m_pOwnerRole) {
		auto cameLoc = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		auto toRota = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), cameLoc);
		this->SetActorRotation(toRota);
	}
}

void ABuffEffect::StartBuff(ARole* pOwner, int nBuffId)
{
	if (!pOwner)
	{
		return;
	}
	m_pOwnerRole = pOwner;
	m_nBuffId = nBuffId;
	auto&& pCfg = BuffTypeCfg::getInstance().GetBuffType(m_nBuffId);
	if (!pCfg)
	{
		return;
	}
	m_fAllTm = pCfg->f_tm;
	m_nAdditionNum = 1;
	m_nFlllowRot = pCfg->n_FollowRat;
}

void ABuffEffect::BuffDestroy()
{
	//if (m_pOwnerRole)
	//{
	//	m_pOwnerRole->RemoveBuff(m_nBuffId);
	//}

	m_pOwnerRole = nullptr;
	Destroy();
}

void ABuffEffect::RestStatus()
{
	auto&& pCfg = BuffTypeCfg::getInstance().GetBuffType(m_nBuffId);
	if (!pCfg)
	{
		return;
	}
	m_fCurTm = 0.f;
	m_fAllTm = pCfg->f_tm;
	m_nAdditionNum = m_nAdditionNum >= pCfg->n_nOverlay ? m_nAdditionNum : m_nAdditionNum++;
}

