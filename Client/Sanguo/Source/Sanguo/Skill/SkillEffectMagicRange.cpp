// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectMagicRange.h"
#include "Kismet/KismetMathLibrary.h"

#include "config/SkillCfg.h"
#include "config/SkillTypeCfg.h"

void ASkillEffectMagicRange::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	m_allTm = 10.f;
	Super::BeginPlay();
}


void ASkillEffectMagicRange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_curTm += DeltaTime;
	if (m_curTm >= m_allTm)
	{
		Destroy();
		return;
	}
	if (m_InterTm > 0.00001f) {
		m_curInterTm += DeltaTime;
		if (m_curInterTm >= m_InterTm) {
			m_curInterTm = m_curInterTm - m_InterTm;
			StartDamage();
		}
	}
}


void ASkillEffectMagicRange::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	m_allTm = pCfg->f_maxTm;
	m_curTm = 0;
	m_InterTm = pCfg->f_interTm;
	m_nDamageRange1 = pCfg->n_DamageParam1;
	m_nDamageRange2 = pCfg->n_DamageParam2;
	m_nDamageRange3 = pCfg->n_DamageParam3;
	m_nDamageRange4 = pCfg->n_DamageParam4;
	m_nDamageType = pCfg->n_DamageTpy;
	m_nBPDamageType = pCfg->n_BpDamageTpy;
	FRotator rRot = GetAttackRole()->GetActorRotation();
	SetActorRotation(rRot);

	if (pCfg->n_ScrollHit == 0) {
		if (pCfg->n_AtkType == 0 && GetAttackRole()) {
			m_targetPos = GetAttackRole()->GetMagicBeginLocationByName(pCfg->s_sockName.c_str());
		}
		if (pCfg->n_AtkType == 1 && GetTargetRole()) {
			m_targetPos = GetTargetRole()->GetActorLocation();
		}
	}
	else if (pCfg->n_ScrollHit == 2) {
		if (pCfg->n_AtkType == 0 && GetAttackRole()) {
			m_targetPos = GetAttackRole()->GetActorLocation();
		}
	}
	else {
		m_targetPos = GetInitTargetPos();
	}


	auto&& pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetSkillId());
	if (!pSkillCfg) {
		if (GetParentSkillId() > 0) {
			pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetParentSkillId());
		}
	}

	if (pSkillCfg) {
		m_nDamageSkillId = pSkillCfg->m_id;
	}
	
	SetActorLocation(m_targetPos);
	RegisterOverlayEvents();
	StartDamage();
}

void ASkillEffectMagicRange::BPDamageIndex(int nIdx, int nBPDamgeRange)
{
	if (m_nDamageSkillId == 0)
	{
		return;
	}

	if (nBPDamgeRange == 0)
	{
		nBPDamgeRange = m_nDamageRange1;
	}

	if (SkillDamageTpy_Distance_Range == m_nBPDamageType) {
		if (nIdx == 1 && m_nDamageRange2 > 0 && GetAttackRole()
			&& GetAttackRole()->GetDamageCollision()) {
			GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamage(GetActorLocation(), GetActorForwardVector(),
				nBPDamgeRange, m_nDamageRange2, m_nDamageSkillId);
		}
	}

	if (SkillDamageTpy_Distance_Range == m_nBPDamageType) {
		if (nIdx == 2 && m_nDamageRange3 > 0 && GetAttackRole()
			&& GetAttackRole()->GetDamageCollision()) {
			GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamage(GetActorLocation(), GetActorForwardVector(), 
				nBPDamgeRange, m_nDamageRange3, m_nDamageSkillId);
		}
	}

	if (SkillDamageTpy_Distance_Range == m_nBPDamageType) {
		if (nIdx == 3 && m_nDamageRange4 > 0 && GetAttackRole()
			&& GetAttackRole()->GetDamageCollision()) {
			GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamage(GetActorLocation(), GetActorForwardVector(), 
				nBPDamgeRange, m_nDamageRange4, m_nDamageSkillId);
		}
	}
}

