// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MyButton.generated.h"

/**
 * 
 */
enum {
	UMyButton_State_Normal = 0,
	UMyButton_State_Select,
	UMyButton_State_Disable,
};

UCLASS()
class SANGUO_API UMyButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	void SetLCText(const char* pszTxt);
	void SetMyText(const FText& txt);
	void SetMyString(const FString& str);

	void SetParam1(int nParam1) { m_nParam1 = nParam1; }
	int  GetParam1() { return m_nParam1; }

	void SetState(int nState);
public:
	UPROPERTY(BlueprintReadWrite, Category = "UI | UMyButton")
	bool m_bHasSet{ false };

	UButton *m_btn;
	UTextBlock *m_texblock;

	int m_nParam1{ 0 };
};
