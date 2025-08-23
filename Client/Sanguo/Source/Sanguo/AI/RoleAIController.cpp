// Fill out your copyright notice in the Description page of Project Settings.


#include "RoleAIController.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "config/BehaviorTreeTypeCfg.h"
#include "../Player/Role.h"



void ARoleAIController::SetAISight(int nAtkSight, int nLoseSight, int nAngle)
{
	UAIPerceptionComponent* pPercept = this->GetPerceptionComponent();
	if (!pPercept)
	{
		return;
	}
	UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>(this, TEXT("Sight Config"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SightRadius = nAtkSight;
	SightConfig->LoseSightRadius = nLoseSight;
	SightConfig->PeripheralVisionAngleDegrees = nAngle;


	pPercept->SetDominantSense(SightConfig->GetSenseImplementation());
	pPercept->ConfigureSense(*SightConfig);
	

	UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
	AIPerceptionSys->RegisterSenseClass(SightConfig->GetSenseImplementation());
	const FAISenseID SenseID = UAISense::GetSenseID(SightConfig->GetSenseImplementation());
	if (!SenseID.IsValid())
	{
		return;
	}
	pPercept->UpdatePerceptionWhitelist(SenseID, true);
	AIPerceptionSys->UpdateListener(*pPercept);
}


void ARoleAIController::StartBehaviorTree(int nAI)
{
	auto&& pCfg = BehaviorTreeTypeCfg::getInstance().GetBehaviorType(nAI);
	if (!pCfg) {
		return;
	}
	//FStringAssetReference asset = FStringAssetReference(FString(pCfg->s_path.c_str()));
	//UObject* itemObj = asset.ResolveObject();
	//UBehaviorTree* ubp = Cast<UBehaviorTree>(itemObj);

	//if (!ubp)
	//{
	//	UE_LOG(LogOutputDevice, Log, TEXT("ARoleAIController::StartBehaviorTree class failed"));
	//	UE_LOG(LogTemp, Log, TEXT("ARoleAIController::StartBehaviorTree class failed"));
	//	return;
	//}
	m_bRunTree = true;
	this->BTRunBehaviorTree(0);

	ARole* pRole = Cast<ARole>(GetOwnerRole());
	if (pRole)
	{
		auto pBlackBoard = GetBlackboardComponent();
		if (pBlackBoard)
		{
			pBlackBoard->SetValueAsFloat(GetTag(ATKINTER), pRole->GetNormalAttackInterTm());
		}
	}
}

void ARoleAIController::StopBehaviorTree()
{
	StopMovement();
	UBrainComponent* comp = GetBrainComponent();
	if (!comp)
	{
		return;
	}
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(comp);
	if (BTComp == NULL)
	{
		return;
	}
	BTComp->StopTree(EBTStopMode::Safe);
}

void ARoleAIController::SetBlackBoolValue(int nTag, bool bValue)
{
	auto pOwnerRole = GetOwnerRole();
	auto pBlackBoard = GetBlackboardComponent();
	if (!pOwnerRole || !pBlackBoard)
	{
		return;
	}
	pBlackBoard->SetValueAsBool(GetTag((BLACKBOARD_TAG)nTag), bValue);
}

void ARoleAIController::OnPossess(APawn* InPawn)
{
	ARole* pRole = Cast<ARole>(InPawn);
	if (pRole)
	{
		m_pOwnerRole = pRole;
	}

	UAIPerceptionComponent* pPercept = this->GetPerceptionComponent();
	if (pPercept)
	{
		SetPerceptionUpdate(pPercept);
	}
	Super::OnPossess(InPawn);
}

FName ARoleAIController::GetTag(BLACKBOARD_TAG tag)
{
	if (tag >= TAG_NUM)
	{
		return FName(TEXT(""));
	}
	FString str(BB_Tag_Str[tag]);
	return FName(*str);
}

void ARoleAIController::RePerceptionActors()
{
	// 获取 AIPerception 组件
	UAIPerceptionComponent* AIPerceptionComponent = this->GetPerceptionComponent();

	if (AIPerceptionComponent)
	{
		// 强制重新检测所有 Actor
		AIPerceptionComponent->RequestStimuliListenerUpdate();
	}
}
