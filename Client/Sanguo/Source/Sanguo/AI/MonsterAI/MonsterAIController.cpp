// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "config/MonsterCfg.h"
#include "../../Player/Monster.h"
#include "../../Player/Role.h"

//****************************************************************************************
//
//****************************************************************************************
void AMonsterAIController::SetPerceptionUpdate(UAIPerceptionComponent* pPercept)
{
	if (!pPercept)
	{
		return;
	}
	pPercept->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnTargetPerceptionUpdate);
	//pPercept->OnPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnPerceptionUpdated);
}

void AMonsterAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	auto pOwnerRole = GetOwnerRole();
	auto pBlackBoard = GetBlackboardComponent();
	if (!pOwnerRole || !pBlackBoard)
	{
		return;
	}

	for (AActor* Actor : UpdatedActors)
	{
		ARole* role = Cast<ARole>(Actor);
		if (!role)
		{
			continue;
		}

		if (pOwnerRole->IsCanAttackTarget(role))
		{
			pOwnerRole->AddTarget(role);
			auto monster = Cast<AMonster>(pOwnerRole);
			if (monster) {
				monster->ResetWalkSpeed();
			}

			if (pOwnerRole->GetFocusRole() == nullptr) {
				pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), false);
			}
			else {
				pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), true);
			}
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AMonsterAIController::OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit)
{
	if (!Ator)
	{
		return;
	}
	auto pOwnerRole = GetOwnerRole();
	auto pBlackBoard = GetBlackboardComponent();
	ARole* role = Cast<ARole>(Ator);
	if (!role || !pOwnerRole || !pBlackBoard)
	{
		return;
	}
	if (sit.WasSuccessfullySensed())
	{
		if (pOwnerRole->IsCanAttackTarget(role))
		{
			pOwnerRole->AddTarget(role);
			pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), true);

			if (!IsRoleFocus())
			{
				auto pos = pOwnerRole->GetActorLocation();
				pBlackBoard->SetValueAsVector(GetTag(BEGINFOCUSPOS), pos);
				SetRoleFocus(true);
				m_vecFocusPos = pos;
			}
		}
	}
	else {
		AMonster* monster = Cast<AMonster>(pOwnerRole);
		if (monster && monster->m_pMonsterData && monster->m_pMonsterData->getMonsterType() &&
			monster->m_pMonsterData->getMonsterType()->n_nTowerTpy == MonsterCfg::MonsterType::MobTowerTypeDefTower &&
			monster->IsCanAttackTarget(role))
		{
			monster->RemoveTarget(role);
			if (!monster->GetFocusRole())
			{
				pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), false);
				//SetFocus(false);
			}
		}
	}

	auto monster = Cast<AMonster>(pOwnerRole);
	if (monster) {
		monster->ResetWalkSpeed();
	}

	if (pOwnerRole->GetFocusRole() == nullptr) {
		pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), false);
	}
	else {
		pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), true);
	}

	//UE_LOG(LogClass, Log, TEXT("AMonster SetAISight OnPerceptionUpdate"));
}

//****************************************************************************************
//
//****************************************************************************************
void AMonsterAIController::Tick(float DeltaTime)
{
	if (!IsBehaviorTreeRuning())
	{
		return;
	}
	if (IsRoleFocus() && GetFocusActor())
	{
		float fDis = FVector::Distance(GetFocusActor()->GetActorLocation(), m_vecFocusPos);
		if (fDis < m_nLoseDis)
		{
			return;
		}
		auto pBlackBoard = GetBlackboardComponent();
		if (!pBlackBoard)
		{
			return;
		}
		pBlackBoard->SetValueAsBool(GetTag(BTOOFAR), true);
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AMonsterAIController::InitAI()
{
	AMonster* monster = Cast<AMonster>(GetOwnerRole());
	if (!monster)
	{
		return;
	}

	float fAtkSight = 1000.f;
	float fLoseSight = 2000.f;
	float fAtkAngle = 360.f;
	monster->GetAight(fAtkSight, fLoseSight, fAtkAngle);
	
	SetAISight(fAtkSight, fLoseSight, fAtkAngle);
	m_nLoseDis = fLoseSight;

	auto&& pMonCfg = MonsterCfg::getInstance().findMonsterType(monster->GetMonsterId());
	if (!pMonCfg)
	{
		return;
	}

	SetAIId(pMonCfg->n_AI);
	StartBehaviorTree(pMonCfg->n_AI);
}


//****************************************************************************************
//
//****************************************************************************************
void AMonsterAIController::BackToBornPosition()
{
	auto owenRole = GetOwnerRole();
	if (!owenRole)
	{
		return;
	}
	SetRoleFocus(false);
	auto monster = Cast<AMonster>(owenRole);
	if (!monster)
	{
		return;
	}
	monster->SetReturnBorn(true);
}

void AMonsterAIController::EndToBornPosition()
{
	auto monster = Cast<AMonster>(GetFocusActor());
	if (!monster)
	{
		return;
	}
	auto pBlackBoard = GetBlackboardComponent();
	if (!pBlackBoard || !monster)
	{
		return;
	}
	pBlackBoard->SetValueAsBool(GetTag(BTOOFAR), false);
	monster->SetReturnBorn(false);
}

