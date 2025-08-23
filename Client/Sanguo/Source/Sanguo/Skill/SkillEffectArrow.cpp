// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectArrow.h"
#include "Kismet/KismetMathLibrary.h"

#include "Player/IPlayer.h"
#include "Player/Hero.h"
#include "config/SkillTypeCfg.h"

void ASkillEffectArrow::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	Super::BeginPlay();
}

void ASkillEffectArrow::StartTimeline()
{
	FOnTimelineVectorStatic TimelineCallBack;
	TimelineCallBack.BindUFunction(this, TEXT("UpdateEffectLocation"));
	m_timelineBall.AddInterpVector(m_CurveVector, TimelineCallBack);

	FOnTimelineEventStatic finishEvt;
	finishEvt.BindUFunction(this, TEXT("FinishEffectTimeline"));
	m_timelineBall.SetTimelineFinishedFunc(finishEvt);
	m_timelineBall.Play();
	m_bStartTmline = true;
}


void ASkillEffectArrow::AddTimeEvent(float tm)
{
	if (tm < 0.00001f) {
		return;
	}
	FOnTimelineEvent TimelineCallBack;
	TimelineCallBack.BindUFunction(this, TEXT("TimeEventBack"));
	m_timelineBall.AddEvent(tm, TimelineCallBack);
}

void ASkillEffectArrow::TimeEventBack()
{
	ClearTargets();
}


void ASkillEffectArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsSkillInvalid()) {
		return;
	}
	
	if (!GetAttackRole() || !GetAttackRole()->GetDamageCollision()) {
		SkillDestroy();
		return;
	}

	if (m_trace == SkillDirectTpy_TraceTarget)
	{
		if (!GetTargetRole())
		{
			SkillDestroy();
		}
		else {
			auto pos = GetActorLocation();
			auto toRota = UKismetMathLibrary::FindLookAtRotation(pos, GetTargetRole()->GetMagicBeginLocationByName("S_Hit"));
			SetActorRotation(toRota);
			auto vec = GetActorForwardVector();
			pos.X += vec.X*DeltaTime*m_speed;
			pos.Y += vec.Y*DeltaTime*m_speed;
			pos.Z += vec.Z*DeltaTime*m_speed;
			SetActorLocation(pos);
			float fDisPer = FVector::Distance(FVector(0 , 0 , 0), FVector(vec.X*DeltaTime*m_speed, vec.Y*DeltaTime*m_speed, vec.Z*DeltaTime*m_speed));
			if (FVector::Distance(pos , GetTargetRole()->GetMagicBeginLocationByName("S_Hit")) < fDisPer)
			{
				if (m_nDamageTpy == SkillDamageTpy_Single&& m_damageSkillId > 0  //µ¥ΜεΉ¥»χ
					&& GetAttackRole() && GetAttackRole()->GetDamageCollision() && GetTargetRole()) {
					GetAttackRole()->GetDamageCollision()->SingleDamage(GetTargetRole()->GetObjectId(), m_damageSkillId);
				}
				FinishEffect();
				return;
			}
		}
	} else {
		if (m_nDamageTpy == SkillDamageTpy_Fly_Range && m_nDamageRange > 0.00001f && m_damageSkillId > 0
			&& GetAttackRole() && GetAttackRole()->GetDamageCollision()) {
			GetAttackRole()->GetDamageCollision()->EffectCircleArrowDamage(GetActorLocation(), (double)m_nDamageRange, m_vecHasAttacks, m_damageSkillId);
		}
		else if(m_nDamageTpy == SkillDamageTpy_Hit_Range  && GetAttackRole() && GetAttackRole()->GetDamageCollision()
			&& m_nDamageRange > 0.00001f){
			if (GetAttackRole()->GetDamageCollision()->CheckHitTarget(GetActorLocation(), m_nDamageRange)) {
				if (m_nDamageRange1 > 0.00001f) {
					GetAttackRole()->GetDamageCollision()->EffectCircleArrowDamage(GetActorLocation(), (double)m_nDamageRange1, m_vecHasAttacks, m_damageSkillId);
				}
				else if (GetTargetRole()) {
					GetAttackRole()->GetDamageCollision()->SingleDamage(GetTargetRole()->GetObjectId(), m_damageSkillId);
				}
				FinishEffect();
			} 
		} else if (m_bHitDeath) {
			if (GetAttackRole()->GetDamageCollision()->CheckHitTarget(GetActorLocation(), m_nDamageRange)) {
				FinishEffect();
			}
		}
	}
	if (!m_bStartTmline) return;
	if (m_trace != SkillDirectTpy_TraceTarget)
	{
		m_timelineBall.TickTimeline(DeltaTime / m_allTm);
	}
	m_curTm += DeltaTime;
	if (m_allTm > 0 && m_curTm > m_allTm)
	{
		SkillDestroy();
		return;
	}
}

void ASkillEffectArrow::BPDamageCircle(ARole* pRole, int nDamageRange, int nDamageSkillId)
{
	if (!pRole || !pRole->GetDamageCollision() || nDamageSkillId == 0)
	{
		return;
	}
	pRole->GetDamageCollision()->EffectCircleArrowDamage(GetActorLocation(), (double)nDamageRange, m_vecHasAttacks, nDamageSkillId);
}


