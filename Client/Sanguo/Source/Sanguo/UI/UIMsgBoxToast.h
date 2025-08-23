// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameAppExtern.h"
#include "UIWindow.h"
#include "UIMsgBoxToast.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API UUIMsgBoxToast : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	virtual bool Initialize() override;

	virtual void NativePreConstruct();
	virtual void NativeDestruct();

	void ShowText(const char* pszText);
	void ShowNextText();

	UFUNCTION(BlueprintCallable, Category = "UUIMsgBoxToast")
	void NextTips();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "UUIMsgBoxToast")
	void StartShow();
public:
	UPROPERTY()
	TArray<FString> m_arrayText;
private:
	UTextBlock* m_textBlock{ nullptr };
	bool m_bNext{ false };
	float m_curTm{ 0.f };
};

class MsgBoxToastMgr// : public bzbee::MsgHandler
{
	MsgBoxToastMgr();
	//DECLARE_EVENT_HOST();
public:
	bool initialize();
	void terminate();
	bool reset();
	void update(float dt);

	void ShowText(const std::string& strText);
public:
	static MsgBoxToastMgr& getInstance();

private:
};
