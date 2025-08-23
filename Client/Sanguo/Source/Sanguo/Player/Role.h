// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "../Common/UIEvents.h"
#include "../AI/RoleAIController.h"
#include "MyCollision.h"
#include <vector>
#include <map>
#include "Role.generated.h"


using namespace std;


UENUM(BlueprintType)
enum class CampState :uint8 //设置uint8类型
{
	CAMP_HERO UMETA(DisplayName = "camp hero"),
	CAMP_MONSTER UMETA(DisplayName = "Camp Monster"),
	CAMP_GEN_GUARD UMETA(DisplayName = "Camp Gen Guard"),
	CAMP_SOL_GUARD UMETA(DisplayName = "Camp Sol Guard"),
	CAMP_MIDDLE UMETA(DisplayName = "Camp Middle"),
	CAMP_ALL UMETA(DisplayName = "Camp All"),
	CAMP_OthPlayer UMETA(DisplayName = "Camp OtherPlayer"),
};

UCLASS()
class SANGUO_API ARole : public ACharacter
{
	GENERATED_BODY()
	//DECLARE_EVENT_HOST();
public:
	// Sets default values for this character's properties
	ARole();

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void Init();

	virtual void Reset();
	virtual void DestroyRole();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "ROLE")
	void ResetHud();

	//击退
	UFUNCTION(BlueprintImplementableEvent, Category = "ROLE")
	void BeatBack(float nPosX, float nPosY);

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetFreeze(bool bFreeze);

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool IsFreeze() const;

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetDizziess(bool bDizziess);

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool IsDizziess() const;

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	float GetSpeedRate() const;

	virtual void SetMaterialScalerParamer(FName name , float fValue);
	virtual void SetMaterialVectorParamer(FName name, FLinearColor vecParamer);

	void SetAIMoveAvodiance(bool bAvo, float fDis);

public:
	//阵营
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetCamp(CampState camp) { m_campState = camp; }

	//判断当前场景是否是主城
	UFUNCTION(BlueprintCallable, Category = "Generals")
	bool IsInMainCity();

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	CampState GetCamp() { return m_campState; }

	//是否是可攻击的目标
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsCanAttackTarget(ARole* pRole, bool bPassive = false) { return false; }

	//是否是可被我方攻击的目标
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsCanBeAttackTarget(ARole* pRole) { return true; }

	//获取受击位置
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	FTransform GetHitWorldTransform();

	//设置战斗状态
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool GetIsFighting();

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetIsFighting(bool bFighting);

	//死亡
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void Death();

	//复活
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void Revive();

	//是否已经死亡
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsDeath() { return false; }

	//伤害
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void Damage(int castId, int nHurtDamage, bool bCrit, bool bDot = false);

	//是否攻击状态
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool IsAttacking() { return m_bAttack; }

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetAttacking(bool bAttack) { m_bAttack = bAttack; }

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void AttackEffect() {};

	//获取攻击间隔时间
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual float GetNormalAttackInterTm() { return 1.5f; };

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsCanAttack() { return false; }

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool PlayAttack() { return false; };

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void PlayDeath() {};

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool CanActiveAttack() { return true; }   //是否会主动攻击

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsForbidAttack() { return false; }   //是否不能被攻击

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsOnlyBeNormalAtk() { return false; }   //是否只能被普通攻击伤害

public:
	virtual std::string GetHitEffectPath() { return ""; } //获取攻击者的击中目标特效
	virtual void HitEffect(const char* pszEffect);

public:
	//焦点目标
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetFocusRole(ARole* aRole);

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	ARole* GetFocusRole();

	//显示边框
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void ShowHighLightMesh(bool bShow);

	//设置播放技能中,不能移动
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetAttackStopMove(bool bStopMove) { m_bAttackStopMove = bStopMove; }

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool GetAttackStopMove() { return m_bAttackStopMove; }

	//是否可以移动 状态影响
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool CanMove();

	//是否可以攻击 状态影响
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool CanAttack();

	//是否可以使用技能 状态影响
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool CanSkillAttack();

	//增加BUff
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void AddBuff(int nBuffId);
	void AddBuffEff(int nBuffId , AActor* eff);
	virtual void ClearBuff();

	//删除BUff
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void RemoveBuff(int nBuffId);

	const std::map<int, AActor*>& GetBuffs() { return m_mapBuffEffects; }

	//播放被攻击特效
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void PlayHit() {};

	//设置被攻击态势
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetHitPlaying(bool bHit) { m_bHitPlaying = bHit; }

	//是否在被攻击态势
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool IsHitPlaying() { return m_bHitPlaying; }

	//是否开始AI
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void StartAI() {}

	//获取当前地图类型
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	int GetCurrentLevelTpy();

	//
	virtual FTransform GetRoleTransByName(const char* pszSockName);
	virtual FTransform GetWeaponTransByName(FString strSockName);

	virtual FVector GetMagicBeginLocationByName(const char* pszSockName) {return GetRoleTransByName(pszSockName).GetLocation();}

	//玩家点击中回应
	virtual bool HitTest() { return false; }

	//获取武器拖尾特效地址
	virtual FString GetTrailParticlePath() {return FString("");}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	void RegisterRoleClick();

	//是否在摄像机范围内
	bool IsInCameraView();

	//物体点中
	UFUNCTION()
	void RoleClicked();

	UFUNCTION()
	void RoleTouchBegin();

	UFUNCTION()
	void RoleTouchEnd();

	void CreateMyCollision(int nR);

	AMyCollision* GetDamageCollision() { return	m_aRangeCollion; }

	ARoleAIController* GetRoleController() { return m_pAIControll; }

	void DestroyAttach();

