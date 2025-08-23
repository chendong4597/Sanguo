// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "UIWindow.h"
#include "UILoading.generated.h"

/**
 * 
 */

enum {
	ui_unload_state_begin = 0,
	ui_unload_state_players = 10,
	ui_unload_state_npcs = 20,
	ui_unload_state_npcs_end = 30
};

enum {
	ui_load_state_begin = 40,
	ui_load_state_player = 50,
	ui_load_state_plant = 60,
	ui_load_state_def = 70,
	ui_load_state_house = 80,
	ui_load_state_npc = 90,
	ui_load_state_init = 95,
	ui_unload_state_end = 100
};

UCLASS()
class SANGUO_API UUILoading : public UUIWindow
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
	virtual void UnInit() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	void SetIsLoadingPre(bool bPreLoad);
	void IsTriggerState(int nState, float preTm, float curTm);
private:
	UProgressBar* m_prgBar{ nullptr };
	UTextBlock*   m_txtPrg{ nullptr };
	UImage*       m_imgBg{ nullptr };

	bool m_bUnLoad{ false };  // «∑Ò «–∂‘ÿ≥°æ∞

	float m_fCurTm{ 0.f };
	float m_fMaxTm{ 3.f };
	int m_load_status{ 0 };
};
