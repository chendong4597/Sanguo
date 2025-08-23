// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponEffect.h"

// Sets default values
AWeaponEffect::AWeaponEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponEffect::BeginPlay()
{
	Super::BeginPlay();
	m_fTmDie = 3.0f;
	m_fCurTm = 0.f;
}

// Called every frame
void AWeaponEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_fCurTm += DeltaTime;
	if (m_fCurTm >= m_fTmDie)
	{
		Destroy();
	}
}

void AWeaponEffect::AddFloatEffParams(FString strName, float fValue)
{
	FStuWeaponEffInfo  info;
	info.strParam = strName;
	info.emTpy = WEAPONEFFTPY::WEAPONEFFTPY_F;
	info.fValue = fValue;
	m_arrayEffParms.Add(info);
}

//****************************************************************************************
//
//****************************************************************************************
void AWeaponEffect::AddVectorEffParams(FString strName, FVector vecParams)
{
	FStuWeaponEffInfo  info;
	info.strParam = strName;
	info.emTpy = WEAPONEFFTPY::WEAPONEFFTPY_V;
	info.vecParams = vecParams;
	m_arrayEffParms.Add(info);
}

//****************************************************************************************
//
//****************************************************************************************
void AWeaponEffect::ClearVectorEffParams()
{
	m_arrayEffParms.RemoveAll([this](const FStuWeaponEffInfo&) {return true;});
}

//****************************************************************************************
//
//****************************************************************************************
void AWeaponEffect::DieDelay(float tm)
{
	m_fTmDie = tm;
}

