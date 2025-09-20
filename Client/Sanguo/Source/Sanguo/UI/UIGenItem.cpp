// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGenItem.h"
#include "../ReichGameInstance.h"

#include "../Player/IPlayer.h"
#include "../Player/Hero.h"
#include "../Player/Generals.h"

#include "UIGenData.h"

#include "UIWindow.h"

extern UWorld* gp_UWorld;

bool UUIGenItem::Initialize()
{
	if (!Super::Initialize())return false;
	if (UReichGameInstance::IsEditorMode()) return true;

	return true;
}

void UUIGenItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UUIGenData* pItem = Cast<UUIGenData>(ListItemObject);
	if (!pItem)
	{
		return;
	}
	auto&& imgSel = Cast<UImage>(GetWidgetFromName(FName("imgSel")));
	auto&& hero = AIPlayerMgr::getInstance().GetHero();
	if (!hero) {
		return;
	}

	AGenerals* pGen = Cast<AGenerals>(hero->GetAttachParentActor());
	if (!pGen)
	{
		return;
	}

	if (imgSel)
	{
		imgSel->SetVisibility(pItem->m_Id == pGen->GetMapObjectId() ?
			ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
