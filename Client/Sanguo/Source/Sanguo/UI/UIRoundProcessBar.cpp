// Fill out your copyright notice in the Description page of Project Settings.


#include "UIRoundProcessBar.h"
#include "common/Heartbeat.h"

void UUIRoundProcessBar::SetProcessTime(float allTime, float curTime)
{
	m_showTime = allTime;
	m_curTime = curTime;


	m_n64AllTm = m_curTime * 1000;
	m_n64BeginTm = Heartbeat::getInstance().getServerTimeMs();
	
	CoolTimeEvt();
}

void UUIRoundProcessBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (m_showTime < 0.000001f)
	{
		return;
	}
	NS::I64 n64Has = Heartbeat::getInstance().getServerTimeMs() - m_n64BeginTm;
	m_curTime = (m_n64AllTm - n64Has) * 1.f / 1000;
}