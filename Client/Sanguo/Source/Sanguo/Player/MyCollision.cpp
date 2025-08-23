// Fill out your copyright notice in the Description page of Project Settings.
#include "MyCollision.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Blueprint.h"
#include "Monster.h"
#include "IPlayer.h"
#include "Generals.h"
#include "Soldier.h"
#include "Role.h"
#include "Hero.h"


// Sets default values
AMyCollision::AMyCollision()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyCollision::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> meshs = this->GetComponentsByTag(USphereComponent::StaticClass(), TEXT("HitBox"));
	if (meshs.Num() <= 0)
	{
		return;
	}

	auto StaticMesh = Cast<USphereComponent>(meshs[0]);
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
	m_bInitIn = true;
}


//****************************************************************************************
//
//****************************************************************************************
void AMyCollision::OnOverlayBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor || m_bInvalid) {
		return;
	}

	ARole* pRole = Cast<ARole>(OtherActor);
	if (!pRole)
	{
		return;
	}

	//AddTargetRole(pRole);
	//if (pRole == AIPlayerMgr::getInstance().GetHero()) {
	//	return;
	//}
	//if (!GameAppExtern::IsServer() || pRole == AIPlayerMgr::getInstance().GetHero())
	//{
	//	return;
	//}
	//if (!m_owner) {
	//	return;
	//}
	//AIPlayer* pAttacker = Cast<AIPlayer>(m_owner);
	//if (!pAttacker)
	//{
	//	return;
	//}
	//AIPlayer* pPlayer = Cast<AIPlayer>(pRole);
	//if (!pPlayer || !pPlayer->m_playerData) {
	//	return;
	//}
	//pAttacker->SendInPlayerSight(pPlayer->m_playerData->getIggId());
	//if (m_bInitIn)
	//{
	//	pPlayer->SendInPlayerSight(pAttacker->m_playerData->getIggId());
	//}
}


//****************************************************************************************
//
//****************************************************************************************
void AMyCollision::OnOverlayEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || m_bInvalid)
	{
		return;
	}
	ARole* pRole = Cast<ARole>(OtherActor);
	if (!pRole)
	{
		return;
	}
	//m_arryRoles.RemoveSingle(pRole);
	if (!GameAppExtern::IsServer())
	{
		return;
	}
}

// Called every frame
void AMyCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//****************************************************************************************
//判断攻击范围内有没有目标被击中
//****************************************************************************************
bool AMyCollision::CheckHitTarget(FVector pos, double R)
{
	if (!m_owner) {
		return false;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return false;
	}

	DamageRange::Point owerPos(pos.X, pos.Y);

	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true)
			|| std::abs(pTargeter->GetActorLocation().Z - pos.Z) > 200)
		{
			continue;
		}

		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R)
		{
			return true;
		}
	}
	return false;
}

//****************************************************************************************
//攻击者单体伤害
//****************************************************************************************
void AMyCollision::SingleDamage(int nObjId, int nSkillId, int nDis)
{
	if (!m_owner || nObjId == 0) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}

	std::vector<int> vecTargets;
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true)
			|| pTarget->GetObjectId() != nObjId)
		{
			continue;
		}
		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}
		if (nDis > 0 && FVector::Distance(pAttacker->GetActorLocation(), pTargeter->GetActorLocation()) > nDis)
		{
			continue;
		}
		vecTargets.push_back(nObjId);
		break;
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
		}
		else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}

//****************************************************************************************
//攻击者范围伤害
//****************************************************************************************
void AMyCollision::CircleDamage(double R, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}
	std::vector<int> vecTargets;
	DamageRange::Point owerPos(pAttacker->GetActorLocation().X, pAttacker->GetActorLocation().Y);
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true)
			|| pTarget->GetObjectId() == 0)
		{
			continue;
		}
		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}
		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R &&
			std::abs(pTargeter->GetActorLocation().Z - pAttacker->GetActorLocation().Z) < 300) {
			vecTargets.push_back(pTarget->GetObjectId());
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
		}
		else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}


//****************************************************************************************
//位置范围伤害
//****************************************************************************************
void AMyCollision::EffectCircleDamage(FVector pos, double R, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}
	std::vector<int> vecTargets;
	DamageRange::Point owerPos(pos.X, pos.Y);
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true)
			|| pTarget->GetObjectId() == 0)
		{
			continue;
		}
		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}
		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R &&
			std::abs(pTargeter->GetActorLocation().Z - pos.Z) < 800) {
			vecTargets.push_back(pTarget->GetObjectId());
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
		}
		else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}

