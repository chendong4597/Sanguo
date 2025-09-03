// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Role.h"
#include <map>

#include "DefenceBuilding.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API ADefenceBuilding : public ARole
{
	GENERATED_BODY()

public:
	virtual void Init() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual bool CanAttack() override;
	virtual bool PlayAttack() override;
	virtual void AttackEffect() override;
	virtual void StartAI() override;
	virtual bool IsCanAttackTarget(ARole* pRole, bool bPassive = false) override;

	UFUNCTION(BlueprintCallable, Category = "DEFENCE_BUILD")
	void StartDefence(int nDefenceId);

	int GetDefenceBuildTypeId() { return m_nDefenceId; }

	void SetUuid(NS::I64 uuid) { m_uuid = uuid; }
	NS::I64 GetUuid() { return m_uuid; }

	virtual bool HitTest() override;

public:
	void onTowerBuildEvent(const HandlerTowerBuildEvent& evt);

public:
	UPROPERTY(EditAnywhere, Category = "DEFENCE_BUILD")
	int m_buildId{};

	UPROPERTY(EditAnywhere, Category = "DEFENCE_BUILD")
	int m_buildTpy{};

	UPROPERTY(EditAnywhere, Category = "DEFENCE_BUILD")
	int m_buildNeedLev{};

private:
	float m_curTm{};
	float m_InterTm{};
	int m_nDefenceId{};
	NS::I64 m_uuid{};
};

class DefenceBuidMgr
{
public:
	bool initialize();
	void update(float dt);
	bool reset();
	void initTowerBuidings();

public:
	DefenceBuidMgr();
	~DefenceBuidMgr() {};
	static DefenceBuidMgr& getInstance();

	void InitDefeceBuilding();
	void CreateDefenceBuid(NS::I64 uuid , int nBuildId, CampState camp , int posX , int posY);
	void AddDefenceBuildByUUId(NS::I64 uuid);
	ADefenceBuilding* GetDefeceBuild(int nX, int nY);
	void RemoveDefeceBuild(int nX, int nY);
	void CreateNoneBuild(int nX, int nY, int nBuildTpy, FTransform trans);

private:
	using ADefenceBuildings = std::vector<ADefenceBuilding*>;
	ADefenceBuildings m_vecDefenceBuid;
};
