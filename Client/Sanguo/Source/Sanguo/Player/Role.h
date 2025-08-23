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
enum class CampState :uint8 //����uint8����
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

	//����
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
	//��Ӫ
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetCamp(CampState camp) { m_campState = camp; }

	//�жϵ�ǰ�����Ƿ�������
	UFUNCTION(BlueprintCallable, Category = "Generals")
	bool IsInMainCity();

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	CampState GetCamp() { return m_campState; }

	//�Ƿ��ǿɹ�����Ŀ��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsCanAttackTarget(ARole* pRole, bool bPassive = false) { return false; }

	//�Ƿ��ǿɱ��ҷ�������Ŀ��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsCanBeAttackTarget(ARole* pRole) { return true; }

	//��ȡ�ܻ�λ��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	FTransform GetHitWorldTransform();

	//����ս��״̬
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool GetIsFighting();

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetIsFighting(bool bFighting);

	//����
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void Death();

	//����
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void Revive();

	//�Ƿ��Ѿ�����
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsDeath() { return false; }

	//�˺�
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void Damage(int castId, int nHurtDamage, bool bCrit, bool bDot = false);

	//�Ƿ񹥻�״̬
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool IsAttacking() { return m_bAttack; }

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetAttacking(bool bAttack) { m_bAttack = bAttack; }

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void AttackEffect() {};

	//��ȡ�������ʱ��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual float GetNormalAttackInterTm() { return 1.5f; };

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsCanAttack() { return false; }

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool PlayAttack() { return false; };

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void PlayDeath() {};

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool CanActiveAttack() { return true; }   //�Ƿ����������

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsForbidAttack() { return false; }   //�Ƿ��ܱ�����

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool IsOnlyBeNormalAtk() { return false; }   //�Ƿ�ֻ�ܱ���ͨ�����˺�

public:
	virtual std::string GetHitEffectPath() { return ""; } //��ȡ�����ߵĻ���Ŀ����Ч
	virtual void HitEffect(const char* pszEffect);

public:
	//����Ŀ��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetFocusRole(ARole* aRole);

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	ARole* GetFocusRole();

	//��ʾ�߿�
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void ShowHighLightMesh(bool bShow);

	//���ò��ż�����,�����ƶ�
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetAttackStopMove(bool bStopMove) { m_bAttackStopMove = bStopMove; }

	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool GetAttackStopMove() { return m_bAttackStopMove; }

	//�Ƿ�����ƶ� ״̬Ӱ��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool CanMove();

	//�Ƿ���Թ��� ״̬Ӱ��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool CanAttack();

	//�Ƿ����ʹ�ü��� ״̬Ӱ��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual bool CanSkillAttack();

	//����BUff
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void AddBuff(int nBuffId);
	void AddBuffEff(int nBuffId , AActor* eff);
	virtual void ClearBuff();

	//ɾ��BUff
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void RemoveBuff(int nBuffId);

	const std::map<int, AActor*>& GetBuffs() { return m_mapBuffEffects; }

	//���ű�������Ч
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void PlayHit() {};

	//���ñ�����̬��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	void SetHitPlaying(bool bHit) { m_bHitPlaying = bHit; }

	//�Ƿ��ڱ�����̬��
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	bool IsHitPlaying() { return m_bHitPlaying; }

	//�Ƿ�ʼAI
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	virtual void StartAI() {}

	//��ȡ��ǰ��ͼ����
	UFUNCTION(BlueprintCallable, Category = "ROLE")
	int GetCurrentLevelTpy();

	//
	virtual FTransform GetRoleTransByName(const char* pszSockName);
	virtual FTransform GetWeaponTransByName(FString strSockName);

	virtual FVector GetMagicBeginLocationByName(const char* pszSockName) {return GetRoleTransByName(pszSockName).GetLocation();}

	//��ҵ���л�Ӧ
	virtual bool HitTest() { return false; }

	//��ȡ������β��Ч��ַ
	virtual FString GetTrailParticlePath() {return FString("");}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	void RegisterRoleClick();

	//�Ƿ����������Χ��
	bool IsInCameraView();

	//�������
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

	//����ʧȥĿ��󣬿��ٷ���
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
	//��������ŭ�����ӣ� Ŀǰֻ��hero��
	virtual void BeAttackAnger(int nDamage) {}

	void SetAtkSpeed(int nSpeed);
	int GetAtkSpeed() const;
private:
	int				 m_nAtkSpeedRate{ 100 };	 //���� 100��ʼ
private:
	
	CampState        m_campState;
	bool             m_bFighting{false};         //�Ƿ���ս����
	bool			 m_bAttack{ false };         //�Ƿ��ڹ�����
	bool             m_bDizziness{ false };      //�Ƿ���ѣ��״̬
	bool             m_bFreeze{ false };	     //�Ƿ��ڱ���״̬
	bool			 m_bAttackStopMove{ false }; //���ò��ż�����,�����ƶ�
	bool			 m_bHitPlaying{};    //�Ƿ��ڲ��ű�������Ч��
	int				 m_nAttackDis{ 0 };	 //��������
	float			 m_fDieTm{ 0.0f };	//�Ƴ�����ʱ
	ARoleAIController* m_pAIControll{};  //AI������
	bool			 m_bHasPlayDeath{ false }; //�Ƿ��Ѿ����Ź���������

	std::map<int, AActor*> m_mapBuffEffects; //buff״̬
	std::map<int, NS::I64>   m_mapSkillCoolTime;  //������ȴʱ��
public:
	AMyCollision*	m_aRangeCollion{ nullptr };
};
