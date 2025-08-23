// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffect.h"
#include "Engine/StaticMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

#include "data/role/Player.h"
#include "config/SkillCfg.h"
#include "config/SkillTypeCfg.h"
#include "config/BuffTypeCfg.h"
#include "config/SkillFrontEffectCfg.h"

#include "../ReichGameInstance.h"

#include "../UI/UIMsgBoxToast.h"

#include "Player/IPlayer.h"
#include "Player/Generals.h"
#include "Player/Monster.h"
#include "Player/Hero.h"

#include "SkillEffectArrow.h"
#include "SkillEffectCannonball.h"
#include "SkillEffectDarts.h"
#include "SkillEffectLaser.h"
#include "SkillFrontEffect.h"
#include "SkillEffectMagicRange.h"
#include "SkillEffectGas.h"
#include "SkillEffectEmitter.h"
#include "SkillEffectMultiTarget.h"
#include "SkillEffectTrigger.h"

extern UWorld* gp_UWorld;

// Sets default values
ASkillEffect::ASkillEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkillEffect::BeginPlay()
{
	Super::BeginPlay();
	
	UI_REGISTER_MYEVENT(InsEndEvent, &ASkillEffect::onInsEndEvent);
}

void ASkillEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsSkillInvalid()) {
		if (m_fDieTm > 0.000001f)
		{
			m_fDieTm -= DeltaTime;
			if (m_fDieTm < 0.000001f)
			{
				Destroy();
			}
		}
	}
}

void ASkillEffect::onInsEndEvent(const InsEndEvent& evt)
{
	Destroy();
}

//****************************************************************************************
//
//****************************************************************************************
void ASkillEffect::RegisterOverlayEvents()
{
	TArray<UActorComponent*> meshs = this->GetComponentsByTag(UBoxComponent::StaticClass(), TEXT("HitBox"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	auto StaticMesh = Cast<UBoxComponent>(meshs[0]);
	if (!StaticMesh)
	{
		return;
	}
	FScriptDelegate OverlayBegin;
	OverlayBegin.BindUFunction(this, "OnOverlayBegin");
	StaticMesh->OnComponentBeginOverlap.Add(OverlayBegin);

	FScriptDelegate OverlayEnd;
	OverlayEnd.BindUFunction(this, "OnOverlayEnd");
	StaticMesh->OnComponentEndOverlap.Add(OverlayEnd);
}

//****************************************************************************************
//
//****************************************************************************************
void ASkillEffect::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit)
{
	if (!OtherActor)
	{
		return;
	}
	//BeginCollision(OtherActor);
}

//****************************************************************************************
//
//****************************************************************************************
void ASkillEffect::OnOverlayBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor || m_bInvalid)
	{
		return;
	}
	BeginCollision(OtherActor);
}

//****************************************************************************************
//
//****************************************************************************************
void ASkillEffect::OnOverlayEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}
	BeginCollision(OtherActor);
}

void ASkillEffect::BeginCollision(AActor* OtherActor)
{
}

void ASkillEffect::EndCollision(AActor* OtherActor)
{

}

void ASkillEffect::Damage(int nSkillId, int nDamageTpy, int nDamageParam1, int nDamageParam2, int nDamageParam3, int nDamageParam4)
{
	if (!m_rAttackRole) {
		return;
	}

	switch (nDamageTpy) {
	case SkillDamageTpy_CircleRange:
	{
		m_rAttackRole->GetDamageCollision()->CircleDamage(nDamageParam1, nSkillId);
	}
	break;
	case SkillDamageTpy_Rectangle_Range:
	{
		if (nDamageParam4 == 1)  //nDamageParam4拿来临时判断，是按照特效的算矩形区域还是攻击者的计算矩形区域
		{
			m_rAttackRole->GetDamageCollision()->RectangleEffectDamage(GetActorLocation(), GetActorForwardVector(), 
				nDamageParam2, nDamageParam3, nSkillId);
		}
		else {
			m_rAttackRole->GetDamageCollision()->RectangleDamage(nDamageParam1, nDamageParam2, nDamageParam3, nSkillId);
		}
	}
	break;
	case SkillDamageTpy_Effect_CircleRange:
	{
		m_rAttackRole->GetDamageCollision()->EffectCircleDamage(GetActorLocation(), nDamageParam1, nSkillId);
	}
	break;
	case SkillDamageTpy_Sector:
	{
		m_rAttackRole->GetDamageCollision()->SectorDamage(nDamageParam1, nDamageParam2, nDamageParam3, nSkillId);
	}
	break;
	case SkillDamageTpy_To_Self:
	{
		m_rAttackRole->GetDamageCollision()->CastSelfSingle(nSkillId);
	}
	break;
	default:
		break;
	}
}

