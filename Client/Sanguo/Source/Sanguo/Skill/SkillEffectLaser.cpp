// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectLaser.h"
#include "Kismet/KismetMathLibrary.h"

#include "Player/IPlayer.h"
#include "Player/Hero.h"

void ASkillEffectLaser::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	Super::BeginPlay();
}

void ASkillEffectLaser::RegisterOverlayEvents()
{
	UI_REGISTER_MYEVENT(SkillStopContinueEvent, &ASkillEffectLaser::onSkillStopContinueEvent);
	Super::RegisterOverlayEvents();
}

void ASkillEffectLaser::onSkillStopContinueEvent(const SkillStopContinueEvent& evt)
{
	if (GetAttackRole() && evt.m_nId == GetAttackRole()->GetObjectId())
	{
		SkillDestroy();
		//UNREGISTER_EVENT(&HandlerUIEvents::getInstance() , SkillStopContinueEvent);
	}
}

void ASkillEffectLaser::DamageSingle(AActor* pActor)
{
	ARole* pRole = Cast<ARole>(pActor);
	if (!pRole)
	{
		return;
	}
	if (GetAttackRole() && GetAttackRole()->GetDamageCollision() && m_nDamageSkillId > 0)
	{
		GetAttackRole()->GetDamageCollision()->SingleDamage(pRole->GetObjectId(), m_nDamageSkillId);
	}
}


void ASkillEffectLaser::Tick(float DeltaTime)
{
	if (GetAttackRole() != nullptr && !m_strSockName.empty()) {
		auto trans = GetAttackRole()->GetRoleTransByName(m_strSockName.c_str());
		auto pos = GetAttackRole()->GetMagicBeginLocationByName(m_strSockName.c_str());
		SetActorLocation(pos);
		SetActorRotation(GetAttackRole()->GetActorRotation());
		auto&& pPlayer = Cast<AIPlayer>(GetAttackRole());
		if (pPlayer && pPlayer->GetContinueIdx() == 0) {
			SkillDestroy();
			return;
		}

		if (GetAttackRole()->IsDeath())
		{
			SkillDestroy();
			return;
		}

		if (m_InterTm > 0.0001f && GetAttackRole()->GetSpeedRate() > 0.001f) {
			m_curInterTm += (DeltaTime * GetAttackRole()->GetSpeedRate());
			if (m_curInterTm >= m_InterTm)
			{
				if (GetAttackRole() == AIPlayerMgr::getInstance().GetHero())
				{
					AHero* pHero = Cast<AHero>(GetAttackRole());
					auto&& self = PlayerMgr::getInstance().getSelf();
					if (pHero && self) {
						auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
						if (!pHero->SkillConsume(self->CalcConsumeHp(pCfg->f_Hp), self->CalcConsumeHp(pCfg->f_Mana), pCfg->n_Sten, pCfg->n_Anger)) {
							StopContinueAttackEvent evt;
							UI_DISPATCH_MYEVENT(StopContinueAttackEvent, evt);
							return;
						}
					}
				}

				if (m_nDamageTpy == SkillDamageTpy_Single)
				{
					if (!GetTargetRole() || GetTargetRole()->IsDeath()
						|| FVector::Distance(GetAttackRole()->GetActorLocation(), GetTargetRole()->GetActorLocation()) > m_fCastDistance) {
						//FinishDestroy();
						StopContinueAttackEvent evt;
						UI_DISPATCH_MYEVENT(StopContinueAttackEvent, evt);
						return;
					}
					else {
						if (GetAttackRole() && GetAttackRole()->GetDamageCollision() && m_nDamageSkillId > 0)
						{
							GetAttackRole()->GetDamageCollision()->SingleDamage(GetTargetRole()->GetObjectId(), m_nDamageSkillId);
						}
					}
				} else {
					StartDamage();
				}
				PlaySkillSound();
				
				m_curInterTm -= m_InterTm;
			}
		}
	}
	else {
		SkillDestroy();
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


void ASkillEffectLaser::StartSkill()
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
	m_nDamageTpy = pCfg->n_DamageTpy;
	m_curInterTm = m_InterTm - 0.1f;
	m_fCastDistance = pCfg->n_nCastDis;
	FVector vec = GetAttackRole()->GetActorForwardVector();
	m_targetPos.X += pCfg->n_length*vec.X;
	m_targetPos.Y += pCfg->n_length*vec.Y;

	auto&& pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetSkillId());
	if (!pSkillCfg) {
		if (GetParentSkillId() > 0) {
			pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetParentSkillId());
		}
	}

	if (pSkillCfg) {
		m_nDamageSkillId = pSkillCfg->m_id;
	}

	SetActorRotation(GetAttackRole()->GetActorRotation());
	RegisterOverlayEvents();
	StartDamage();
}

void ASkillEffectLaser::BeginCollision(AActor* OtherActor)
{
	Super::BeginCollision(OtherActor);
}


void ASkillEffectLaser::EndCollision(AActor* OtherActor)
{
	Super::EndCollision(OtherActor);
}
