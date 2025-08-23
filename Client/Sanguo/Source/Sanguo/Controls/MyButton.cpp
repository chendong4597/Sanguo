// Fill out your copyright notice in the Description page of Project Settings.


#include "MyButton.h"
#include "../UI/UIWindow.h"

bool UMyButton::Initialize()
{
	if (!Super::Initialize())return false;
	m_btn = Cast<UButton>(GetWidgetFromName(TEXT("btn")));
	m_texblock = Cast<UTextBlock>(GetWidgetFromName(TEXT("text")));
	return true;
}

void UMyButton::SetLCText(const char* pszTxt)
{
	if (!m_texblock || !pszTxt)
	{
		return;
	}
	m_bHasSet = true;
	std::string strContent = __LC(pszTxt);
	FString stra = UTF8_TO_TCHAR(strContent.c_str());
	m_texblock->SetText(FText::FromString(stra));
}

void UMyButton::SetMyText(const FText& txt)
{
	if (!m_texblock)
	{
		return;
	}
	m_bHasSet = true;
	m_texblock->SetText(txt);
}

void UMyButton::SetMyString(const FString& str)
{
	if (!m_texblock)
	{
		return;
	}
	m_bHasSet = true;
	m_texblock->SetText(FText::FromString(str));
}

void UMyButton::SetState(int nState)
{
	if (!m_btn) {
		return;
	}
	if (nState == UMyButton_State_Normal) {
		m_btn->SetIsEnabled(true);
	} else if (nState == UMyButton_State_Select) {
		m_btn->SetIsEnabled(true);
	} else if (nState == UMyButton_State_Disable) {
		m_btn->SetIsEnabled(false);
	}
}