//****************************************************************************************
//正前方距离位置的范围伤害
//****************************************************************************************
void AMyCollision::EffectCircleDistanceDamage(FVector effectPos, FVector forwardPos, double R, double Dis, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker)
	{
		return;
	}

	std::vector<int> vecTargets;

	DamageRange::Point owerPos(effectPos.X + forwardPos.X * Dis,
		effectPos.Y + forwardPos.Y * Dis);
	
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true) || pTarget->GetObjectId() == 0
			|| std::abs(pTargeter->GetActorLocation().Z - effectPos.Z) > 300)  //不可攻击或者攻击不到
		{
			continue;
		}
		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}

		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R) {
			vecTargets.push_back(pTarget->GetObjectId());
			continue;
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
		}
		else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}

//****************************************************************************************
//正前方距离位置的范围伤害，不重复计算对象
//****************************************************************************************
void AMyCollision::EffectCircleDistanceDamageFit(FVector effectPos, FVector forwardPos, double R, double Dis, std::vector<int>& vecObjIds, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker)
	{
		return;
	}

	std::vector<int> vecTargets;

	DamageRange::Point owerPos(effectPos.X + forwardPos.X * Dis,
		effectPos.Y + forwardPos.Y * Dis);

	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true) || pTarget->GetObjectId() == 0
			|| std::abs(pTargeter->GetActorLocation().Z - effectPos.Z) > 300
			|| std::find(vecObjIds.begin(), vecObjIds.end(), pTarget->GetObjectId()) != vecObjIds.end())  //不可攻击或者攻击不到
		{
			continue;
		}

		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}

		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R) {
			vecTargets.push_back(pTarget->GetObjectId());
			vecObjIds.push_back(pTarget->GetObjectId());
			continue;
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
		}
		else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}

//****************************************************************************************
//箭伤害，不重复计算对象
//***************************************************************************************
void AMyCollision::EffectCircleArrowDamage(FVector pos, double R, std::vector<int>& vecObjIds, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}
	std::vector<int> vecTargets;
	DamageRange::Point owerPos(pos.X, pos.Y);
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true)
			|| pTarget->GetObjectId() == 0 || std::find(vecObjIds.begin(), vecObjIds.end(), pTarget->GetObjectId()) != vecObjIds.end())
		{
			continue;
		}

		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}

		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R &&
			std::abs(pTargeter->GetActorLocation().Z - pos.Z) < 300) {
			vecTargets.push_back(pTarget->GetObjectId());
			vecObjIds.push_back(pTarget->GetObjectId());
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
		}
		else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}

//****************************************************************************************
//回旋镖
//****************************************************************************************
void AMyCollision::EffectCircleDartsDamage(FVector pos, double R, std::vector<int>& vecObjIds, int nSkillId)
{
	return EffectCircleArrowDamage(pos, R, vecObjIds, nSkillId);
}

//****************************************************************************************
//攻击者位置扇形范围伤害
//****************************************************************************************
void AMyCollision::SectorDamage(double R1, double angle, double R2, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker)
	{
		return;
	}

	std::vector<int> vecTargets;
	DamageRange::Point owerPos(pAttacker->GetActorLocation().X, pAttacker->GetActorLocation().Y);
	FVector vecForward = pAttacker->GetActorForwardVector();
	DamageRange::Point owerPosA(pAttacker->GetActorLocation().X + vecForward.X*100,
		pAttacker->GetActorLocation().Y + vecForward.Y*100);
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true) || pTarget->GetObjectId() == 0
			|| std::abs(pTargeter->GetActorLocation().Z - pAttacker->GetActorLocation().Z) > 300)  //不可攻击或者攻击不到
		{
			continue;
		}

		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}

		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R1) {
			vecTargets.push_back(pTarget->GetObjectId());
			continue;
		}

		if (m_DaRange.isInSector(owerPos, owerPosA, targetPos, angle, R2)) {
			vecTargets.push_back(pTarget->GetObjectId());
			continue;
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
		} else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}

