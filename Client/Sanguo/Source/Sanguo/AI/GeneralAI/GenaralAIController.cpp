// Fill out your copyright notice in the Description page of Project Settings.


#include "GenaralAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "config/MonsterCfg.h"
#include "../../Player/Generals.h"
#include "../../Player/Role.h"
#include "../../Scene/LevelBase.h"

//****************************************************************************************
//
//****************************************************************************************
void AGenaralAIController::OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit)
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
			}
		}
	} else {
		if (pOwnerRole->RemoveTarget(role)) {
			pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), pOwnerRole->GetFocusRole() != nullptr);
		}
	}

	UE_LOG(LogClass, Log, TEXT("ASoldierAIController SetAISight OnPerceptionUpdate"));
}

void AGenaralAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
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

		if (!role || role == pOwnerRole)
		{
			continue;
		}

		if (pOwnerRole->IsCanAttackTarget(role))
		{
			pOwnerRole->AddTarget(role);
			pBlackBoard->SetValueAsBool(GetTag(FIND_TARGET), true);

			if (!IsRoleFocus())
			{
				auto pos = pOwnerRole->GetActorLocation();
				pBlackBoard->SetValueAsVector(GetTag(BEGINFOCUSPOS), pos);
				SetRoleFocus(true);
			}
		}
	}
}

//****************************************************************************************
//
//****************************************************************************************
void AGenaralAIController::InitAI()
{
	auto pOwnerRole = GetOwnerRole();
	if (pOwnerRole) {
		ARole* pRole = Cast<ARole>(pOwnerRole);
		if (pRole && pRole->GetCurrentLevelTpy() == (int)LevelType::LevelType_MainCity) {
			SetAISight(5000, 35000, 360);
		} else {
			SetAISight(25000, 35000, 360);
		}
	} else {
		SetAISight(2500, 3500, 360);
	}
	
	
	SetAIId(1);
	StartBehaviorTree(1);
}

//****************************************************************************************
//
//****************************************************************************************
void AGenaralAIController::SetPerceptionUpdate(UAIPerceptionComponent* pPercept)
{
	if (!pPercept)
	{
		return;
	}
	pPercept->OnTargetPerceptionUpdated.AddDynamic(this, &AGenaralAIController::OnTargetPerceptionUpdate);
	//pPercept->OnPerceptionUpdated.AddDynamic(this, &AGenaralAIController::OnPerceptionUpdated);
}

