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
	TouchSightTpy_L = 0,  //�̶�����ӽ�
	TouchSightTpy_R,  //�̶��ұ��ӽ�
	TouchSightTpy_B,  //�̶������ӽ�
	TouchSightTpy_F, //�̶�ǰ���ӽ�
	TouchSightTpy_A  //�����ӽ�
};

enum BattleGenBuff {
	BattleGenBuff_None = 0,
	BattleGenBuff_Inf = 0x01,  //������սbuff
	BattleGenBuff_Cav = 0x02,  //�����սbuff
	BattleGenBuff_Arc = 0x04,  //������սbuff
	BattleGenBuff_Bla = 0x08,  //�󵶱���սbuff
	BattleGenBuff_Mag = 0x16,  //��������սbuff
	BattleGenBuff_Cro = 0x32,  //�����սbuff
	BattleGenBuff_Doc = 0x64,  //���Ʊ���սbuff
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
	void SetTouchSightRecord(int nSight);  //��Ұ���أ���¼����

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void HeroInputTouch(const ETouchIndex::Type TouchIndex, ETouchType::Type Type, FVector vecPos);


	//��NPC�Ի����ߴ��������ʱ����ͷ�л�
	void ResetCamera();

	//Hud��ʾ֪ͨ
	void PostHudShow();

	//��ת����Ŵ���С��Ұ�ȹ���
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
		ATK_TPY_SKILL_NORMAL = 0, //��������
		ATK_TPY_SKILL_EXT, //���⼼��
		ATK_TPY_SKILL_COMBO, //����
		ATK_TPY_SKILL_PERSKILL, //ǰ�ü��ܣ��ͷſ�ǰ
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
	float m_fFoodDec{ 0.f };  //��¼����ܶ�ʱ��ÿ����������
	float m_fFoodInc{ 0.f };  //��¼���ֹͣʱ��ÿ�������ظ�
	float m_fBreathInc{ 0.f };  //ƽ��ֵ�ظ���ŭ��ֵ����
	float m_fAIReSimInc{ 0.f };  //AI���
private:
	
	//�Ƿ��ڲ���ѡ��λ�û��߷���
	bool m_bScrollHit{false};
	int  m_nScrollR{ 0 }; //ѡ��λ�õ�������

	AActor* m_GroundActor{ nullptr };  //����λ��ѡ��
	AActor* m_ArrowActor{ nullptr };   //���ܷ���ѡ��

	AActor* m_pBuildLand{ nullptr };   //��������ʾ����
	AActor* m_pCityCamera{ nullptr };  //���Ž���ʱ�����

	FRotator m_preSpringRota;  //��������ͷ��תλ��
	int     m_nPreSpringLen;  //��������ͷSping����
	int     m_nTouchSightTpy{ TouchSightTpy_L }; //�̶�����ӽ�

	FVector2D m_BeginRoundVec;  //��ת��ͷ��������
	FVector2D m_MoveRoundVec;  //��ת��ͷ��������
	bool m_bAround{ false };  //��ת��ͷ��������
	bool m_bPlantMove{ false }; //�Ƿ�����ֲģʽ�µ��ƶ�
	int m_nProfessionIdx{ 0 };

	bool m_bWaitTrans{ false };	//�Ƿ��ڵȴ��������λ��
	FTimerHandle MyTimerHand;
	

	std::map<ETouchIndex::Type , TouchHis> m_mapTouchHis;
	float    m_fTouchDistance;
	float    m_fPreSightValue;
	float    m_fPreSightValueRecord;
	std::map<int, int64> m_mapEquipWeapon;		//����������
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
