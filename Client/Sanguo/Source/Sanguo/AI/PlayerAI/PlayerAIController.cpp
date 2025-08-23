// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "config/MonsterCfg.h"
#include "../../Player/IPlayer.h"
#include "../../Player/Hero.h"
#include "../../Player/Role.h"

//****************************************************************************************
//
//****************************************************************************************
void APlayerAIController::SetPerceptionUpdate(UAIPerceptionComponent* pPercept)
{
	if (!pPercept)
	{
		return;
	}
	pPercept->OnTargetPerceptionUpdated.AddDynamic(this, &APlayerAIController::OnTargetPerceptionUpdate);
}

//****************************************************************************************
//
//****************************************************************************************
void APlayerAIController::OnTargetPerceptionUpdate(AActor* Ator, FAIStimulus sit)
{
	if (!Ator)
	{
		return;
	}
	auto pOwnerRole = GetOwnerRole();
	AIPlayer* pTargetPlayer = Cast<AIPlayer>(Ator);
	if (!pTargetPlayer)
	{
		return;
	}
	AIPlayer* pPlayer = Cast<AIPlayer>(pOwnerRole);
	if (!pPlayer) {
		return;
	}

	AIPlayer* pHero = Cast<AIPlayer>(AIPlayerMgr::getInstance().GetHero());

	if (!GameAppExtern::IsServer() || pPlayer == pHero || pTargetPlayer == pHero || pPlayer == pTargetPlayer)
	{
		return;
	}

	if (sit.WasSuccessfullySensed()) {
		pPlayer->SendInPlayerSight(pTargetPlayer->m_playerData->getIggId());
	}
	else if (sit.SensingFailed) {

	}

	UE_LOG(LogClass, Log, TEXT("APlayer SetAISight OnPerceptionUpdate"));
}

//****************************************************************************************
//
//****************************************************************************************
void APlayerAIController::Tick(float DeltaTime)
{
	if (!IsBehaviorTreeRuning())
	{
		return;
	}
}

//****************************************************************************************
//
//****************************************************************************************
void APlayerAIController::InitAI()
{
	AIPlayer* monster = Cast<AIPlayer>(GetOwnerRole());
	if (!monster)
	{
		return;
	}

	SetAISight(1000, 1500, 360);
	//SetAIId(1000);
	//StartBehaviorTree(1000);
}
