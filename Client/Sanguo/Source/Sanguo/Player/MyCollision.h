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

	//三个普通攻击计算 范围 角度范围， 矩形范围
	void CircleDamage(double R, int nSkillId = 0);
	void EffectCircleDamage(FVector pos, double R, int nSkillId = 0);
	void EffectCircleDartsDamage(FVector pos, double R, std::vector<int>& vecObjIds, int nSkillId = 0);   //回旋镖伤害计算，记录被伤害对象，不重复伤害
	void EffectCircleArrowDamage(FVector pos, double R, std::vector<int>& vecObjIds, int nSkillId = 0);   //不消失的箭计算，记录被伤害对象，不重复伤害
	void EffectCircleDistanceDamage(FVector owerPos, FVector forwardPos, double R, double Dis, int nSkillId = 0);   //正前方距离位置的范围伤害
	void EffectCircleDistanceDamageFit(FVector owerPos, FVector forwardPos, double R, double Dis, std::vector<int>& vecObjIds, int nSkillId = 0);   //正前方距离位置的范围伤害,不重复计算
	void SectorDamage(double R1, double angle, double R2, int nSkillId = 0);
	void SectorForwardDamage(FVector owerPos, FVector forwardPos, double R1, double angle, double R2, std::vector<int>& vecObjIds, int nSkillId = 0);
	void RectangleDamage(double R, double fWidth, double fHeight, int nSkillId = 0);
	void RectangleEffectDamage(FVector effectPos, FVector vecFor, double R, double fWidth, double fHeight, int nSkillId = 0);
	void SingleDamage(int nObjId, int nSkillId = 0, int nDis = 0);
	void CalMutiTargets(int nDistace, int nMaxNum, TArray<ARole*>& arrTargets);		//范围内多目标计算
	bool CheckHitTarget(FVector pos, double R);
	void CastSelfSingle(int nSkillId = 0);

	void SetOwnerRole(ARole* aOwner) { m_owner = aOwner; }


	const TArray<ARole*>& GetTargets();

	//AI有时怪物无法计算完整，找下附件怪物
	void CalNearstTargets();
	void ResortCavNextTarget();

	
private:
	TArray<ARole*> m_arryRoles;              //范围内目标
	ARole*          m_aFocusRole{};             //选中焦点 

	bool m_bInvalid{false};   //死亡时使用
	DamageRange m_DaRange;
	ARole* m_owner{nullptr};
	bool m_bInitIn{ false };  //初始化碰撞结束
};