void ASkillEffect::StartDamage()
{
	if (!m_rAttackRole || !m_rAttackRole->GetDamageCollision() || m_nSkillId == 0) {
		return;
	}
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(m_nSkillId);
	if (!pCfg || pCfg->n_DamageTpy == 0) {
		return;
	}
	auto&& pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(m_nSkillId);
	if (!pSkillCfg) {
		if (m_nParentSkillId > 0)
		{
			pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(m_nParentSkillId);
		}
		if (!pSkillCfg) {
			return;
		}
	}
	Damage(pSkillCfg->m_id, pCfg->n_DamageTpy, pCfg->n_DamageParam1, pCfg->n_DamageParam2, pCfg->n_DamageParam3, pCfg->n_DamageParam4);
}

void ASkillEffect::BpStartDamage()
{
	if (!m_rAttackRole || !m_rAttackRole->GetDamageCollision() || m_nSkillId == 0) {
		return;
	}
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(m_nSkillId);
	if (!pCfg || pCfg->n_BpDamageTpy == 0) {
		return;
	}
	auto&& pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(m_nSkillId);
	if (!pSkillCfg) {
		if (m_nParentSkillId > 0)
		{
			pSkillCfg = SkillCfg::getInstance().findSkillTypeByTempId(m_nParentSkillId);
		}
		if (!pSkillCfg) {
			return;
		}
	}
	Damage(pSkillCfg->m_id, pCfg->n_BpDamageTpy, pCfg->n_DamageParam1, pCfg->n_DamageParam2, pCfg->n_DamageParam3, pCfg->n_DamageParam4);
}

//****************************************************************************************
//
//****************************************************************************************
void ASkillEffect::SetInfo(int nSkillId, ARole* aAttackRole, ARole* rTargetRole, FVector targetPos, FVector rota, int nSkillIdx)
{
	m_nSkillId = nSkillId;
	m_rAttackRole = aAttackRole;
	m_rTargetRole = rTargetRole;
	m_vecTargetPos = targetPos;
	m_rotaTarget = rota;
	m_nSkillIdx = nSkillIdx;
	PlaySkillSound();
}

//****************************************************************************************
//
//****************************************************************************************
void ASkillEffect::PlaySkillSound()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(m_nSkillId);
	if (!pCfg || pCfg->s_sound.empty())
	{
		return;
	}

	if (UReichGameInstance::IsEditorMode() || !AIPlayerMgr::getInstance().GetHero())
	{
		return;
	}

	FVector pos = AIPlayerMgr::getInstance().GetHero()->GetActorLocation();
	if (FVector::Distance(GetActorLocation(), pos) >= 3000) {
		return;
	}

	FString cstr(pCfg->s_sound.c_str());
	TArray<FStringFormatArg> arrStr;
	arrStr.Add(cstr);
	arrStr.Add(cstr);
	FString strPath = FString::Format(TEXT("SoundWave'/Game/Sound/effect/{0}.{1}'"), arrStr);
	UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, strPath.GetCharArray().GetData());
	if (soundObj)
	{
		UGameplayStatics::PlaySound2D(gp_UWorld, Cast<USoundBase>(soundObj));
	}
}

//****************************************************************************************
//
//****************************************************************************************
void ASkillEffect::SetParentSkill(ASkillEffect* pSkillEffect)
{
	m_pParentEffect = pSkillEffect;
	m_bSubSkillEffect = true;
	m_bOriginSKill = true;
	m_nParentSkillId = pSkillEffect->GetSkillId();
}

FVector ASkillEffect::GetInitTargetPos() 
{ 
	return m_vecTargetPos; 
}


