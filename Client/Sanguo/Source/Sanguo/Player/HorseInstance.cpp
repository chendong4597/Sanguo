// Fill out your copyright notice in the Description page of Project Settings.


#include "HorseInstance.h"
#include "IPlayer.h"
#include "Hero.h"
#include "config/WeaponInfoTypeCfg.h"
#include "Kismet/GameplayStatics.h"

void UHorseInstance::NativeInitializeAnimation()
{
	m_nSoundPlay = 0;
	StartAudio();
	UAnimInstance::NativeInitializeAnimation();
}

void UHorseInstance::NativeUninitializeAnimation()
{
	StopAudio();
	UAnimInstance::NativeUninitializeAnimation();
}

void UHorseInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	PlayAudio();
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);
}


void UHorseInstance::BeginDestroy()
{
	StopAudio();
	Super::BeginDestroy();
}


void UHorseInstance::StartAudio()
{
	auto&& pMaster = GetOwningActor();
	if (pMaster) {
		AHero* pPlayer = Cast<AHero>(pMaster);
		if (pPlayer) {
			m_nHorseId = pPlayer->GetHorseId();
		}
	}
	if (m_nHorseId == 0) {
		return;
	}

	if (!m_audioComp) {
		UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, TEXT("SoundWave'/Game/Sound/effect/bag.bag'"));
		m_audioComp = UGameplayStatics::CreateSound2D(GetWorld(), Cast<USoundBase>(soundObj), 1.f, 1.f, 0.f, nullptr, false, false);
	}
}

void UHorseInstance::StopAudio()
{
	if (m_audioComp)
	{
		m_nSoundPlay = 0;
		m_audioComp->Stop();
		m_audioComp->DestroyComponent();
		m_audioComp = nullptr;
		m_nHorseId = 0;
	}
}

void UHorseInstance::PlayAudio()
{
	if (!m_audioComp) {
		return;
	}
	auto&& pHorse = GetOwningActor();
	if (!pHorse || m_nHorseId == 0) {
		m_audioComp->Stop();
		return;
	}
	float fSpeed = pHorse->GetVelocity().Size()*0.5f;
	if (fSpeed >= 100.f && m_nSoundPlay == HORSE_RUN_RUNING) {
		return;
	} else if (fSpeed < 100.f && m_nSoundPlay == HORSE_RUN_IDLE) {
		return;
	}
	m_audioComp->Stop();
	if (fSpeed >= 100.f) {
		m_nSoundPlay = HORSE_RUN_RUNING;
		std::string str = WeaponInfoTypeCfg::getInstance().GetAnimSound(m_nHorseId, true);
		if (str.empty()) {
			return;
		}
		FString setSoundPath = FString(str.c_str());
		UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, *(setSoundPath));
		if (!soundObj) {
			return;
		}
		m_audioComp->SetSound(Cast<USoundBase>(soundObj));
		m_audioComp->Play();
	} else {
		m_nSoundPlay = HORSE_RUN_IDLE;
		std::string str = WeaponInfoTypeCfg::getInstance().GetAnimSound(m_nHorseId, false);
		if (str.empty()) {
			return;
		}
		FString setSoundPath = FString(str.c_str());
		UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, *(setSoundPath));
		if (!soundObj) {
			return;
		}
		m_audioComp->SetSound(Cast<USoundBase>(soundObj));
		m_audioComp->Play();
		
	}
}

