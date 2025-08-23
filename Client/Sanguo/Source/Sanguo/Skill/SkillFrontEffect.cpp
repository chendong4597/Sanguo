// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFrontEffect.h"

#include "config/SkillTypeCfg.h"
#include "config/SkillFrontEffectCfg.h"

// Sets default values
ASkillFrontEffect::ASkillFrontEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkillFrontEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkillFrontEffect::Tick(float DeltaTime)
{
	if (m_AllTm > 0.f)
	{
		m_curTm += DeltaTime;
		if (m_curTm >= m_AllTm)
		{
			Destroy();
		}
	}
	Super::Tick(DeltaTime);
}


void ASkillFrontEffect::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	auto&& pSkillCfg = SkillFrontEffectCfg::getInstance().GetSkillFrontType(pCfg->n_frontSkill);
	if (!pSkillCfg || pSkillCfg->s_DemoPath.empty())
	{
		return;
	}
	m_curTm = 0.f;
	m_AllTm = pSkillCfg->f_tm;
}
