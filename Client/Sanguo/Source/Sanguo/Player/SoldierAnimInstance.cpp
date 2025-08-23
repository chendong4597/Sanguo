// Fill out your copyright notice in the Description page of Project Settings.

#include "SoldierAnimInstance.h"
#include "Soldier.h"
#include "Engine/Engine.h"
#include "Hero.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"


void USoldierAnimInstance::NativeUpdateAnimation(float dt)
{
	APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
	ASoldier* pSoldier = Cast<ASoldier>(Owner);
	if (pSoldier != nullptr)
	{
		m_fSpeed = pSoldier->GetVelocity().Size()*0.5f; // 获得移动速度
		m_fDirection = CalculateDirection(pSoldier->GetVelocity(), pSoldier->GetBaseAimRotation()); // 通过速度和旋转计算方向 (-180~180)
		m_bFocus = pSoldier->GetFocusRole() != nullptr;
		m_bDead = pSoldier->IsDeath();
		if (pSoldier->GetWeaponTpy() == Weapon_Tpy_Knife) {
			m_iWeaponTpy = 2;
		}
		else if (pSoldier->GetWeaponTpy() == Weapon_Tpy_Bow) {
			m_iWeaponTpy = 1;
		}
		else {
			m_iWeaponTpy = 0;
			m_bDefenceing = pSoldier->IsDefenceing();
		}
	}
}

void USoldierAnimInstance::NativeInitializeAnimation()
{
	m_bFocus = false;
	m_bDead = false;
}


void USoldierAnimInstance::AnimNotify_Attack(UAnimNotify * Notify)
{
	UE_LOG(LogTemp, Warning, TEXT("USoldierAnimInstance AnimNotify_Attack"));
	APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
	ASoldier* pSoldier = Cast<ASoldier>(Owner);
	if (pSoldier != nullptr)
	{
		pSoldier->AttackEffect();
	}
}


void USoldierAnimInstance::AnimNotify_CavAtkEnd(UAnimNotify * Notify)
{
	APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
	ASoldier* pSoldier = Cast<ASoldier>(Owner);
	if (!pSoldier) {
		return;
	}
	pSoldier->ResortAttackTargets();
}
