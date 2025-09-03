// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenceAnimInstance.h"

void UDefenceAnimInstance::NativeUpdateAnimation(float dt)
{
	if (!m_pDefence)
	{
		APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
		ADefenceBuilding* Player = Cast<ADefenceBuilding>(Owner);
		if (Player)
		{
			m_pDefence = Player;
		}
	}
	
	if (m_pDefence)
	{
		m_bDead = m_pDefence->IsDeath();
	}
}

void UDefenceAnimInstance::NativeInitializeAnimation()
{
	m_bDead = false;
	APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
	ADefenceBuilding* pDefence = Cast<ADefenceBuilding>(Owner);
	if (pDefence != nullptr)
	{
		m_pDefence = pDefence;
	}
}

void UDefenceAnimInstance::AnimNotify_MagicEffect(UAnimNotify * Notify)
{
	if (m_pDefence)
	{
		m_pDefence->AttackEffect();
	}
}

