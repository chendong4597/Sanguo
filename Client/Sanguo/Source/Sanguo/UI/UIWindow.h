// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "GameAppExtern.h"
#include "../Common/CommonDefines.h"

#include "UIWindow.generated.h"



#define STR_TO_TEXT(a) FText::FromString(FString(a.c_str()))
#define LC_TO_TEXT(a) FText::FromString(FString(UTF8_TO_TCHAR(a.c_str())))
#define ORI_TO_TEXT(a) FText::FromString(FString(UTF8_TO_TCHAR(__LC(a.c_str()).c_str())))

#define LC_TO_STRING(a) FString(UTF8_TO_TCHAR(a.c_str()))
#define ORI_TO_STRING(a) FString(UTF8_TO_TCHAR(__LC(a.c_str()).c_str()))

#define STR_FORMAT_ARG_1(x) TArray<FStringFormatArg> Args; \
Args.Add(FStringFormatArg(x));

#define STR_FORMAT_ARG_1_X(x) TArray<FStringFormatArg> Args_x; \
Args_x.Add(FStringFormatArg(x));

#define STR_FORMAT_ARG_2(x1, x2) TArray<FStringFormatArg> Args; \
Args.Add(FStringFormatArg(x1)); \
Args.Add(FStringFormatArg(x2));

#define STR_FORMAT_ARG_2_X(x1, x2) TArray<FStringFormatArg> Args_x; \
Args_x.Add(FStringFormatArg(x1)); \
Args_x.Add(FStringFormatArg(x2));

#define STR_FORMAT_ARG_3(x1, x2, x3) TArray<FStringFormatArg> Args; \
Args.Add(FStringFormatArg(x1)); \
Args.Add(FStringFormatArg(x2)); \
Args.Add(FStringFormatArg(x3));

#define STR_FORMAT_ARG_4(x1, x2, x3, x4) TArray<FStringFormatArg> Args; \
Args.Add(FStringFormatArg(x1)); \
Args.Add(FStringFormatArg(x2)); \
Args.Add(FStringFormatArg(x3)); \
Args.Add(FStringFormatArg(x4));

/**
 * 
 */
UCLASS()
class SANGUO_API UUIWindow : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
	virtual void UnInit() {};
	virtual void BeginDestroy() override;
	virtual void NativeDestruct() override;

	void SetIsModel(bool bModel) { m_bModel = bModel; }

	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
	virtual FReply NativeOnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	void SetUIType(int nUIType) { n_UIType = nUIType; }
	int  GetUIType() const { return n_UIType; }
private:
	bool m_bModel{ true };
	int  n_UIType{ 0 };
};
