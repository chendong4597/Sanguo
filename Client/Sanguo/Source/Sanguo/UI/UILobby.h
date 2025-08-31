// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "handler/HandlerLogin.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "../Player/Hero.h"
#include "Camera/CameraActor.h"
#include "../Common/CommonDefines.h"
#include "Delegates/DelegateCombinations.h"

#include "UIWindow.h"

#include "UILobby.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API UUILobby : public UUIWindow
{
	GENERATED_BODY()
	//DECLARE_EVENT_HOST();

	enum {SHOTCUT_NUM = 5 , WEAPON_NUM = 7};
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	void ShowUI(bool bShowUI);
public:
	void onHorseChangeEvent(const HorseChangeEvent& evt);
	void onSkillCoolTimeEvent(const SkillCoolTimeEvent& evt);
	void onWeaponPreEquipEvent(const WeaponPreEquipEvent& evt);
	void onUILayoutEvent(const UILayoutEvent& evt);
public:
	//void onBtnChallengeEvent(const HandlerLoginStateEvent& evt);
	ENU_TOUCH_UITPY SetTouchInLobby(ETouchIndex::Type TouchIndex , FVector2D vec);
	void SetTouchOutLobby(int nTouchUITpy, FVector2D vec);
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI | LOBBY")
	void EventUpdateCoin();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI | LOBBY")
	void BallControll(bool bPress , FVector2D vec);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI | LOBBY")
	void AtkControll(bool bPress, FVector2D vec);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI | LOBBY")
	void HitSkillControll();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI | LOBBY")
	void AtkHover(bool bPress);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI | LOBBY")
	void SetAtkStyle(int nStyle);

public:
	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendWalk(float x , float y);

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendSight(float value);

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendAttack();

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendStartContinueAttack();

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendStopContinueAttack();

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendDodge();

	//使用武器上附加技能
	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendUseWeaponExtern();

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendWeapon(int nWeaponId);

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SendAttackHover(FVector vec);

	//获取技能使用方式  0：无  1：选位置释放  2：选方向释放
	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	int GetAttackHitType();

	//设置剔除渲染
	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SetCullSetting();

	void SetAttackHitType(int nHitTpy);
	bool IsTouchImageCancel(FVector2D vec);
	void SetTouchCancelHover(FVector2D vec , bool bRelease = false);

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	FVector2D GetTouchPosition(int nUITpy);

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	FVector2D GetHitDirPosition(int nUITpy);

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	FVector GetHeroPosition();

	UFUNCTION(BlueprintCallable, Category = "UI | LOBBY")
	void SetLayoutBang(float fValue);

protected:
	void ResetHorseCoolTime();
	void ResetExtSkillCoolTime();
	void ResetWeaponETag();

	void LayoutLobby();

protected:

	UFUNCTION()
	void OnSetup();

	UFUNCTION()
	void OnClickCharacter();

	UFUNCTION()
	void OnClickBag();

	UFUNCTION()
	void OnClickExit();

	UFUNCTION()
	void OnClickSwitchMode();

protected:
	UFUNCTION()
	void onBtnAtkClick();

	UFUNCTION()
	void onBtnAtkPressed();

	UFUNCTION()
	void onBtnAtkRelease();

	UFUNCTION()
	void onBtnDebug();

public:
	virtual bool Initialize() override;
	virtual void UnInit();
	virtual void BeginDestroy() override;

	virtual void NativeDestruct() override;
private:
	UCanvasPanel* m_panAtkBg{ nullptr };

	//主要界面Panel
	UCanvasPanel* m_panBall{nullptr};
	UCanvasPanel* m_RightPanel{ nullptr };
	UCanvasPanel* m_TopRightPanel{ nullptr };

	UImage*  m_imgCancel{nullptr};
	UImage* m_imgAchieveRed{ nullptr };

	UTextBlock* m_txtCancel{ nullptr };

	FVector2D m_posHitScroll;

	int m_nAttackUITpye{ 0 };        //根据武器技能获取攻击方式 0： 无  1：选择位置释放  2：方向释放
	int m_nExSkillId{ 0 };	//武器带的技能ID

	///////////////////////////
	UButton* m_btnCharacter{ nullptr };
	UButton* m_btnBag{ nullptr };
	UButton* m_btnExit{ nullptr };

	bool bIsUsingCineCam{ false };

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ACameraActor* CineCamera{ nullptr };

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* PlayerCamera{ nullptr };

	APlayerController* PlayerController{ nullptr };

};



