// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "UIWindow.h"
#include "UIMain.generated.h"


UCLASS()
class SANGUO_API UUIMain : public UUIWindow
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
	virtual void UnInit() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
private:
};