public:
	void LookHero();
	void ResetLookForward();
	void LookAtTarget();

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool IsTalking();
private:
	FRotator m_rotOri;
	bool m_bTalking{ false };
public:
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void AddTarget(ARole* pRole, bool bFocus = false);

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool HasTarget(ARole* pRole);

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool RemoveTarget(ARole* pRole);

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool ClearTargets();


	UFUNCTION(BlueprintCallable, Category = "ROLE")
	int GetAttackDistance() { if (m_nAttackDis < 100.f) return 100.f; return m_nAttackDis; }

	//怪物失去目标后，快速返回
	virtual void StateHome() {};

	void SetAttackDistance(int nAtkDis) { m_nAttackDis = nAtkDis; }

	void AddSkillTime(int nSkillId, float tm);
	bool IsSkillCoolTime(int nSkillId) { return m_mapSkillCoolTime.find(nSkillId) != m_mapSkillCoolTime.end(); }
	float GetSkillCoolTime(int nSkillId);

	virtual std::string GetBattleSound(int nSoundTpy) { return ""; }

protected:
	void onDeathEvent(const DeathEvent& evt);
	void onBeatBackEvent(const BeatBackEvent& evt);

	void onHandlerRoleBuffEvent(const HandlerRoleBuffEvent& evt);
	void onHandlerRoleStatusEvent(const HandlerRoleStatusEvent& evt);

	void onMsgMapObjectUnderAttackG2C(const MsgMapObjectUnderAttackG2C& msg);
	void onMsgSkillEffectDotDmgMS2C(const MsgSkillEffectDotDmgMS2C& msg);
	void onMsgPlayerNormalAttackResS2C(const MsgPlayerNormalAttackResS2C& msg);
	void onMsgSkillEffectDmgMS2C(const MsgSkillEffectDmgMS2C& msg);
	void onMsgCharacterDeadS2C(const MsgCharacterDeadS2C& msg);
	void onMsgSkillEffectRestoreMS2C(const MsgSkillEffectRestoreMS2C& msg);
public:
	virtual int GetObjectId() { return 0; }
	virtual int GetRoleCtrlStatus() { return 0; }
	virtual int GetHp() { return 0; }
	virtual int GetMaxHp() { return 100; }
	virtual bool IsShowCtrlStatus() { return true; }
	virtual void ClearData() {};
public:
	virtual void ResetAttr() {}
	virtual void ResetSpeed() {}
	virtual void ResetAtkSpeed() {}
	//被攻击后怒气增加， 目前只有hero用
	virtual void BeAttackAnger(int nDamage) {}

	void SetAtkSpeed(int nSpeed);
	int GetAtkSpeed() const;
private:
	int				 m_nAtkSpeedRate{ 100 };	 //攻速 100起始
private:
	
	CampState        m_campState;
	bool             m_bFighting{false};         //是否在战斗中
	bool			 m_bAttack{ false };         //是否在攻击中
	bool             m_bDizziness{ false };      //是否在眩晕状态
	bool             m_bFreeze{ false };	     //是否在冰冻状态
	bool			 m_bAttackStopMove{ false }; //设置播放技能中,不能移动
	bool			 m_bHitPlaying{};    //是否在播放被攻击特效中
	int				 m_nAttackDis{ 0 };	 //攻击距离
	float			 m_fDieTm{ 0.0f };	//移除倒计时
	ARoleAIController* m_pAIControll{};  //AI控制器
	bool			 m_bHasPlayDeath{ false }; //是否已经播放过死亡动画

	std::map<int, AActor*> m_mapBuffEffects; //buff状态
	std::map<int, NS::I64>   m_mapSkillCoolTime;  //技能冷却时间
public:
	AMyCollision*	m_aRangeCollion{ nullptr };
};
