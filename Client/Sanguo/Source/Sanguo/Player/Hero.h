// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BluePrint/UserWidget.h"

//#include "NetworkModule.h"
#include "../Common/UIEvents.h"
#include "IPlayer.h"
#include "../Common/CommonDefines.h"
#include "GameAppExtern.h"
#include "Hero.generated.h"

struct TouchHis
{
	bool bTouch{};
	FVector2D vec;
	ENU_TOUCH_UITPY nTouchUITpy{};
};


enum TouchSightTpy {
	TouchSightTpy_L = 0,  //固定左边视角
	TouchSightTpy_R,  //固定右边视角
	TouchSightTpy_B,  //固定背后视角
	TouchSightTpy_F, //固定前方视角
	TouchSightTpy_A  //随意视角
};

enum BattleGenBuff {
	BattleGenBuff_None = 0,
	BattleGenBuff_Inf = 0x01,  //步兵开战buff
	BattleGenBuff_Cav = 0x02,  //骑兵开战buff
	BattleGenBuff_Arc = 0x04,  //弓兵开战buff
	BattleGenBuff_Bla = 0x08,  //大刀兵开战buff
	BattleGenBuff_Mag = 0x16,  //法术兵开战buff
	BattleGenBuff_Cro = 0x32,  //弩兵开战buff
	BattleGenBuff_Doc = 0x64,  //治疗兵开战buff
};

UCLASS()
class SANGUO_API AHero : public AIPlayer
{
	GENERATED_BODY()
	//DECLARE_EVENT_HOST();
public:
	// Sets default values for this character's properties
	AHero();
	virtual void Init();

