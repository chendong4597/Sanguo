// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectGas.h"
#include "Kismet/KismetMathLibrary.h"

#include "data/role/Player.h"
#include "../Player/Hero.h"
#include "config/SkillTypeCfg.h"

void ASkillEffectGas::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	Super::BeginPlay();
}

void ASkillEffectGas::RegisterOverlayEvents()
{
	UI_REGISTER_MYEVENT(SkillStopContinueEvent, &ASkillEffectGas::onSkillStopContinueEvent);
	//REGISTER_EVENT(&HandlerUIEvents::getInstance(), SkillStopContinueEvent, &ASkillEffectGas::onSkillStopContinueEvent);
	Super::RegisterOverlayEvents();
}

void ASkillEffectGas::onSkillStopContinueEvent(const SkillStopContinueEvent& evt)
{
	if (GetAttackRole() && evt.m_nId == GetAttackRole()->GetObjectId())
	{
		SkillDestroy();
	}
}


void ASkillEffectGas::Tick(float DeltaTime)
{
	if (!GetAttackRole() || GetAttackRole()->IsDeath()) {
		SkillDestroy();
		return;
	}

	if (!m_strSockName.empty()) {
		auto&& pPlayer = Cast<AIPlayer>(GetAttackRole());
		if (pPlayer) {
			auto pos = GetAttackRole()->GetMagicBeginLocationByName(m_strSockName.c_str());
			SetActorLocation(pos);
			SetActorRotation(GetAttackRole()->GetActorRotation());
		}
		else {
			FTransform trans = GetAttackRole()->GetRoleTransByName(m_strSockName.c_str());
			if (trans.GetLocation() != FVector::ZeroVector)
			{
				SetActorLocation(trans.GetLocation());
				SetActorRotation(trans.GetRotation());
			} else {
				auto pos = GetAttackRole()->GetMagicBeginLocationByName(m_strSockName.c_str());
				SetActorLocation(pos);
				SetActorRotation(GetAttackRole()->GetActorRotation());
			}
		}
	}

	auto&& pPlayer = Cast<AIPlayer>(GetAttackRole());
	if (pPlayer && pPlayer->GetContinueIdx() == 0) {
		SkillDestroy();
		return;
	}

	if (GetAttackRole() == AIPlayerMgr::getInstance().GetHero() && m_InterTm > 0.0001f && GetAttackRole()->GetSpeedRate() > 0.001f) {
		m_curInterTm += (DeltaTime * GetAttackRole()->GetSpeedRate());
		if (m_curInterTm >= m_InterTm)
		{
			AHero* pHero = Cast<AHero>(GetAttackRole());
			auto&& self = PlayerMgr::getInstance().getSelf();
			if (pHero && self) {
				auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
				if (pCfg && !pHero->SkillConsume(self->CalcConsumeHp(pCfg->f_Hp), self->CalcConsumeHp(pCfg->f_Mana), pCfg->n_Sten, pCfg->n_Anger)) {
					StopContinueAttackEvent evt;
					UI_DISPATCH_MYEVENT(StopContinueAttackEvent, evt);
				}
				PlaySkillSound();
				StartDamage();
			}
			m_curInterTm -= m_InterTm;
		}
	}
	else if (m_InterTm > 0.0001f) {
		m_curInterTm += (DeltaTime * GetAttackRole()->GetSpeedRate());
		if (m_curInterTm >= m_InterTm) {
			PlaySkillSound();
			StartDamage();
			m_curInterTm -= m_InterTm;
		}
	}

	if (GetAttackRole() != AIPlayerMgr::getInstance().GetHero() && m_allTm > 0.00001f)
	{
		m_Tm += DeltaTime;
		if (m_Tm > m_allTm)
		{
			SkillDestroy();
		}
	}

	Super::Tick(DeltaTime);
}


void ASkillEffectGas::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	m_startPos = GetAttackRole()->GetMagicBeginLocationByName(pCfg->s_sockName.c_str());
	m_strSockName = pCfg->s_sockName;
	m_targetPos = m_startPos;
	m_fLength = pCfg->n_length;
	m_Speed = pCfg->n_speed;
	m_allTm = pCfg->f_maxTm;
	m_InterTm = pCfg->f_continueInterTm;
	m_curInterTm = m_InterTm - 0.1f;
	FVector vec = GetAttackRole()->GetActorForwardVector();
	m_targetPos.X += pCfg->n_length*vec.X;
	m_targetPos.Y += pCfg->n_length*vec.Y;
	SetActorRotation(GetAttackRole()->GetActorRotation());
	RegisterOverlayEvents();
	StartDamage();
}

void ASkillEffectGas::BeginCollision(AActor* OtherActor)
{
	Super::BeginCollision(OtherActor);
}


void ASkillEffectGas::EndCollision(AActor* OtherActor)
{
	Super::EndCollision(OtherActor);
}