//****************************************************************************************
//
//****************************************************************************************
FTransform ASkillEffect::GetRoleTransByName(const char* pszSockName)
{
	if (pszSockName == NULL)
	{
		return GetActorTransform();
	}
	FString strSockName(pszSockName);
	if (strSockName.IsEmpty())
	{
		return GetActorTransform();
	}
	TArray<UActorComponent*> meshs = this->K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	if (meshs.Num() <= 0)
	{
		return GetActorTransform();
	}

	for (int i = 0; i < meshs.Num(); i++)
	{
		auto mesh = Cast<USkeletalMeshComponent>(meshs[i]);
		if (mesh && mesh->GetSocketByName(*strSockName))
		{
			auto trans = mesh->GetSocketTransform(*strSockName);
			auto roleTrans = GetActorTransform();;
			trans.SetRotation(roleTrans.GetRotation());
			return trans;
		}
	}
	return GetActorTransform();
}

void ASkillEffect::SkillDestroy()
{
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(GetSkillId());
	if (pCfg && pCfg->f_dieTm > 0.000001)
	{
		SetSkillInvalid(true);
		if (m_fDieTm < 0.000001) {
			m_fDieTm = pCfg->f_dieTm;
		}
	}
	else {
		PrimaryActorTick.bCanEverTick = false;
		Destroy();
	}
}




//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************

bool SkillMgr::initialize()
{
	return true;
}

//****************************************************************************************
//
//****************************************************************************************
void SkillMgr::terminate()
{

}

//****************************************************************************************
//
//****************************************************************************************
bool SkillMgr::reset()
{
	return true;
}

//****************************************************************************************
//
//****************************************************************************************
void SkillMgr::update(float dt)
{

}

