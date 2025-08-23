// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralAnimInstance.h"

void UGeneralAnimInstance::NativeUpdateAnimation(float dt)
{
	if (m_pPlayer == NULL) {
		APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
		AGenerals* pPlayer = Cast<AGenerals>(Owner);
		if (pPlayer)
		{
			m_pPlayer = pPlayer;
		}
	}
	if (!m_pPlayer)
	{
		return;
	}

	m_fSpeed = m_pPlayer->GetVelocity().Size()*0.5f; // 获得移动速度
	if (m_pPlayer->GetWeaponTpy() == Weapon_Tpy_Knife) {
		m_iWeaponTpy = 2;
	}
	else if (m_pPlayer->GetWeaponTpy() == Weapon_Tpy_Bow) {
		m_iWeaponTpy = 1;
	}
	else if (m_pPlayer->GetWeaponTpy() == Weapon_Tpy_Blade) {
		m_iWeaponTpy = 3;
	}
	else if (m_pPlayer->GetWeaponTpy() == Weapon_Tpy_Staff) {
		m_iWeaponTpy = 4;
	}
	else {
		m_iWeaponTpy = 0;
		m_bDefenceing = m_pPlayer->IsDefenceing();
	}
	m_bDead = m_pPlayer->IsDeath();
	m_bDizz = m_pPlayer->IsDizziess();
	m_bFreeze = m_pPlayer->IsFreeze();
	m_fFighting = m_pPlayer->GetFocusRole() != nullptr ? 1.0f : 0.f;
	UAnimInstance::NativeUpdateAnimation(dt);
}

void UGeneralAnimInstance::NativeInitializeAnimation()
{
	m_iWeaponTpy = 0;
	m_fSpeed = 0.f;
	m_bDead = false;
	m_bDizz = false;
	m_bFreeze = false;
	APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
	AGenerals* pPlayer = Cast<AGenerals>(Owner);
	if (pPlayer) {
		m_pPlayer = pPlayer;
	}
	UAnimInstance::NativeInitializeAnimation();
}


void UGeneralAnimInstance::AnimNotify_NormalEffect(UAnimNotify * Notify)
{
	if (!m_pPlayer) {
		return;
	}
	m_pPlayer->AttackEffect();
}

void UGeneralAnimInstance::AnimNotify_CavAtkEnd(UAnimNotify * Notify)
{
	if (!m_pPlayer) {
		return;
	}
	m_pPlayer->ResortAttackTargets();
}