void AMyCollision::SectorForwardDamage(FVector effectPos, FVector forwardPos, double R1, double angle, double R2, std::vector<int>& vecObjIds, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker)
	{
		return;
	}

	std::vector<int> vecTargets;
	DamageRange::Point owerPos(effectPos.X, effectPos.Y);
	DamageRange::Point owerPosA(effectPos.X + forwardPos.X * 100,
		effectPos.Y + forwardPos.Y * 100);
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true) || pTarget->GetObjectId() == 0
			|| std::abs(pTargeter->GetActorLocation().Z - pAttacker->GetActorLocation().Z) > 300
			|| std::find(vecObjIds.begin(), vecObjIds.end(), pTarget->GetObjectId()) != vecObjIds.end())  //不可攻击或者攻击不到
		{
			continue;
		}

		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}

		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R1) {
			vecTargets.push_back(pTarget->GetObjectId());
			vecObjIds.push_back(pTarget->GetObjectId());
			continue;
		}

		if (m_DaRange.isInSector(owerPos, owerPosA, targetPos, angle, R2)) {
			vecTargets.push_back(pTarget->GetObjectId());
			vecObjIds.push_back(pTarget->GetObjectId());
			continue;
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
		}
		else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}


//****************************************************************************************
//攻击者位置矩形范围伤害
//****************************************************************************************
void AMyCollision::RectangleDamage(double R1, double fWidth, double fHeight, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker)
	{
		return;
	}

	std::vector<int> vecTargets;
	DamageRange::Point owerPos(pAttacker->GetActorLocation().X, pAttacker->GetActorLocation().Y);
	DamageRange::Point owerPosA(pAttacker->GetActorLocation().X + pAttacker->GetActorForwardVector().X*100,
		pAttacker->GetActorLocation().Y + pAttacker->GetActorForwardVector().Y*100);
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true) || pTarget->GetObjectId() == 0
			|| std::abs(pTargeter->GetActorLocation().Z - pAttacker->GetActorLocation().Z) > 300)  //不可攻击或者攻击不到
		{
			continue;
		}

		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}

		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R1) {
			vecTargets.push_back(pTarget->GetObjectId());
			continue;
		}

		if (m_DaRange.isPointInsideRectangle(owerPos, owerPosA, fHeight, fWidth, targetPos)) {
			vecTargets.push_back(pTarget->GetObjectId());
			continue;
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}
			
		} else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}

//****************************************************************************************
//特效位置矩形伤害
//****************************************************************************************
void AMyCollision::RectangleEffectDamage(FVector effectPos, FVector vecFor, double R1, double fWidth, double fHeight, int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker)
	{
		return;
	}

	std::vector<int> vecTargets;
	DamageRange::Point owerPos(effectPos.X, effectPos.Y);
	DamageRange::Point owerPosA(effectPos.X + vecFor.X * 100,
		effectPos.Y + vecFor.Y * 100);
	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true) || pTarget->GetObjectId() == 0
			|| std::abs(pTargeter->GetActorLocation().Z - pAttacker->GetActorLocation().Z) > 300)  //不可攻击或者攻击不到
		{
			continue;
		}

		if ((nSkillId > 0 && nSkillId != 19004) && pTargeter->IsOnlyBeNormalAtk()) {  //只能被普通攻击伤害 骑兵技能除外
			continue;
		}

		DamageRange::Point targetPos(pTargeter->GetActorLocation().X, pTargeter->GetActorLocation().Y);
		if (m_DaRange.getDistance(owerPos, targetPos) < R1) {
			vecTargets.push_back(pTarget->GetObjectId());
			continue;
		}

		if (m_DaRange.isPointInsideRectangle(owerPos, owerPosA, fHeight, fWidth, targetPos)) {
			vecTargets.push_back(pTarget->GetObjectId());
			continue;
		}
	}

	if (!vecTargets.empty()) {
		if (nSkillId == 0) {
			if (pAttacker == Cast<ARole>(AIPlayerMgr::getInstance().GetHero())) {
				GameAppExtern::c2cMsgPlayerNormalAttackC2S(0, vecTargets, 0, 0, false);
			}
			else {
				GameAppExtern::c2cNormalAttackTargets(pAttacker->GetObjectId(), vecTargets);
			}

		}
		else {
			GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
		}
	}
}

//****************************************************************************************
//对自己使用
//****************************************************************************************
void AMyCollision::CastSelfSingle(int nSkillId)
{
	if (!m_owner) {
		return;
	}
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker)
	{
		return;
	}
	if (pAttacker->IsDeath()) {
		return;
	}
	std::vector<int> vecTargets;
	vecTargets.push_back(pAttacker->GetObjectId());
	GameAppExtern::c2cSkillAttackTargets(nSkillId, pAttacker->GetObjectId(), vecTargets);
}

//****************************************************************************************
//
//****************************************************************************************
const TArray<ARole*>& AMyCollision::GetTargets()
{
	return m_arryRoles;
}

