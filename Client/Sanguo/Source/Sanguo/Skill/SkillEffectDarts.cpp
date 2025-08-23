// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectDarts.h"

#include "Kismet/KismetMathLibrary.h"

#include "Player/IPlayer.h"
#include "config/SkillTypeCfg.h"

void ASkillEffectDarts::BeginPlay()
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


void ASkillEffectDarts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto rota = GetActorRotation();
	rota.Yaw += DeltaTime*20000;
	rota.Roll = 0.f;
	rota.Pitch = 0.f;
	SetActorRotation(rota);
	m_timelineBall.TickTimeline(DeltaTime / m_allTm);
	if (GetAttackRole() && GetAttackRole()->GetDamageCollision() && m_fDamageRange > 0.00001f && m_damageSkillId > 0) {
		GetAttackRole()->GetDamageCollision()->EffectCircleDartsDamage(GetActorLocation(), m_fDamageRange, m_vecHasAttacks, m_damageSkillId);
	}
}

void  ASkillEffectDarts::AddTimeEvent(float tm)
{
	if (tm < 0.00001f) {
		return;
	}
	FOnTimelineEvent TimelineCallBack;
	TimelineCallBack.BindUFunction(this, TEXT("TimeEventBack"));
	m_timelineBall.AddEvent(tm, TimelineCallBack);
}

void ASkillEffectDarts::TimeEventBack()
{
	ClearTargets();
}


void ASkillEffectDarts::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	m_speed = pCfg->n_speed;
	m_nCurve = pCfg->n_curve;
	m_bendDis = m_bendMaxLength  = pCfg->n_length;
	m_bHitDeath = pCfg->n_hitdead == 1 ? true : false;
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
		m_bendRangePos = m_targetPos;
	}
	else {
		auto to = m_startPos;
		FVector vec = GetAttackRole()->GetActorForwardVector();
		to.X += pCfg->n_length*vec.X;
		to.Y += pCfg->n_length*vec.Y;
		m_targetPos = to;
		m_bendRangePos = m_targetPos;
		if (GetTargetRole() && (m_nCurve == SSkillCurveTpy_Bending || m_nCurve == SSkillCurveTpy_RE_Bending)) {
			float fDis = FVector::Distance(m_startPos, GetTargetRole()->GetRoleTransByName("E_Hit").GetLocation());
			if (fDis < pCfg->n_length)
			{
				auto rangPos = m_startPos;
				rangPos.X += fDis* vec.X;
				rangPos.Y += fDis * vec.Y;
				m_bendRangePos = rangPos;
				m_bendDis = fDis;
			}
		}
	}

	float fDis = FVector::Distance(m_startPos, m_targetPos);
	if (fDis > 0.f)
	{
		m_allTm = fDis / m_speed;
		m_curTm = 0.f;
	}

	auto&& pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetSkillId());
	if (!pSkillCfg) {
		if (GetParentSkillId() > 0) {
			pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetParentSkillId());
		}
	}

	if (pSkillCfg)
	{
		m_damageSkillId = pSkillCfg->m_id;
		m_fDamageRange = pCfg->n_DamageParam1;
	}

	RegisterOverlayEvents();
}

void ASkillEffectDarts::BeginCollision(AActor* OtherActor)
{
	if (m_bHitDeath)
	{
		Destroy();
	}
	Super::BeginCollision(OtherActor);
}


void ASkillEffectDarts::EndCollision(AActor* OtherActor)
{
	Super::EndCollision(OtherActor);
}


void ASkillEffectDarts::UpdateEffectLocation(FVector vec)
{
	if (m_nCurve == SkillCurveTpy_None)
	{
		FVector pos;
		pos.X = FMath::Lerp(m_startPos.X, m_targetPos.X, vec.X);
		pos.Y = FMath::Lerp(m_startPos.Y, m_targetPos.Y, vec.Y);
		pos.Z = FMath::Lerp(m_startPos.Z, m_targetPos.Z, vec.Z);
		SetActorLocation(pos);
	}
	else if(m_nCurve == SSkillCurveTpy_Bending || m_nCurve == SSkillCurveTpy_RE_Bending){
		float fRate = m_bendDis / m_bendMaxLength;
		fRate = fRate < 0.f ? 0.f : fRate > 1.0f ? 1.0f : fRate;
		float Drgee = 0;
		fRate = fRate * 60.f;
		float ang = FMath::Lerp(fRate, fRate - 60.f, vec.Y);
		Drgee = UKismetMathLibrary::DegAtan2(m_bendRangePos.Y - m_startPos.Y, m_bendRangePos.X - m_startPos.X);
		if (m_nCurve == SSkillCurveTpy_Bending) {
			Drgee += ang;
		}
		else {
			Drgee -= ang;
		}
		
		float X = m_startPos.X + m_bendMaxLength * UKismetMathLibrary::DegCos(Drgee);
		float Y = m_startPos.Y + m_bendMaxLength * UKismetMathLibrary::DegSin(Drgee);
		FVector pos;
		pos.X = FMath::Lerp(m_startPos.X, X, vec.X);
		pos.Y = FMath::Lerp(m_startPos.Y, Y, vec.Y);
		pos.Z = FMath::Lerp(m_startPos.Z, m_targetPos.Z, vec.Z);
		SetActorLocation(pos);
	}
	else if (m_nCurve == SSkillCurveTpy_Circle || m_nCurve == SSkillCurveTpy_RE_Circle) {
		double nDegree = UKismetMathLibrary::DegAtan2(m_bendRangePos.Y - m_startPos.Y, m_bendRangePos.X - m_startPos.X);
		if (m_nCurve == SSkillCurveTpy_Circle)
		{
			nDegree += FMath::Lerp(0, 360, vec.Z);
		}
		else {
			nDegree += FMath::Lerp(0, -360, vec.Z);
		}
		float fDis = FVector::Distance(m_startPos, m_targetPos);
		fDis = FMath::Lerp(0.f, fDis, vec.Y);
		float X = m_startPos.X + fDis * UKismetMathLibrary::DegCos(nDegree);
		float Y = m_startPos.Y + fDis * UKismetMathLibrary::DegSin(nDegree);
		FVector pos;
		pos.X = FMath::Lerp(m_startPos.X, X, vec.X);
		pos.Y = FMath::Lerp(m_startPos.Y, Y, vec.Y);
		pos.Z = FMath::Lerp(m_startPos.Z, m_targetPos.Z, vec.Z);
		SetActorLocation(pos);
	}
}

void ASkillEffectDarts::FinishEffectTimeline()
{
	if (GetAttackRole()) {
		auto&& player = Cast<AIPlayer>(GetAttackRole());
		if (player)
		{
			player->ShowDarts(true);
		}
	}
	Destroy();
	UE_LOG(LogClass, Log, TEXT("FinishEffectTimeline"));
}