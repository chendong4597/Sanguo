// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Common/CommonDefines.h"
#include "MyCollision.generated.h"

class ARole;

UCLASS()
class SANGUO_API AMyCollision : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCollision();

	UFUNCTION()
	virtual void OnOverlayBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnOverlayEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "MYCOLLISION")
	void Sight(float fSight);

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void ScaleEnd();

	void Reset();
	bool RemoveId(int nId);
	bool AddTargetId(int nId, bool bPassive = false);
	void AddTargetRole(ARole* pRole, bool bPassive = false);
	void RemoveTargetRole(ARole* pRole);
	bool HasTargetRole(ARole* pRole);
	void CalFocusRole(FString name);
	void MonsterRefocusRole(float fDetRange = 200.f);
	ARole* GetFocusRole() {
		return m_aFocusRole;
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//������ͨ�������� ��Χ �Ƕȷ�Χ�� ���η�Χ
	void CircleDamage(double R, int nSkillId = 0);
	void EffectCircleDamage(FVector pos, double R, int nSkillId = 0);
	void EffectCircleDartsDamage(FVector pos, double R, std::vector<int>& vecObjIds, int nSkillId = 0);   //�������˺����㣬��¼���˺����󣬲��ظ��˺�
	void EffectCircleArrowDamage(FVector pos, double R, std::vector<int>& vecObjIds, int nSkillId = 0);   //����ʧ�ļ����㣬��¼���˺����󣬲��ظ��˺�
	void EffectCircleDistanceDamage(FVector owerPos, FVector forwardPos, double R, double Dis, int nSkillId = 0);   //��ǰ������λ�õķ�Χ�˺�
	void EffectCircleDistanceDamageFit(FVector owerPos, FVector forwardPos, double R, double Dis, std::vector<int>& vecObjIds, int nSkillId = 0);   //��ǰ������λ�õķ�Χ�˺�,���ظ�����
	void SectorDamage(double R1, double angle, double R2, int nSkillId = 0);
	void SectorForwardDamage(FVector owerPos, FVector forwardPos, double R1, double angle, double R2, std::vector<int>& vecObjIds, int nSkillId = 0);
	void RectangleDamage(double R, double fWidth, double fHeight, int nSkillId = 0);
	void RectangleEffectDamage(FVector effectPos, FVector vecFor, double R, double fWidth, double fHeight, int nSkillId = 0);
	void SingleDamage(int nObjId, int nSkillId = 0, int nDis = 0);
	void CalMutiTargets(int nDistace, int nMaxNum, TArray<ARole*>& arrTargets);		//��Χ�ڶ�Ŀ�����
	bool CheckHitTarget(FVector pos, double R);
	void CastSelfSingle(int nSkillId = 0);

	void SetOwnerRole(ARole* aOwner) { m_owner = aOwner; }


	const TArray<ARole*>& GetTargets();

	//AI��ʱ�����޷��������������¸�������
	void CalNearstTargets();
	void ResortCavNextTarget();

	
private:
	TArray<ARole*> m_arryRoles;              //��Χ��Ŀ��
	ARole*          m_aFocusRole{};             //ѡ�н��� 

	bool m_bInvalid{false};   //����ʱʹ��
	DamageRange m_DaRange;
	ARole* m_owner{nullptr};
	bool m_bInitIn{ false };  //��ʼ����ײ����
};
