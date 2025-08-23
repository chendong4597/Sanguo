// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectTrigger.h"
#include "Player/Hero.h"

void ASkillEffectTrigger::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	Super::BeginPlay();
}


void ASkillEffectTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_curTm >= m_interTm && !m_nHasAdd) {
		if (GetAttackRole() && Cast<AHero>(GetAttackRole()))
		{
			//BuffEvent evt;
			//evt.m_nBuffId = m_nBuffId;
			//UI_DISPATCH_MYEVENT(BuffEvent, evt);
			//m_nHasAdd = true;
		}
	}

	m_curTm += DeltaTime;
	if (m_curTm > m_allTm)
	{
		SkillDestroy();
		return;
	}
}

void ASkillEffectTrigger::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	m_interTm = pCfg->f_interTm;
	m_allTm = pCfg->f_maxTm;
	m_nBuffId = pCfg->n_buffId;
}
