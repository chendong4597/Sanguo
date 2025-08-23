// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIWindow.h"
#include "UIRoundProcessBar.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API UUIRoundProcessBar : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent, Category = "")
	void CoolTimeEvt();

	void SetProcessTime(float allTime, float curTime);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "")
	float m_showTime{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "")
	float m_curTime{};

	NS::I64 m_n64AllTm{ 0 };
	NS::I64 m_n64BeginTm{ 0 };
};
