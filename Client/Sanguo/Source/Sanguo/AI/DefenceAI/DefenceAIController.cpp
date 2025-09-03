// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenceAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "config/DefenceInfoTypeCfg.h"
#include "../../Player/DefenceBuilding.h"
#include "../../Player/Role.h"

//****************************************************************************************
//
//****************************************************************************************
void ADefenceAIController::SetPerceptionUpdate(UAIPerceptionComponent* pPercept)
{
	if (!pPercept)
	{
		return;
	}
	pPercept->OnTargetPerceptionUpdated.AddDynamic(this, &ADefenceAIController::OnTargetPerceptionUpdate);
}

//****************************************************************************************
//
//****************************************************************************************
void ADefenceAIController::OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit)
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
		}
		pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), true);

		if (!IsRoleFocus())
		{
			auto pos = pOwnerRole->GetActorLocation();
			pBlackBoard->SetValueAsVector(GetTag(BEGINFOCUSPOS), pos);
			SetRoleFocus(true);
		}
	}
	else {
		pOwnerRole->RemoveTarget(role);
		if (!pOwnerRole->GetFocusRole())
		{
			pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), false);
			SetRoleFocus(false);
		}
	}
	//UE_LOG(LogClass, Log, TEXT("AMonster SetAISight OnPerceptionUpdate"));
}

//****************************************************************************************
//
//****************************************************************************************
void ADefenceAIController::Tick(float DeltaTime)
{
	if (!IsBehaviorTreeRuning())
	{
		return;
	}
}

//****************************************************************************************
//
//****************************************************************************************
void ADefenceAIController::InitAI()
{
	ADefenceBuilding* defenceBuild = Cast<ADefenceBuilding>(GetOwnerRole());
	if (!defenceBuild)
	{
		return;
	}

	auto&& pDefenceCfg = DefenceInfoTypeCfg::getInstance().GetDefenceById(defenceBuild->GetDefenceBuildTypeId());
	if (!pDefenceCfg)
	{
		return;
	}

	SetAISight(pDefenceCfg->n_AtkSight, pDefenceCfg->n_LoseSight, pDefenceCfg->n_AtkAngle);
	SetAIId(pDefenceCfg->n_AI);
	StartBehaviorTree(pDefenceCfg->n_AI);
}