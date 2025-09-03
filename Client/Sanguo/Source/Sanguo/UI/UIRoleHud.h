// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "../Common/UIEvents.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "../UI/UIRoundProcessBar.h"

#include "UIRoleHud.generated.h"


enum {
	ROLE_HUD_TYPE_NONE = 0,
	ROLE_HUD_TYPE_FUNC_NPC,  //功能Npc
	ROLE_HUD_TYPE_WALK_NPC,  //他人功能Npc
	ROLE_HUD_TYPE_WORKING_NPC,  //自己的Npc
	ROLE_HUD_TYPE_GENEL,  //武将
	ROLE_HUD_TYPE_DEFENCE,  //防御塔
	ROLE_HUD_TYPE_CRYSTAL,  //水晶
	ROLE_HUD_TYPE_MONSTER,  //怪物
	ROLE_HUD_TYPE_STALL,  //摊位
};

/**
 * 
 */
UCLASS()
class SANGUO_API UUIRoleHud : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UFUNCTION(BlueprintCallable, Category = "ROLE_HUD")
	void SetOwnerRole(AActor* pRole);

	UFUNCTION(BlueprintCallable, Category = "ROLE_HUD")
	AActor* GetOwnerRole();

	
public:
private:
	AActor* m_ownerRole{};

	UImage* m_selArrow{ nullptr };
};