void ASkillEffectArrow::StartSkill()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	m_speed = pCfg->n_speed;
	m_bHitDeath = pCfg->n_hitdead == 1 ? true : false;
	FRotator rRot;
	if (GetParentSkillEffect()) {
		m_startPos = GetParentSkillEffect()->GetActorLocation();
	} else if (GetAttackRole()){
		m_startPos = GetAttackRole()->GetMagicBeginLocationByName("S_Magic");
	}
	
	if (Cast<AHero>(GetAttackRole()) != nullptr)
	{
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
		else {
			if (m_trace != SkillDirectTpy_TraceTarget || !GetTargetRole()) {
				auto to = GetAttackRole()->GetMagicBeginLocationByName(pCfg->s_sockName.c_str());
				FVector vec = GetAttackRole()->GetActorForwardVector();
				to.X += pCfg->n_length*vec.X;
				to.Y += pCfg->n_length*vec.Y;
				m_targetPos = to;
			}
			else {
				m_targetPos = GetTargetRole()->GetMagicBeginLocationByName("S_Hit");
			}
		}
	} else {
		if (GetTargetRole() && pCfg->n_ScrollHit != SkillEffectHitTpy_ArrowHit) {
			m_targetPos = GetTargetRole()->GetMagicBeginLocationByName("S_Hit");
		}
		else {
			auto to = GetAttackRole()->GetMagicBeginLocationByName(pCfg->s_sockName.c_str());
			FVector vec = GetAttackRole()->GetActorForwardVector();
			to.X += pCfg->n_length*vec.X;
			to.Y += pCfg->n_length*vec.Y;
			m_targetPos = to;
		}
	}
	
	rRot = UKismetMathLibrary::FindLookAtRotation(GetAttackRole()->GetActorLocation(), m_targetPos);
	float fDis = FVector::Distance(GetAttackRole()->GetActorLocation(), m_targetPos);
	m_fBomeHei = m_startPos.Z + pCfg->n_BomrangHei;
	m_prePos = GetActorLocation();
	m_trace = pCfg->n_trace;
	m_nDamageRange = pCfg->n_DamageParam1;
	m_nDamageRange1 = pCfg->n_DamageParam2;
	m_nDamageTpy = pCfg->n_DamageTpy;

	auto&& pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetSkillId());
	if (!pSkillCfg) {
		if (GetParentSkillId() > 0) {
			pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(GetParentSkillId());
		}
	}

	if (pSkillCfg) {
		m_damageSkillId = pSkillCfg->m_id;
	}

	if (fDis > 0.f)
	{
		m_allTm = fDis / m_speed;
		m_curTm = 0.f;
	}
	if (m_allTm <= 0.f)
	{
		m_allTm = 1.0f;
	}
	if (m_trace == SkillDirectTpy_AsRate && m_allTm < 1.0f)
	{
		m_allTm = 1.0f;
	}
	if (GetParentSkillEffect()) {
		rRot = GetParentSkillEffect()->GetActorRotation();
	}
	SetActorRotation(rRot);
	RegisterOverlayEvents();
	StartDamage();
	
	if (m_trace != SkillDirectTpy_TraceTarget || !GetTargetRole())
	{
		StartTimeline();
	}
}

void ASkillEffectArrow::BeginCollision(AActor* OtherActor)
{
	if (IsSkillInvalid()) {
		return;
	}
	//if (m_bHitDeath)
	//{
	//	if (OtherActor == GetTargetRole())
	//	{
	//		FinishEffect();
	//	}
	//}
	Super::BeginCollision(OtherActor);
}


void ASkillEffectArrow::EndCollision(AActor* OtherActor)
{

}

void ASkillEffectArrow::UpdateEffectLocation(FVector vec)
{
	FVector pos;
	pos.X = FMath::Lerp(m_startPos.X, m_targetPos.X, vec.X);
	pos.Y = FMath::Lerp(m_startPos.Y, m_targetPos.Y, vec.Y);
	pos.Z = FMath::Lerp(m_startPos.Z, m_fBomeHei, vec.Z);
	auto fAddHei = m_targetPos.Z - m_startPos.Z;
	auto fAddZ = FMath::Lerp(0.f , fAddHei , vec.X);
	pos.Z += fAddZ;
	SetActorLocation(pos);
	auto toRota = UKismetMathLibrary::FindLookAtRotation(m_prePos, pos);
	m_prePos = pos;
	SetActorRotation(toRota);
}

void ASkillEffectArrow::FinishEffectTimeline()
{
	FinishEffect();
}


void ASkillEffectArrow::FinishEffect()
{
	TimeLineEnd();
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (pCfg && pCfg->n_nxtSkill > 0 && m_nNxtSkilNum < pCfg->m_nNxtMaxNum)
	{
		m_nNxtSkilNum++;
		SkillMgr::getInstance().CreateSubSkill(pCfg->n_nxtSkill, GetAttackRole(), GetTargetRole(), this,
			GetActorLocation(), GetInitRotaArrowPos());
	}
	
	SkillDestroy();
	UE_LOG(LogClass, Log, TEXT("FinishEffectTimeline"));
}

