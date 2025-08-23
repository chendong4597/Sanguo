// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "config/MonsterCfg.h"
#include "../../Player/Soldier.h"
#include "../../Player/Role.h"

//****************************************************************************************
//
//****************************************************************************************
void ASoldierAIController::OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit)
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
	}

	UE_LOG(LogClass, Log, TEXT("ASoldierAIController SetAISight OnPerceptionUpdate"));
}

//****************************************************************************************
//
//****************************************************************************************
void ASoldierAIController::InitAI()
{
	SetAISight(3000, 20000, 360);
	SetAIId(1);
	StartBehaviorTree(1);
}

//****************************************************************************************
//
//****************************************************************************************
void ASoldierAIController::SetPerceptionUpdate(UAIPerceptionComponent* pPercept)
{
	if (!pPercept)
	{
		return;
	}
	pPercept->OnTargetPerceptionUpdated.AddDynamic(this, &ASoldierAIController::OnTargetPerceptionUpdate);
}