void ASkillEffectMagicRange::BPDamageDistanceRange(int nDis)
{
	if (m_nDamageSkillId == 0 || !GetAttackRole() || !GetAttackRole()->GetDamageCollision())
	{
		return;
	}
	GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamage(GetActorLocation(), GetActorForwardVector(),
		m_nDamageRange1, nDis, m_nDamageSkillId);
}

void ASkillEffectMagicRange::BPDamageDelay()
{
	if (GetAttackRole() && GetAttackRole()->GetDamageCollision() && m_nDamageSkillId > 0 && m_nDamageRange1 > 0)
	{
		GetAttackRole()->GetDamageCollision()->EffectCircleDamage(GetActorLocation(), m_nDamageRange1, m_nDamageSkillId);
	}
}

void ASkillEffectMagicRange::BPDamageRange2Delay()
{
	if (GetAttackRole() && GetAttackRole()->GetDamageCollision() && m_nDamageSkillId > 0 && m_nDamageRange2 > 0)
	{
		GetAttackRole()->GetDamageCollision()->EffectCircleDamage(GetActorLocation(), m_nDamageRange2, m_nDamageSkillId);
	}
}

void ASkillEffectMagicRange::BPDamageFiveTornado()
{
	if (!GetAttackRole() || !GetAttackRole()->GetDamageCollision() || m_nDamageSkillId <= 0 || m_nDamageRange1 <= 0) {
		return;
	}
	FVector ownerPos = GetActorLocation();
	GetAttackRole()->GetDamageCollision()->EffectCircleArrowDamage(ownerPos, m_nDamageRange1, m_vecHasAttacks, m_nDamageSkillId);
	FVector forVec = GetActorForwardVector();
	GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamageFit(ownerPos, forVec, m_nDamageRange1, m_nDamageRange2, m_vecHasAttacks, m_nDamageSkillId);
	forVec *= -1.f;
	GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamageFit(ownerPos, forVec, m_nDamageRange1, m_nDamageRange2, m_vecHasAttacks, m_nDamageSkillId);
	forVec = GetActorRightVector();
	GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamageFit(ownerPos, forVec, m_nDamageRange1, m_nDamageRange2, m_vecHasAttacks, m_nDamageSkillId);
	forVec *= -1.f;
	GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamageFit(ownerPos, forVec, m_nDamageRange1, m_nDamageRange2, m_vecHasAttacks, m_nDamageSkillId);
}

void ASkillEffectMagicRange::BPClearTargets()
{
	m_vecHasAttacks.clear();
}

int ASkillEffectMagicRange::GetDamageSkillId()
{
	return m_nDamageSkillId;
}

int ASkillEffectMagicRange::GetDamageRange()
{
	return m_nDamageRange1;
}

int ASkillEffectMagicRange::GetDamageRange2()
{
	return m_nDamageRange2;
}

ARole* ASkillEffectMagicRange::GetDamageAttacker()
{
	return GetAttackRole();
}

void ASkillEffectMagicRange::BPDamageCircle(int Dis)
{
	if (m_nDamageRange1 == 0 || m_nDamageSkillId == 0 || !GetAttackRole() || !GetAttackRole()->GetDamageCollision())
	{
		return;
	}
	GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamageFit(GetActorLocation(), GetActorForwardVector(), m_nDamageRange1, Dis, m_vecHasAttacks, m_nDamageSkillId);
}

void ASkillEffectMagicRange::BPDamageSector(int R)
{
	if (m_nDamageSkillId == 0 || !GetAttackRole() || !GetAttackRole()->GetDamageCollision())
	{
		return;
	}
	GetAttackRole()->GetDamageCollision()->SectorForwardDamage(GetActorLocation(), GetActorForwardVector(),
		m_nDamageRange1, m_nDamageRange2, R, m_vecHasAttacks, m_nDamageSkillId);
}

void ASkillEffectMagicRange::BeginCollision(AActor* OtherActor)
{
	Super::BeginCollision(OtherActor);
}


void ASkillEffectMagicRange::EndCollision(AActor* OtherActor)
{
	Super::EndCollision(OtherActor);
}
