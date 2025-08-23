// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAnim.h"
#include "Monster.h"

void UMonsterAnim::NativeUpdateAnimation(float dt)
{
	APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
	AMonster* Player = Cast<AMonster>(Owner);
	if (Player != nullptr)
	{
		Speed = Owner->GetVelocity().Size(); // 获得移动速度
		Direction = CalculateDirection(Owner->GetVelocity(), Owner->GetBaseAimRotation()); // 通过速度和旋转计算方向 (-180~180)
		bFocusPlayer = Player->GetFocusRole() != nullptr;
		m_bDead = Player->IsDeath();
		fFighting = bFocusPlayer ? 1.0f : 0.f;
		//bFocusPlayer = Owner->focu
	}
}

void UMonsterAnim::NativeInitializeAnimation()
{
	bFocusPlayer = false;
	m_bDead = false;
	fFighting = 1.0f;
}


void UMonsterAnim::AnimNotify_Attack(UAnimNotify * Notify)
{
	UE_LOG(LogTemp, Warning, TEXT("AnimNotify_Attack"));

	AMonster* pMonster = Cast<AMonster>(TryGetPawnOwner());
	if (pMonster != nullptr)
	{
		pMonster->NormalAttack();
	}
}

void UMonsterAnim::AnimNotify_Magic(UAnimNotify * Notify)
{
	AMonster* pMonster = Cast<AMonster>(TryGetPawnOwner());
	if (pMonster != nullptr)
	{
		pMonster->MagicAttack();
	}
}

void UMonsterAnim::AnimNotify_MagicEnd(UAnimNotify * Notify)
{
	AMonster* pMonster = Cast<AMonster>(TryGetPawnOwner());
	if (pMonster != nullptr)
	{
		pMonster->MagicAttackEnd();
	}
}

void UMonsterAnim::AnimNotify_DeathEnd(UAnimNotify * Notify)
{
	UE_LOG(LogTemp, Warning, TEXT("AnimNotify_DeathEnd"));
	APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
	AMonster* pMonster = Cast<AMonster>(Owner);
	if (pMonster != nullptr)
	{
		pMonster->DeathEnd();
	}
}
