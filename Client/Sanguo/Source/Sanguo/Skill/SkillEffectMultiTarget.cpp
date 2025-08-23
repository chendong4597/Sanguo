// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectMultiTarget.h"
#include "Kismet/KismetMathLibrary.h"

#include "config/SkillTypeCfg.h"

void ASkillEffectMultiTarget::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	m_allTm = 10.f;
	Super::BeginPlay();
}


void ASkillEffectMultiTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_curTm += DeltaTime;
	if (m_curTm >= m_allTm)
	{
		Destroy();
	}
}


void ASkillEffectMultiTarget::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	m_allTm = pCfg->f_maxTm;
	m_curTm = 0;
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

	SetActorLocation(m_targetPos);
	RegisterOverlayEvents();
	SingleDamage();
}


void ASkillEffectMultiTarget::BPDamageDelay()
{
	if (GetAttackRole() && GetAttackRole()->GetDamageCollision() && m_nDamageSkillId > 0 && m_nDamageRange1 > 0)
	{
		GetAttackRole()->GetDamageCollision()->EffectCircleDamage(GetActorLocation(), m_nDamageRange1, m_nDamageSkillId);
	}
}

void ASkillEffectMultiTarget::SingleDamage()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg)
	{
		return;
	}

	m_nDamageRange1 = pCfg->n_DamageParam1;

	auto&& pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetSkillId());
	if (!pSkillCfg) {
		if (GetParentSkillId() > 0) {
			pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetParentSkillId());
		}
	}

	if (pSkillCfg) {
		m_nDamageSkillId = pSkillCfg->m_id;
	}

	if (m_nDamageSkillId == 0) {
		return;
	}

	if (pCfg->n_DamageTpy == SkillDamageTpy_Single && GetAttackRole() && GetTargetRole()
		&& GetAttackRole()->GetDamageCollision()) {
		GetAttackRole()->GetDamageCollision()->SingleDamage(GetTargetRole()->GetObjectId(), m_nDamageSkillId);
	}
}

void ASkillEffectMultiTarget::BeginCollision(AActor* OtherActor)
{
	Super::BeginCollision(OtherActor);
}


void ASkillEffectMultiTarget::EndCollision(AActor* OtherActor)
{
	Super::EndCollision(OtherActor);
}