	void StopHoverAttack(bool bRelease = true);
	void SetHeroCamera();
	void SetCityCamera(AActor* aCamera) { m_pCityCamera = aCamera; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	void onItemEvent(const HandlerItemEvent& evt);
	void onHandlerTipsEvent(const HandlerTipsEvent& evt);
	void onHandlerItemGetEvent(const HandlerItemGetEvent& evt);
	void onHandlerNetEvent(const HandlerNetEvent& evt);
	void onHandlerHeroAppearanceEvent(const HandlerHeroAppearanceEvent& evt);
	void onHandlerHeroAttrChangeEvent(const HandlerHeroAttrChangeEvent& evt);
	void onHandlerSoundEvent(const HandlerSoundEvent& evt);
	void onMsgDeleteAccountS2C(const MsgDeleteAccountS2C& msg);
protected:
	void onWalkEvent(const BallEvent& evt);
	void onSightEvent(const SightEvent& evt);
	void onAttackEvent(const AttackEvent& evt);
	void onWeaponEvent(const WeaponEvent& evt);
	void onWindowEvent(const WindowEvent& evt);
	void onProfessEvent(const ProfessEvent& evt);
	void onSkillEvent(const SkillEvent& evt);
	void onBuildEvent(const BuildEvent& evt);
	void onHorseEvent(const HorseEvent& evt);
	void onHorseSkillEvent(const HorseSkillEvent& evt);
	void onStartContinueAttackEvent(const StartContinueAttackEvent& evt);
	void onStopContinueAttackEvent(const StopContinueAttackEvent& evt);
	void onHoveAttackEvent(const HoverAttackEvent& evt);
	void onUseWeaponExternEvent(const UseWeaponExternEvent& evt);
	void onUITapLoginEvent(const UITapLoginEvent& evt);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "HERO")
	void Walk(FVector vec, float fScale = 1.0f);

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void Sight(int sightValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void Round(bool bLeft , float fDelTm);

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void UpAndDown(bool bUp, float fDelTm);

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void Attack();

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void Dodge();

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void BPWalk(FVector vec);

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void SaveCombo();

	UFUNCTION(BlueprintCallable, Category = "HERO")
	void SetHeroFocusRole(ARole* aRole);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	FVector2D GetTouchPosition(int nUITpy);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void SetTouchSightRecord(int nSight);  //视野返回，记录保存

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void HeroInputTouch(const ETouchIndex::Type TouchIndex, ETouchType::Type Type, FVector vecPos);


	//和NPC对话或者打开人物界面时，镜头切换
	void ResetCamera();

	//Hud显示通知
	void PostHudShow();

	//旋转人物，放大缩小视野等功能
	UFUNCTION(BlueprintCallable, Category = "HERO")
	void SightAround(ETouchIndex::Type TouchIndex , FVector2D vec, bool bPressed);

	float GetTouchDistance();
	void Around(float DeltaTime);
	void CityAround(float DeltaTime);
	void PostUITouchEvent(float X, float Y, int touchTpy);

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void SaveHeroCameraInfo();

	void RefreshEquipAttr();

public:
	void InitPreEquipWeapons();
	void SavePreEquipWeapons();
	void SetEquipWeapon(int nPos, NS::I64 uuid, bool bEquip = true);
	NS::I64 GetEquipWeapon(int nPos);
public:
	void GetNearstGens(NS::I64 uuid, std::vector<NS::I64>& vecUuids);
public:
	void InitCurrencyItems();
	void SaveCurrencyItems();
	void SetCurrencyItems(int nItemId, bool bEquip = true);
	bool IsCurrencyItems(int nItemId);
	const std::vector<int>& GetCurrencyItems(){ return m_vecSaveMat; }
private:
	std::vector<int> m_vecSaveMat;
public:
	void HeroRideHorse(bool bRide);
public:
	void LockFocusObject();
	void ResetFocusObject();
private:
	FString m_strLockObject;
	float m_fLockLessTm{0.f};
public:
	void LoadSightInfo();
	void SaveSightInfo();
	void ResetTouchSight(int nSightTpy = -1, bool bSetSight = true);
	int GetTouchSight() const { return m_nTouchSightTpy; }
public:
	FString ReadHeroCfgFile(FString strFile);
	void SaveHeroCfgFile(FString strFile, FString strData);

	void TranslateToBorth();
	void TranslateEnd();
	void ResetWaitTrans();
public:
	virtual void Death() override;
	virtual void Revive() override;
	virtual void BeAttackAnger(int nDamage) override;
public:
	enum {
		ATK_TPY_SKILL_NORMAL = 0, //基础攻击
		ATK_TPY_SKILL_EXT, //额外技能
		ATK_TPY_SKILL_COMBO, //连招
		ATK_TPY_SKILL_PERSKILL, //前置技能，释放靠前
	};
	//nAtkTpyAtk
	bool CheckSkillConsume(int nSkillId = 0, int nAtkTpySkill = 0, bool bTips = false);
	bool SkillConsume(int nHp, int nMp, int nStrenth, int nAnger);
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void EvtPreGetGenByff();

	UFUNCTION(BlueprintImplementableEvent, Category = "HERO")
	void EvtSetGenByff(int nBuff);

	UFUNCTION(BlueprintCallable, Category = "HERO")
	void SetBattleGenBuff(int nBattleBuf);

	void SetBattleGenBuffInx(int nIdx, bool bSet);

	int  GetBattleGenBuff();

	bool IsGenBuffState(int nState);

	UFUNCTION(BlueprintCallable, Category = "HERO")
	void SetAutoMediaPercent(int nTpy, int nPerent);

private:
	bool m_bGuidNav{ false };
	TArray<FVector> m_PathPoints;
	int  m_achieveId { 0 };
	int  m_nAchieveGuidIdx{ 0 };
	bool m_bPreAchieve{ false };
	NS::I64 m_nGuidProterUuid{ 0 };
	int  m_nGuidHouseId{ 0 };
private:
	int m_nBattleGenBuf{ (1 << 7) - 1};

private:
	int m_nAutoLifePer{ 0 };
	int m_nAutoManaPer{ 0 };
	int m_nAutoStrePer{ 0 };
	int m_nAutoBreaPer{ 0 };

private:
	float m_fFoodDec{ 0.f };  //记录玩家跑动时。每秒体力消耗
	float m_fFoodInc{ 0.f };  //记录玩家停止时。每秒体力回复
	float m_fBreathInc{ 0.f };  //平静值回复，怒气值减少
	float m_fAIReSimInc{ 0.f };  //AI检测
private:
	
	//是否在操作选择位置或者方向
	bool m_bScrollHit{false};
	int  m_nScrollR{ 0 }; //选择位置的最大距离

	AActor* m_GroundActor{ nullptr };  //技能位置选择
	AActor* m_ArrowActor{ nullptr };   //技能方向选中

	AActor* m_pBuildLand{ nullptr };   //防御塔显示网格
	AActor* m_pCityCamera{ nullptr };  //布放建筑时的相机

	FRotator m_preSpringRota;  //保存摄像头旋转位置
	int     m_nPreSpringLen;  //保存摄像头Sping长度
	int     m_nTouchSightTpy{ TouchSightTpy_L }; //固定左边视角

	FVector2D m_BeginRoundVec;  //旋转镜头参数设置
	FVector2D m_MoveRoundVec;  //旋转镜头参数设置
	bool m_bAround{ false };  //旋转镜头参数设置
	bool m_bPlantMove{ false }; //是否是种植模式下的移动
	int m_nProfessionIdx{ 0 };

	bool m_bWaitTrans{ false };	//是否在等待返回起点位置
	FTimerHandle MyTimerHand;
	

	std::map<ETouchIndex::Type , TouchHis> m_mapTouchHis;
	float    m_fTouchDistance;
	float    m_fPreSightValue;
	float    m_fPreSightValueRecord;
	std::map<int, int64> m_mapEquipWeapon;		//穿戴的武器
};

class AIPlayerMgr// : public bzbee::MsgHandler
{
	//DECLARE_EVENT_HOST();
public:
	bool initialize();
	void terminate();
	void PreReset();
	bool reset();
	void update(float dt);

public:
	AHero* GetHero();
	void SetHero(AHero* hero);
	bool AddPlayer(AIPlayer* pPlayer);
	bool RemovePlayer(int objId);
	AIPlayer* GetPlayer(int objId);

public:
	AIPlayerMgr();
	~AIPlayerMgr() {};
	static AIPlayerMgr& getInstance();

private:
	using MapAIPlayer = std::map<int, AIPlayer*>;
	MapAIPlayer m_players;

	AHero* m_Hero{ nullptr };
};