void AMyCollision::AddTargetRole(ARole* pRole, bool bPassive)
{
	if (!pRole || !m_owner || m_owner == pRole || m_owner->IsDeath())
	{
		return;
	}

	if (m_arryRoles.Contains(pRole)) {
		return;
	}

	if (!m_owner->IsCanAttackTarget(pRole, bPassive))
	{
		return;
	}

	m_arryRoles.Add(pRole);
	if (m_aFocusRole == nullptr) {
		m_aFocusRole = pRole;
	}
}

void AMyCollision::RemoveTargetRole(ARole* pRole)
{
	if (!pRole || !m_arryRoles.Contains(pRole)) {
		return;
	}

	m_arryRoles.RemoveSingle(pRole);
	if (pRole == m_aFocusRole) {
		if (m_arryRoles.Num() > 0) {
			m_aFocusRole = m_arryRoles[0];
		}
		else {
			m_aFocusRole = nullptr;
		}
	}
}

void AMyCollision::Reset()
{
	m_arryRoles.Empty();
	m_aFocusRole = nullptr;
}

bool AMyCollision::HasTargetRole(ARole* pRole)
{
	if (!pRole) {
		return false;
	}
	return m_arryRoles.Contains(pRole);
}

bool AMyCollision::RemoveId(int nId)
{
	for (auto&& role : m_arryRoles)
	{
		if (role && role->GetObjectId() == nId)
		{
			RemoveTargetRole(role);
			return true;
		}
	}
	return false;
}

bool AMyCollision::AddTargetId(int nId, bool bPassive)
{
	if (!m_owner) {
		return false;
	}
	AIPlayer* pPlayer = AIPlayerMgr::getInstance().GetPlayer(nId);
	if (pPlayer) {
		if (m_owner->IsCanAttackTarget(pPlayer, bPassive)) {
			AddTargetRole(pPlayer, bPassive);
			return true;
		}
		return false;
	}

	AGenerals* pGen = AGeneralsMgr::getInstance().GetGeneral(nId);
	if (pGen) {
		if (m_owner->IsCanAttackTarget(pGen, bPassive)) {
			AddTargetRole(pGen, bPassive);
			return true;
		}
		return false;
	}

	AMonster* pMonster = AMonsterMgr::getInstance().GetMonster(nId);
	if (pMonster) {
		if (m_owner->IsCanAttackTarget(pMonster, bPassive)) {
			AddTargetRole(pMonster, bPassive);
			return true;
		}
		return false;
	}

	ASoldier* pSolider = ASoldierMgr::getInstance().GetSolider(nId);
	if (pSolider) {
		if (m_owner->IsCanAttackTarget(pSolider, bPassive)) {
			AddTargetRole(pSolider, bPassive);
			return true;
		}
		return false;
	}

	return false;
}

void AMyCollision::CalFocusRole(FString name)
{
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}
	
	if (m_arryRoles.Num() == 0)
	{
		return;
	}

	m_arryRoles.Sort([this, pAttacker, name](const AActor& aA, const AActor& aB) {
		if (aA.GetName() == name) {
			return true;
		}

		if (aB.GetName() == name) {
			return false;
		}

		DamageRange::Point owerPos(pAttacker->GetActorLocation().X, pAttacker->GetActorLocation().Y);
		DamageRange::Point owerPosA(pAttacker->GetActorLocation().X + pAttacker->GetActorForwardVector().X * 100,
			pAttacker->GetActorLocation().Y + pAttacker->GetActorForwardVector().Y * 100);
		DamageRange::Point targetPosA(aA.GetActorLocation().X, aA.GetActorLocation().Y);
		DamageRange::Point targetPosB(aB.GetActorLocation().X, aB.GetActorLocation().Y);
		double fDisA = m_DaRange.getDistance(owerPos, targetPosA) + m_DaRange.calTheta(owerPos, owerPosA, targetPosA)*5;
		double fDisB = m_DaRange.getDistance(owerPos, targetPosB) + m_DaRange.calTheta(owerPos, owerPosA, targetPosB)*5;

		return fDisA < fDisB;
	});
	m_aFocusRole = m_arryRoles[0];
}