//****************************************************************************************
//
//****************************************************************************************
void SkillMgr::CreateSkill(int nSkillId, ARole* attackRole, ARole* targetRole , FVector targetPos, FVector rator)
{
	if (!attackRole)
	{
		return;
	}
	ARole* pOriTargetRole = targetRole;
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(nSkillId);
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}

	TArray<ARole*> MutiTargets;

	if (attackRole == AIPlayerMgr::getInstance().GetHero())
	{
		if (pCfg->n_needTarget == 1 && targetRole == nullptr) {
			MsgBoxToastMgr::getInstance().ShowText(__LC("UI_SKILL_EFFECT_NEED_TARGET"));
			return;
		}

		if (pCfg->n_nCastDis > 0) {
			if ((pCfg->n_SkillTpy == SkillEffectTpy_Arrow || (pCfg->n_SkillTpy == SkillEffectTpy_Laser && pCfg->n_DamageTpy == SkillDamageTpy_Single)
				|| pCfg->n_SkillTpy == SkillEffectTpy_Emitter)
				&& attackRole->GetFocusRole() &&
				FVector::Distance(attackRole->GetActorLocation(), attackRole->GetFocusRole()->GetActorLocation()) > pCfg->n_nCastDis) {
				MsgBoxToastMgr::getInstance().ShowText(__LC("UI_SKILL_EFFECT_CAST_DISTANCE_FAR"));
				return;
			} else if (pCfg->n_SkillTpy == SkillEffectTpy_Multi) {
				if (attackRole->GetFocusRole() &&
					FVector::Distance(attackRole->GetActorLocation(), attackRole->GetFocusRole()->GetActorLocation()) > pCfg->n_nCastDis) {
					MsgBoxToastMgr::getInstance().ShowText(__LC("UI_SKILL_EFFECT_CAST_DISTANCE_FAR"));
					return;
				}
				else if(attackRole->GetDamageCollision()) {
					attackRole->GetDamageCollision()->CalMutiTargets(pCfg->n_nCastDis, pCfg->n_num, MutiTargets);
				}
			}
		}
	} else {
		if (pCfg->n_SkillTpy == SkillEffectTpy_Multi || pCfg->n_SkillTpy == SkillEffectTpy_Multi_Arrow) {
			if (attackRole->GetDamageCollision()) {
				attackRole->GetDamageCollision()->CalMutiTargets(pCfg->n_nCastDis, pCfg->n_num, MutiTargets);
			}
		}
	}
	
	

	if (attackRole == AIPlayerMgr::getInstance().GetHero()) {
		AHero* pHero = Cast<AHero>(attackRole);
		auto&& self = PlayerMgr::getInstance().getSelf();
		if (pHero && self) {
			pHero->SkillConsume(self->CalcConsumeHp(pCfg->f_Hp), self->CalcConsumeHp(pCfg->f_Mana), pCfg->n_Sten, pCfg->n_Anger);
		}
	}

	FTransform trans = attackRole->GetRoleTransByName(pCfg->s_sockName.c_str());

	std::string strPath = SkillTypeCfg::getInstance().GetSkillPath() + "/" + pCfg->s_demoPath;//TCHAR_TO_UTF8(*FString::Format(TEXT("{0}/{1}", SkillTypeCfg::getInstance().GetSkillPath(), pCfg->s_demoPath);
	FString path = UReichGameInstance::TransClassAssertPath(strPath);
	UE_LOG(LogOutputDevice, Log, TEXT("SkillMgr::CreateSkill path = %s") , *path);

	for (int i = 0 ; i < pCfg->n_num ; i++)
	{
		UClass* generalClass = LoadObject<UClass>(NULL, *path);
		if (!generalClass) {
			UE_LOG(LogOutputDevice, Log, TEXT("SkillMgr::CreateSkill failed"));
			return;
		}

		ASkillEffect* pSkillEffect = NULL;
		switch (pCfg->n_SkillTpy)
		{
		case SkillEffectTpy_Arrow:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectArrow>(generalClass, trans);
			break;
		case SkillEffectTpy_Cannon:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectCannonball>(generalClass, trans);
			break;
		case SkillEffectTpy_Darts:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectDarts>(generalClass, trans);
			break;
		case SkillEffectTpy_Laser:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectLaser>(generalClass, trans);
			break;
		case SkillEffectTpy_MagicRange:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectMagicRange>(generalClass, trans);
			break;
		case SkillEffectTpy_Gas:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectGas>(generalClass, trans);
			break;
		case SkillEffectTpy_Emitter:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectEmitter>(generalClass, trans);
			break;
		case SkillEffectTpy_Trigger:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectTrigger>(generalClass, trans);
			break;
		case SkillEffectTpy_Multi:
			{
				if (!attackRole->m_aRangeCollion)
				{
					return;
				}
				if (i >= MutiTargets.Num())
				{
					break;
				}
				targetRole = MutiTargets[i];
				targetPos = MutiTargets[i]->GetActorLocation();
				pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectMultiTarget>(generalClass, trans);
				break;
			}
		case SkillEffectTpy_Multi_Arrow:
			{
				if (!attackRole->m_aRangeCollion)
				{
					return;
				}
				if (i >= MutiTargets.Num())
				{
					break;
				}
				targetRole = MutiTargets[i];
				targetPos = MutiTargets[i]->GetActorLocation();
				pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectArrow>(generalClass, trans);
			}
			break;
		default:
			break;
		}
		if (!pSkillEffect)
		{
			UE_LOG(LogOutputDevice, Log, TEXT("SkillMgr::CreateSkill pSkillEffect is null"));
			return;
		}
		pSkillEffect->SetInfo(nSkillId, attackRole, targetRole, targetPos, rator , i);
		pSkillEffect->StartSkill();
		pSkillEffect->InitSkillEvent();
	}
}


//****************************************************************************************
//
//****************************************************************************************
void SkillMgr::CreateFrontSkill(int nSkillId, ARole* attackRole, ARole* targetRole, FVector targetPos, FVector rator)
{
	if (!attackRole)
	{
		return;
	}
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(nSkillId);
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	auto&& pSkillCfg = SkillFrontEffectCfg::getInstance().GetSkillFrontType(pCfg->n_frontSkill);
	if (!pSkillCfg || pSkillCfg->s_DemoPath.empty())
	{
		return;
	}

	FTransform trans = attackRole->GetRoleTransByName(pSkillCfg->s_SockName.c_str());

	std::string strPath = SkillFrontEffectCfg::getInstance().GetSkillPath() + "/" + pSkillCfg->s_DemoPath;//std::string strPath = TCHAR_TO_UTF8(*FString::Format(TEXT("{0}/{1}", SkillFrontEffectCfg::getInstance().GetSkillPath(), pSkillCfg->s_DemoPath);
	FString path = UReichGameInstance::TransClassAssertPath(strPath);

	UClass* generalClass = LoadObject<UClass>(NULL, *path);
	if (!generalClass) {
		UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::CreateFrontSkill failed"));
		return;
	}

	ASkillFrontEffect* pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillFrontEffect>(generalClass, trans);;
	if (!pSkillEffect)
	{
		return;
	}
	pSkillEffect->SetInfo(nSkillId, attackRole, targetRole, targetPos, rator);
	pSkillEffect->StartSkill();
	pSkillEffect->InitSkillEvent();
}

