// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "Kismet/GameplayStatics.h"

void UPlayerAnimInstance::NativeUpdateAnimation(float dt)
{
	if (m_pPlayer == NULL){
		APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
		AIPlayer* pPlayer = Cast<AIPlayer>(Owner);
		if (pPlayer)
		{
			m_pPlayer = pPlayer;
		}
	}
	if (!m_pPlayer)
	{
		m_iWeaponTpy = 1;
		m_fFighting = 0.f;
		return;
	}
	
	m_fSpeed = m_pPlayer->GetVelocity().Size()*0.5f; // 获得移动速度
	m_iWeaponTpy = static_cast<int>(m_pPlayer->GetWeaponTpy());
	m_bDead = m_pPlayer->IsDeath();
	m_fFighting = m_pPlayer->GetIsFighting() ? 1.0f : 0.0f;
	m_nContinueIdx = m_pPlayer->GetContinueIdx();
	m_bDizz = m_pPlayer->IsDizziess();
	m_bFreeze = m_pPlayer->IsFreeze();
	m_bRiding = m_pPlayer->IsRiding();
	m_bBlend = m_fSpeed > 100 || m_bRiding;//m_pPlayer->GetBlend();
	PlayAnumAudio();
	UAnimInstance::NativeUpdateAnimation(dt);
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	m_iWeaponTpy = 0;
	m_fSpeed = 0.f;
	m_bBlend = false;
	m_bDead = false;
	m_fFighting = 1.0f;
	m_nContinueIdx = 0;
	m_bDizz = false;
	m_bFreeze = false;
	m_bRiding = false;
	APawn* Owner = TryGetPawnOwner(); // 得到所属的pawn
	AIPlayer* pPlayer = Cast<AIPlayer>(Owner);
	if (pPlayer){
		m_pPlayer = pPlayer;
	}
	StartAnimAudio();
	UAnimInstance::NativeInitializeAnimation();
}


void UPlayerAnimInstance::NativeUninitializeAnimation()
{
	m_iWeaponTpy = 0;
	m_fSpeed = 0.f;
	m_bBlend = false;
	m_bDead = false;
	m_fFighting = 1.0f;
	m_nContinueIdx = 0;
	m_bDizz = false;
	m_bFreeze = false;
	m_bRiding = false;
	StopAnimAudio();
	UAnimInstance::NativeUninitializeAnimation();
}

void UPlayerAnimInstance::BeginDestroy()
{
	StopAnimAudio();
	Super::BeginDestroy();
}

void UPlayerAnimInstance::AnimNotify_NormalEffect(UAnimNotify * Notify)
{
	m_pPlayer->AttackEffect();
}

void UPlayerAnimInstance::AnimNotify_SaveCombo(UAnimNotify * Notify)
{
	m_pPlayer->SetAttacking(false);
}

void UPlayerAnimInstance::AnimNotify_HideDarts(UAnimNotify * Notify)
{
	m_pPlayer->ShowDarts(false);
}

void UPlayerAnimInstance::AnimNotify_ShowDarts(UAnimNotify * Notify)
{
	m_pPlayer->ShowDarts(true);
	m_pPlayer->KillAttackTimer();
	m_pPlayer->DelayReset();
}

void UPlayerAnimInstance::AnimNotify_MagicEff(UAnimNotify * Notify)
{
	m_pPlayer->PlayWeaponSkillEffect();
}

void UPlayerAnimInstance::AnimNotify_ComboMagicEffect(UAnimNotify * Notify)
{
	m_pPlayer->PlayWeaponComboEffect();
}

void UPlayerAnimInstance::AnimNotify_HitEff(UAnimNotify * Notify)
{
	//m_pPlayer->HitEffect();
}

void UPlayerAnimInstance::AnimNotify_TriggerEffect(UAnimNotify * Notify)
{
	m_pPlayer->PlayTriggerSkillEffect();
}

//主要是多重伤害，播放动作的时候已经计算一次伤害了，这个是多次伤害的
void UPlayerAnimInstance::AnimNotify_CalDamage(UAnimNotify * Notify)
{
	m_pPlayer->CalDamageRange();
}

void UPlayerAnimInstance::AnimNotify_PreEffect(UAnimNotify * Notify)
{
	m_pPlayer->PlayWeaponPreSkillEffect();
}

void UPlayerAnimInstance::StartAnimAudio()
{
	AHero* pHero = Cast<AHero>(m_pPlayer);
	if (pHero == nullptr) {
		return;
	}

	if (!m_audioComp) {
		UObject* soundObj = StaticLoadObject(USoundBase::StaticClass(), NULL, TEXT("SoundWave'/Game/Sound/effect/run2.run2'"));
		m_audioComp = UGameplayStatics::CreateSound2D(GetWorld(), Cast<USoundBase>(soundObj), 1.f, 1.f, 0.f, nullptr, false, false);
	}
}

void UPlayerAnimInstance::StopAnimAudio()
{
	if (m_audioComp) {
		m_audioComp->Stop();
		m_audioComp->DestroyComponent();
		m_audioComp = nullptr;
	}
}

void UPlayerAnimInstance::PauseAnimAudio()
{
	if (m_audioComp) {
		m_audioComp->Stop();
		m_bSoundPlaying = false;
	}
}

void UPlayerAnimInstance::PlayAnumAudio()
{
	if (!m_audioComp || !m_pPlayer) {
		return;
	}

	AHero* pPlayer = Cast<AHero>(m_pPlayer);
	if (!pPlayer) {
		return;
	}

	int nHorseId = pPlayer->GetHorseId();
	if (nHorseId == 0) {
		if (m_fSpeed > 100.f) {
			if (!m_bSoundPlaying) {
				m_audioComp->Play();
				m_bSoundPlaying = true;
			}
		} else if(m_bSoundPlaying) {
			PauseAnimAudio();		}
	} else {
		if (m_bSoundPlaying) {
			PauseAnimAudio();
		}
	}
}

