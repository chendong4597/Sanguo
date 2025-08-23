// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectCannonball.h"
#include "Kismet/KismetMathLibrary.h"

#include "config/SkillTypeCfg.h"

void ASkillEffectCannonball::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FOnTimelineVectorStatic TimelineCallBack;
	TimelineCallBack.BindUFunction(this, TEXT("UpdateEffectLocation"));
	m_timelineBall.AddInterpVector(m_CurveVector, TimelineCallBack);

	FOnTimelineEventStatic finishEvt;
	finishEvt.BindUFunction(this, TEXT("FinishEffectTimeline"));
	m_timelineBall.SetTimelineFinishedFunc(finishEvt);
	m_timelineBall.Play();
	Super::BeginPlay();
}


void ASkillEffectCannonball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!m_bStartTmline)
	{
		return;
	}
	m_timelineBall.TickTimeline(DeltaTime / m_allTm);
}


void ASkillEffectCannonball::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	m_speed = pCfg->n_speed;
	m_bHitDeath = pCfg->n_hitdead == 1 ? true : false;
	FRotator rRot;
	m_startPos = GetAttackRole()->GetMagicBeginLocationByName(pCfg->s_sockName.c_str());
	if (pCfg->n_ScrollHit == SkillEffectHitTpy_ScrollHit)
	{
		m_targetPos = GetInitTargetPos();
	}
	else if (pCfg->n_ScrollHit == SkillEffectHitTpy_ArrowHit)
	{
		auto to = GetAttackRole()->GetMagicBeginLocationByName(pCfg->s_sockName.c_str());
		to.X += pCfg->n_length*GetInitRotaArrowPos().X;
		to.Y += pCfg->n_length*GetInitRotaArrowPos().Y;
		m_targetPos = to;
	}
	else{
		if (GetTargetRole())
		{
			m_targetPos = GetTargetRole()->GetRoleTransByName("E_Hit").GetLocation();
		}
		else if (GetAttackRole()) {
			auto to = m_startPos;
			FVector vec = GetAttackRole()->GetActorForwardVector();
			UE_LOG(LogClass, Log, TEXT("==================== %s"), *(vec.ToString()));
			to.X += pCfg->n_length*vec.X;
			to.Y += pCfg->n_length*vec.Y;
			m_targetPos = to;
		}
	}
	
	rRot = UKismetMathLibrary::FindLookAtRotation(GetAttackRole()->GetActorLocation(), m_targetPos);
	float fDis = FVector::Distance(m_startPos, m_targetPos);
	m_fBomeHei = m_startPos.Z + pCfg->n_BomrangHei* fDis / pCfg->n_length;
	if (fDis > 0.f)
	{
		m_allTm = fDis / m_speed;
		m_curTm = 0.f;
	}
	SetActorRotation(rRot);
	RegisterOverlayEvents();
	m_bStartTmline = true;
}

void ASkillEffectCannonball::BeginCollision(AActor* OtherActor)
{
	if (m_bHitDeath)
	{
		Destroy();
	}
	Super::BeginCollision(OtherActor);
}


void ASkillEffectCannonball::EndCollision(AActor* OtherActor)
{
	Super::EndCollision(OtherActor);
}


void ASkillEffectCannonball::UpdateEffectLocation(FVector vec)
{
	FVector pos;
	pos.X = FMath::Lerp(m_startPos.X, m_targetPos.X , vec.X);
	pos.Y = FMath::Lerp(m_startPos.Y, m_targetPos.Y, vec.Y);
	pos.Z = FMath::Lerp(m_startPos.Z, m_fBomeHei , vec.Z);
	SetActorLocation(pos);
}

void ASkillEffectCannonball::FinishEffectTimeline()
{
	Destroy();
	UE_LOG(LogClass, Log, TEXT("FinishEffectTimeline"));
}