//****************************************************************************************
//
//****************************************************************************************
void SkillMgr::CreateSubSkill(int nSkillId, ARole* attackRole, ARole* targetRole, ASkillEffect* pParentSkill, FVector targetPos, FVector rator, int nSkillIdx)
{
	if (!attackRole || !pParentSkill)
	{
		return;
	}
	ARole* pOriTargetRole = targetRole;
	auto&& pCfg = SkillTypeCfg::getInstance().GetSkillType(nSkillId);
	if (!pCfg || pCfg->s_demoPath.empty())
	{
		return;
	}
	FTransform trans = pParentSkill->GetActorTransform();
	FVector vPos = pParentSkill->GetActorLocation();

	std::string strPath = SkillTypeCfg::getInstance().GetSkillPath() + "/" + pCfg->s_demoPath;//std::string strPath = TCHAR_TO_UTF8(*FString::Format(TEXT("{0}/{1}", SkillTypeCfg::getInstance().GetSkillPath(), pCfg->s_demoPath);
	FString path = UReichGameInstance::TransClassAssertPath(strPath);

	for (int i = 0; i < pCfg->n_num; i++)
	{
		UClass* generalClass = LoadObject<UClass>(NULL, *path);
		if (!generalClass) {
			UE_LOG(LogOutputDevice, Log, TEXT("AIPlayer::CreateSubSkill failed"));
			return;
		}
		ASkillEffect* pSkillEffect = NULL;
		switch (pCfg->n_SkillTpy)
		{
		case SkillEffectTpy_Arrow:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectArrow>(generalClass, trans);
			break;
		case SkillEffectTpy_Cannon:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectCannonball>(generalClass, trans);
			break;
		case SkillEffectTpy_Darts:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectDarts>(generalClass, trans);
			break;
		case SkillEffectTpy_Laser:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectLaser>(generalClass, trans);
			break;
		case SkillEffectTpy_MagicRange:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectMagicRange>(generalClass, trans);
			break;
		case SkillEffectTpy_Gas:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectGas>(generalClass, trans);
			break;
		case SkillEffectTpy_Emitter:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectEmitter>(generalClass, trans);
			break;
		case SkillEffectTpy_Trigger:
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectTrigger>(generalClass, trans);
			break;
		case SkillEffectTpy_Multi:
		{
			if (!attackRole->m_aRangeCollion)
			{
				return;
			}
			const TArray<ARole*>& targets = attackRole->m_aRangeCollion->GetTargets();
			if (targets.Find(targetRole)) {
				if (i >= targets.Num()) {
					return;
				}
				targetRole = targets[i];
			}
			else {
				if (i < targets.Num()) {
					targetRole = targets[i];
				}
				else if (i == targets.Num()) {
					targetRole = pOriTargetRole;
				}
				else if (i > targets.Num()) {
					return;
				}
			}
			if (!targetRole) {
				return;
			}
			
			pSkillEffect = attackRole->GetWorld()->SpawnActor<ASkillEffectMultiTarget>(generalClass, trans);
			break;
		}
		default:
			break;
		}
		if (!pSkillEffect)
		{
			return;
		}
		pSkillEffect->SetInfo(nSkillId, attackRole, targetRole, targetPos, rator, (nSkillIdx >= 0 && pCfg->n_num == 1) ? nSkillIdx : i);
		pSkillEffect->SetParentSkill(pParentSkill);
		pSkillEffect->StartSkill();
		pSkillEffect->InitSkillEvent();
	}
}

//****************************************************************************************
//
//****************************************************************************************
SkillMgr::SkillMgr()
{
	initialize();
}

//****************************************************************************************
//
//****************************************************************************************
SkillMgr& SkillMgr::getInstance()
{
	static SkillMgr mgr;
	return mgr;
}



