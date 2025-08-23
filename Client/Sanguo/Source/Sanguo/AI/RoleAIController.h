// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"

#include "AIController.h"
#include "RoleAIController.generated.h"

/**
 * 
 */



class ARole;

enum BLACKBOARD_TAG { FIND_TARGET = 0, BEGINFOCUSPOS, BTOOFAR, WALKWAIT, ATKINTER, ISDEAD, TAG_NUM };



UCLASS()
class SANGUO_API ARoleAIController : public AAIController
{
	GENERATED_BODY()

	char BB_Tag_Str[TAG_NUM][32] = {
	"FindTarget",       //�Ƿ�鵽�ɹ���Ŀ��
	"FocusBeginPos",    //���㿪ʼλ��
	"bTooFar",          //�Ƿ��������̫Զ
	"IsWait",           //Npc�ڵȴ���
	"AtkInter",           //�������ʱ��
	"IsDead",           //�Ƿ�������
	};
public:
	UFUNCTION(BlueprintCallable, Category = "AI_ROLE")
	virtual void InitAI() {}
	void SetAISight(int nAtkSight , int nLoseSight , int nAngle);

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void BTRunBehaviorTree(int nTreeTpy);

	void StartBehaviorTree(int nAI);
	virtual void SetPerceptionUpdate(UAIPerceptionComponent* pPercept) {}

	void StopBehaviorTree();

	UFUNCTION(BlueprintCallable, Category = "AI_ROLE")
	bool IsBehaviorTreeRuning() { return m_bRunTree;}

	UFUNCTION(BlueprintCallable, Category = "AI_ROLE")
	bool IsRoleFocus() { return m_bFocus; }

	UFUNCTION(BlueprintCallable, Category = "AI_ROLE")
	void SetRoleFocus(bool bFocus) { m_bFocus = bFocus; }

	UFUNCTION(BlueprintCallable, Category = "AI_ROLE")
	ARole* GetOwnerRole() { return m_pOwnerRole; }

	virtual void SetAI(int nId) { m_nAI = nId; }

	void SetAIId(int nAIId) { m_nAI = nAIId; }

	FName GetTag(BLACKBOARD_TAG tag);

	virtual void OnPossess(APawn* InPawn) override;

	void SetBlackBoolValue(int nTag, bool bValue);

	//���¼��Actor
	void RePerceptionActors();

private:
	bool m_bRunTree{};   //�Ƿ�������Ϊ��
	ARole* m_pOwnerRole{};  //������AI
	bool m_bFocus{};        //�Ƿ�۽����
	int  m_nAI{};
};
