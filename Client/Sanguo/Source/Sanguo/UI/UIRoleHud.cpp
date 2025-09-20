// Fill out your copyright notice in the Description page of Project Settings.


#include "UIRoleHud.h"

#include "../Player/DefenceBuilding.h"
#include "../UI/UIManager.h"
#include "../Player/Role.h"
#include "common/Heartbeat.h"
#include "config/NpcTypeCfg.h"
#include "config/PlantCfg.h"
#include "data/PlantData.h"
#include "data/NpcProtectorData.h"
#include "data/HouseData.h"
#include "data/TowerBuildData.h"
#include "data/CraftData.h"
#include "data/role/Npc.h"
#include "data/role/Monsters.h"
#include "data/role/Player.h"

#include "../Player/Hero.h"
#include "../Player/Generals.h"
#include "../ReichGameInstance.h"

extern UWorld* gp_UWorld;

extern bool g_bIsUsingCineCam;

bool UUIRoleHud::Initialize()
{
	if (!Super::Initialize())return false;
	if (UReichGameInstance::IsEditorMode())
	{
		return true;
	}
	m_selArrow = Cast<UImage>(GetWidgetFromName(FName("ImgSel")));
	return true;
}

void UUIRoleHud::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (m_selArrow) {
		m_selArrow->SetVisibility(
			AIPlayerMgr::getInstance().GetHero()->GetAttachParentActor() == m_ownerRole ?
			ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UUIRoleHud::SetOwnerRole(AActor* pRole)
{
	m_ownerRole = pRole;
}

AActor* UUIRoleHud::GetOwnerRole()
{
	return m_ownerRole;
}
