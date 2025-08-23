// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectEmitter.h"
#include "Kismet/KismetMathLibrary.h"

#include "Player/IPlayer.h"
#include "Player/Hero.h"
#include "config/SkillTypeCfg.h"

void ASkillEffectEmitter::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	m_allTm = 10.f;
	Super::BeginPlay();
}

void ASkillEffectEmitter::RegisterOverlayEvents()
{
	UI_REGISTER_MYEVENT(SkillStopContinueEvent, &ASkillEffectEmitter::onSkillStopContinueEvent);
	Super::RegisterOverlayEvents();
}

void ASkillEffectEmitter::onSkillStopContinueEvent(const SkillStopContinueEvent& evt)
{
	if (evt.m_nId == GetAttackRole()->GetObjectId() && m_nTraceTpy == SkillDirectTpy_AttachAtker
		&& m_allTm >= 999)
	{
		SkillDestroy();
	}
}


void ASkillEffectEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_curTm += DeltaTime;
	m_interCurTm += DeltaTime;
	if (m_interCurTm >= m_interTm)
	{
		m_interCurTm = 0;
		Attack();
		NotifyAttack();
		if (m_bContinueAtk) {
			AHero* pHero = Cast<AHero>(GetAttackRole());
			auto&& self = PlayerMgr::getInstance().getSelf();
			if (pHero && self) {
				auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
				if (!pHero->SkillConsume(self->CalcConsumeHp(pCfg->f_Hp), self->CalcConsumeHp(pCfg->f_Mana), pCfg->n_Sten, pCfg->n_Anger)) {
					StopContinueAttackEvent evt;
					UI_DISPATCH_MYEVENT(StopContinueAttackEvent, evt);
				}
				PlaySkillSound();
			}
		}
	}
	if (GetAttackRole() && m_nTraceTpy == SkillDirectTpy_AttachAtker) {
		SetActorLocation(GetAttackRole()->GetActorLocation());
	}
	//if (GetTargetRole() && GetAttackRole())
	//{
	//	auto tartPos = GetTargetRole()->GetMagicBeginLocationByName("S_Hit");
	//	auto rRot = UKismetMathLibrary::FindLookAtRotation(GetAttackRole()->GetActorLocation(), tartPos);
	//	rRot.Pitch = GetAttackRole()->GetActorRotation().Pitch;
	//	rRot.Yaw = GetAttackRole()->GetActorRotation().Yaw;
	//	SetActorRotation(rRot);
	//}
	if (m_curTm >= m_allTm)
	{
		Destroy();
		return;
	}
}

void ASkillEffectEmitter::BPDamageInteral()
{
	if (m_damageSkillId && m_nDamageRange > 0 && GetAttackRole()
		&& GetAttackRole()->GetDamageCollision()) {
		FVector pos = GetActorLocation();
		pos.Z = 0;
		GetAttackRole()->GetDamageCollision()->EffectCircleDamage(pos, m_nDamageRange, m_damageSkillId);
	}
}

void ASkillEffectEmitter::BPDamageDisRange(float nDis, int nRange)
{
	if (m_damageSkillId && GetAttackRole() && GetAttackRole()->GetDamageCollision()) {
		FVector pos = GetActorLocation();
		pos.Z = 0;
		FVector vecFor = GetActorForwardVector();
		GetAttackRole()->GetDamageCollision()->EffectCircleDistanceDamage(pos, vecFor, nRange, nDis, m_damageSkillId);
	}
}


void ASkillEffectEmitter::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	
	m_allTm = pCfg->f_maxTm;
	m_curTm = 0;
	m_interTm = pCfg->f_interTm;
	m_interCurTm = 0;
	m_nTraceTpy = pCfg->n_trace;
	m_bContinueAtk = pCfg->n_AniTpy == 1;
	m_nDamageRange = pCfg->n_DamageParam1;

	auto&& pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetSkillId());
	if (!pSkillCfg) {
		if (GetParentSkillId() > 0) {
			pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetParentSkillId());
		}
	}

	if (pSkillCfg) {
		m_damageSkillId = pSkillCfg->m_id;
	}

	if (GetTargetRole() && GetAttackRole())
	{
		auto tartPos = GetTargetRole()->GetMagicBeginLocationByName("S_Hit");
		auto rRot = UKismetMathLibrary::FindLookAtRotation(GetAttackRole()->GetActorLocation(), tartPos);
		rRot.Pitch = GetAttackRole()->GetActorRotation().Pitch;
		rRot.Yaw = GetAttackRole()->GetActorRotation().Yaw;
		SetActorRotation(rRot);
	}
	if (GetInitTargetPos() != FVector::ZeroVector) {
		SetActorLocation(GetInitTargetPos());
	}
	
	RegisterOverlayEvents();
}

void ASkillEffectEmitter::BeginCollision(AActor* OtherActor)
{
	Super::BeginCollision(OtherActor);
}


void ASkillEffectEmitter::EndCollision(AActor* OtherActor)
{
	Super::EndCollision(OtherActor);
}


void ASkillEffectEmitter::NotifyAttack()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->n_nxtSkill == 0 || m_nNxtSkilNum >= pCfg->m_nNxtMaxNum)
	{
		return;
	}
	m_nNxtSkilNum++;
	int nSkillId = pCfg->n_nxtSkill;
	if (pCfg->m_vecNxtSkills.size() >= 2)
	{
		if (m_nSkillIdx >= pCfg->m_vecNxtSkills.size() - 1)
		{
			m_nSkillIdx = 0;
		}
		nSkillId = pCfg->m_vecNxtSkills[m_nSkillIdx];
	}
	auto&& pNxtCfg = SkillTypeCfg::getInstance().GetSkillType(nSkillId);
	if ((pNxtCfg->n_trace == SkillDirectTpy_TraceTarget && GetTargetRole()) || pNxtCfg->n_trace != SkillDirectTpy_TraceTarget)
	{
		AHero* pHero = Cast<AHero>(GetAttackRole());
		if (pHero) {
			SkillMgr::getInstance().CreateSubSkill(nSkillId, GetAttackRole(), GetTargetRole(), this,
				GetInitTargetPos(), GetInitRotaArrowPos(), m_nSkillIdx);
		}
		else if(GetAttackRole() && GetTargetRole()) {
			SkillMgr::getInstance().CreateSubSkill(nSkillId, GetAttackRole(), GetTargetRole(), this,
				GetTargetRole()->GetActorLocation(), GetAttackRole()->GetActorForwardVector(), m_nSkillIdx);
		}
		
	}
	m_nSkillIdx++;
}

