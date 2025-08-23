// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Role.h"
//#include "NetworkModule.h"
#include "config/EquipTypeCfg.h"
#include "../Common/UIEvents.h"
#include "../Common/CommonDefines.h"
#include "data/role/Player.h"
#include <map>

#include "IPlayer.generated.h"

class AWeapon;


USTRUCT(BlueprintType)
struct FStuPlayerBaseInfo
{
	GENERATED_USTRUCT_BODY()

		// ʹ��UPROPERTY���η����Ը����������ͼ������
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Role | FStuPlayerInfo")
	int nId = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Role | FStuPlayerInfo")
	int nProfessIdx = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Role | FStuPlayerInfo")
	int nHairStyle = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Role | FStuPlayerInfo")
	int nHairColor = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Role | FStuPlayerInfo")
	int nSkinColor = 0;
};


struct StuPlayerEquipInfo {
public:
	int nEquipHeadId{ 0 };
	int nEquipBodyId{ 0 };
	int nEquipLegsId{ 0 };
	int nEquipCapeId{ 0 };
	int nEquipWrapId{ 0 };
	int nWeaponId{ 0 };
	int nWeaponTpy{ 0 };//��������ID�ı䣬Ϊ����Ч�ʶ���
};

/**
 * 
 */
UCLASS()
class SANGUO_API AIPlayer : public ARole
{
	GENERATED_BODY()
	//DECLARE_TIMER_HOST();
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const
	{
		//AssetType��FPrimaryAssetType���͵ı�����Ӧ����ζ��壬�������½�˵��
		return FPrimaryAssetId("Player", GetFName());
	}

	//message
	void onMsgInteractionS2C(const MsgInteractionS2C& msg);
	void onMsgMagicEffectAppenceS2C(const MsgMagicEffectAppenceS2C& msg);
	void onMsgPlayerInfoGS2C(const MsgPlayerInfoGS2C& msg);
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);
	virtual void Init();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsHero();
	void SetLobbyAttackTpy(int nTpy);
	//���岿��
	void InitSkin(int nProfessIdx);
	void ResetWeapon();
	void ResetHorse();

	virtual FTransform GetWeaponTransByName(FString strSockName);
public:
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	AWeapon* LoadWeapon(FString path);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	AWeaponEffect* LoadWeaponEffect(FString path, FTransform trans);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	UAnimMontage* LoadAniMontage(int weaponTpy, int comboIdx);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	UAnimMontage* LoadHorseMontage(int weaponId);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	UAnimMontage* LoadAniByWeaponId(bool bContinue , int& nContinueIdx , bool& bAttackStopMove);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	const FStuPlayerBaseInfo& GetPlayerBaseInfo();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void SetPlayerBaseInfo(int nSex, int nHairStyle, int nHairColor, int nSkinColor);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void ResetPlayerMesh();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	FString GetLWeaponSockName();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	FString GetRWeaponSockName();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void AttachWeaponById(int weaponId = 0);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void RemoveWeapons();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	AWeapon* GetLeftWeapon() { return m_LeftAWeapon; }

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	AWeapon* GetRightWeapon() { return m_RightAWeapon; }

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void ShowDarts(bool bShow);

	virtual bool IsCanAttack() override;
	virtual bool PlayAttack() override;

	void PlaySkillAttack(int m_nSkillId);

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayAttackContinue();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void StopAttackContinue();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void RideHorse(int nHorseId);

	int GetHorseId() { return m_idHorseId; }

	void ResetRunAudio();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void UnRideHorse();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayWeaponSoundEffect(int nCombomIdx);

	void SendInPlayerSight(NS::I64 iggId);
	void SendOutPlayerSight(NS::I64 iggId);

	virtual void AddBuff(int nBuffId);
	virtual void RemoveBuff(int nBuffId);

	virtual FVector GetMagicBeginLocationByName(const char* pszSockName) override;
	virtual void SetMaterialScalerParamer(FName name, float fValue) override;
	
public:
	virtual void Death() override;
	virtual bool IsDeath() override;
	virtual void AttackEffect() override;
	virtual bool IsCanAttackTarget(ARole* pRole, bool bPassive = false) override;
	virtual std::string GetHitEffectPath() override;

	void StartSkill(int nSkillId, ARole* attackRole, ARole* targetRole, FVector vec = FVector::ZeroVector, FVector rator = FVector::ZeroVector);
public:
	//�����Ƿ��϶���
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	bool GetBlend() { return m_bBlend; }

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void SetBlend(bool bBlend) { m_bBlend = bBlend; }

	//������������
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	int GetWeaponTpy() { return m_EquipInfo.nWeaponTpy; }
	int GetWeaponId() { return m_EquipInfo.nWeaponId; }

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void SetWeaponId(int nWeaponId);

	//��ȡ��������
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	int GetComboNum();

	//�ͷż���
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayWeaponSkillEffect();

	//�ͷż���ǰ��
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayWeaponPreSkillEffect();

	//�ͷ����м���
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayWeaponComboEffect();

	//�ͷŴ�������
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayTriggerSkillEffect();

	//���﹥������
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayHorseAttack();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void HorseEffect();

	//�˺�ǰ  weaponIdx : 0���� 1������ 2����ֻ��
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayWeaponPreHurt(int weaponIdx);

	//�˺���  weaponIdx : 0���� 1������ 2����ֻ��
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void PlayWeaponHurt(int weaponIdx);

	void CalDamageRange();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void SetProfessIdx(int nIdx);

	//�Ƿ�������״̬
	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	bool IsRiding();

	UFUNCTION(BlueprintCallable, Category = "PLAYER")
	void SetRiding(bool bRiding);
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "PLAYER")
	void SetTargetPos(FVector vecPos);
public:
	//��ȡ������β��Ч���ӵ�ַ
	virtual FString GetTrailParticlePath() override;
public:
	std::shared_ptr<Player> m_playerData{};

	FVector m_vecScrollPos;   //����λ��ѡ��
	FVector m_rotScroll;      //����λ�÷���
public:
	void KillAttackTimer();
	void DelayReset();

	int GetContinueIdx() { return m_nContinueIdx; }

public:
	virtual int GetObjectId() override;
	virtual int GetRoleCtrlStatus() override;
	virtual int GetHp() override;
	virtual int GetMaxHp() override;

	virtual std::string GetBattleSound(int nSoundTpy) override;

public:
	virtual void ResetAttr() override;
	virtual void ResetSpeed() override;
	virtual void ResetAtkSpeed() override;
private:
	bool m_bBlend;                   //�Ƿ��϶���
	bool m_bMirror;					 //�Ƿ���
	int m_idWeapon{ 0 };			 //����ID
	int m_idHorseId{ 0 };			 //���ID
	int m_nComboIdx{0};              //��ǰ����idx
	int m_nComboNum{ 0 };            //��������
	int m_nContinueIdx{ 0 };         //������������

	FStuPlayerBaseInfo m_PlayerInfo;
	StuPlayerEquipInfo m_EquipInfo;

	AWeapon* m_LeftAWeapon{};
	AWeapon* m_RightAWeapon{};

	AActor* m_soulActor{nullptr};

	FTimerHandle MyTimerHand;
};

