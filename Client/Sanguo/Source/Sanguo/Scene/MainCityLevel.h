// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelBase.h"
#include "GameAppExtern.h"
#include "MainCityLevel.generated.h"



/**
 * 
 */
UCLASS()
class SANGUO_API AMainCityLevel : public ALevelBase
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void onHouseEvent(const HouseEvent& evt);
	void onHeroInitEvent(const HeroInitEvent& evt);
	virtual void PreUnload() override;
	virtual void TransLoadState(int nLoadState) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FPrimaryAssetId GetPrimaryAssetId() const
	{
		//AssetType��FPrimaryAssetType���͵ı�����Ӧ����ζ��壬�������½�˵��
		return FPrimaryAssetId("TMap", GetFName());
	}

public:
	void onTowerBuildEvent(const HandlerTowerBuildEvent& evt);
	void onPlantsEvent(const HandlerPlantEvent& evt);
	void onHandlerHouseEvent(const HandlerHouseEvent& evt);
	void onHandlerRoleEvent(const HandlerRoleEvent& evt);
private:
	std::map<NS::I64, FTransform> m_mapNpcTempTrans;
private:
	std::map<NS::I64, FTransform> m_mapGenTempTrans;
private:
	float m_fWallLeftTm{ 0.f };
	float m_fWallAllTm{ 0.f };

	std::vector<FTransform> m_vecWallWorkFlag;  //������ǽ�ص�
	std::vector<FTransform> m_vecCenterWorkFlag;  //�����޼���ص�
	std::vector<FTransform> m_vecRoadWorkFlag;  //�����޼���ص�
	std::vector<FTransform> m_vecHouseWork;  //���������ص� ���ɵķ���
	std::vector<FTransform> m_vecWork;  //���������ص� ���ɵķ���

public:
	struct BATTLEPOS {
		int nMinLv{};
		int nMaxLv{};
		FVector pos;
	};

	FVector GetBattlePos(int nLv);
	bool IsSameBattleLevel(int nLev1, int nLev2);
private:
	std::vector<BATTLEPOS> m_vecBattlePos;
};
