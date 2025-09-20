// Copyright Epic Games, Inc. All Rights Reserved.

#include "SanguoPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "SanguoCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Player/IPlayer.h"
#include "Player/Hero.h"
#include "UI/UILobby.h"
#include "UI/UIManager.h"

#include "Player/Generals.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

bool g_bIsUsingCineCam = false;

ASanguoPlayerController::ASanguoPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ASanguoPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ASanguoPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ASanguoPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ASanguoPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ASanguoPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ASanguoPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ASanguoPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ASanguoPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ASanguoPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ASanguoPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASanguoPlayerController::OnInputStarted()
{
	if (!g_bIsUsingCineCam)
	{
		return;
	}
	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void ASanguoPlayerController::OnSetDestinationTriggered()
{
	if (!g_bIsUsingCineCam)
	{
		return;
	}

	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
}

void ASanguoPlayerController::ResetPawn(bool bHero)
{
	return;
	//AHero* pHero = AIPlayerMgr::getInstance().GetHero();
	//if (!pHero)
	//{
	//	return;
	//}
	//if (bHero)
	//{
	//	this->SetPawn(pHero);
	//	Possess(pHero);
	//	return;
	//}
	//AGenerals* pGen = Cast<AGenerals>(pHero->GetAttachParentActor());
	//if (!pGen)
	//{
	//	this->SetPawn(pHero);
	//	Possess(pHero);
	//	return;
	//}
	////UnProcess();
	//this->SetPawn(pGen);
	//Possess(pGen);

	// ÆôÓÃÊäÈë
	//pGen->EnableInput(this);

	//TArray<AActor*> arrActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), arrActors);
	//if (arrActors.Num() > 0)
	//{
	//	ACameraActor* pCamera = Cast<ACameraActor>(arrActors[0]);
	//	SetViewTargetWithBlend(pCamera, 0.f);
	//}
}

void ASanguoPlayerController::OnSetDestinationReleased()
{
	if (!g_bIsUsingCineCam)
	{
		return;
	}
	// If it was a short press
	if (FollowTime > ShortPressThreshold)
	{
		return;
	}

	UUserWidget* pWidget = UIManager::getInstance().GetWidget(UI_TPY_Lobby);
	if (!pWidget)
	{
		return;
	}
	UUILobby* pLobby = Cast<UUILobby>(pWidget);
	if (!pLobby)
	{
		return;
	}

	AGenerals* pSelGen = nullptr;

	for (auto&& gen : AGeneralsMgr::getInstance().GetMapGenerals())
	{
		if (FVector::Distance(gen.second->GetActorLocation(), 
			FVector(CachedDestination.X - 150, CachedDestination.Y, CachedDestination.Z)) < 300.f)
		{
			pSelGen = gen.second;
			break;
		}
	}

	AHero* pHero = AIPlayerMgr::getInstance().GetHero();
	if (!pHero)
	{
		return;
	}
	AGenerals* pGen = Cast<AGenerals>(pHero->GetAttachParentActor());
	if (!pGen)
	{
		if (pSelGen)
		{
			pLobby->SetFocusGenerals(pSelGen->GetMapObjectId());
		}
		return;
	}
	else {
		if (pSelGen && pSelGen != pGen)
		{
			pLobby->SetFocusGenerals(pSelGen->GetMapObjectId());
			return;
		}
	}

	StopMovement();

	pGen->MoveToDestination(CachedDestination);

	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void ASanguoPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ASanguoPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