void AMyCollision::CalMutiTargets(int nDistace, int nMaxNum, TArray<ARole*>& arrTargets)
{
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}

	for (auto&& pTarget : m_arryRoles)
	{
		if (!pTarget)
		{
			continue;
		}
		ARole* pTargeter = Cast<ARole>(pTarget);
		if (!pTargeter || !pAttacker->IsCanAttackTarget(pTarget, true) || pTarget->GetObjectId() == 0)  //不可攻击或者攻击不到
		{
			continue;
		}
		DamageRange::Point owerPos(pAttacker->GetActorLocation().X, pAttacker->GetActorLocation().Y);
		DamageRange::Point owerPosA(pAttacker->GetActorLocation().X + pAttacker->GetActorForwardVector().X * 100,
			pAttacker->GetActorLocation().Y + pAttacker->GetActorForwardVector().Y * 100);
		DamageRange::Point targetPos(pTarget->GetActorLocation().X, pTarget->GetActorLocation().Y);
		double fDis = m_DaRange.getDistance(owerPos, targetPos);
		if (fDis < nDistace) {
			arrTargets.Add(pTarget);
		}
	}
	arrTargets.Sort([this, pAttacker](const AActor& aA, const AActor& aB) {
		DamageRange::Point owerPos(pAttacker->GetActorLocation().X, pAttacker->GetActorLocation().Y);
		DamageRange::Point targetPosA(aA.GetActorLocation().X, aA.GetActorLocation().Y);
		DamageRange::Point targetPosB(aB.GetActorLocation().X, aB.GetActorLocation().Y);
		double fDisA = m_DaRange.getDistance(owerPos, targetPosA);
		double fDisB = m_DaRange.getDistance(owerPos, targetPosB);
		return fDisA < fDisB; // 升序排序
	});
}

void AMyCollision::MonsterRefocusRole(float fDetRange)
{
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}

	if (m_arryRoles.Num() <= 0)
	{
		return;
	}

	m_arryRoles.Sort([this, pAttacker, fDetRange](const AActor& aA, const AActor& aB) {
		DamageRange::Point owerPos(pAttacker->GetActorLocation().X, pAttacker->GetActorLocation().Y);
		DamageRange::Point owerPosA(pAttacker->GetActorLocation().X + pAttacker->GetActorForwardVector().X * 100,
			pAttacker->GetActorLocation().Y + pAttacker->GetActorForwardVector().Y * 100);
		DamageRange::Point targetPosA(aA.GetActorLocation().X, aA.GetActorLocation().Y);
		DamageRange::Point targetPosB(aB.GetActorLocation().X, aB.GetActorLocation().Y);
		double fDisA = m_DaRange.getDistance(owerPos, targetPosA) + m_DaRange.calTheta(owerPos, owerPosA, targetPosA) * fDetRange;
		double fDisB = m_DaRange.getDistance(owerPos, targetPosB) + m_DaRange.calTheta(owerPos, owerPosA, targetPosB) * fDetRange;

		return fDisA < fDisB;
	});
	m_aFocusRole = m_arryRoles[0];
}

void AMyCollision::CalNearstTargets()
{
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}
	auto&& monsters = AMonsterMgr::getInstance().GetMonsters();
	for (auto&& monster : monsters)
	{
		ARole* pRole = Cast<ARole>(monster.second);
		if (!pRole || m_arryRoles.Contains(pRole)) {
			continue;
		}
		FVector pos1 = pRole->GetActorLocation();
		FVector pos2 = pAttacker->GetActorLocation();
		if (FVector::Distance(pos1, pos2) > 1500.f)
		{
			continue;
		}

		AddTargetRole(pRole);
	}
}

void AMyCollision::ResortCavNextTarget()
{
	ARole* pAttacker = Cast<ARole>(m_owner);
	if (!pAttacker || pAttacker->GetObjectId() == 0)
	{
		return;
	}

	if (m_arryRoles.Num() <= 0)
	{
		return;
	}

	m_arryRoles.Sort([this, pAttacker](const AActor& aA, const AActor& aB) {
		DamageRange::Point owerPos(pAttacker->GetActorLocation().X, pAttacker->GetActorLocation().Y);
		DamageRange::Point owerPosA(pAttacker->GetActorLocation().X + pAttacker->GetActorForwardVector().X * 100,
			pAttacker->GetActorLocation().Y + pAttacker->GetActorForwardVector().Y * 100);
		DamageRange::Point targetPosA(aA.GetActorLocation().X, aA.GetActorLocation().Y);
		DamageRange::Point targetPosB(aB.GetActorLocation().X, aB.GetActorLocation().Y);
		double fDisA = m_DaRange.getDistance(owerPos, targetPosA) - 2000.f;
		double fDisB = m_DaRange.getDistance(owerPos, targetPosB) - 2000.f;

		return  fabs(fDisA) < fabs(fDisB);
	});
	m_aFocusRole = m_arryRoles[0];
}

