// Copyright Epic Games, Inc. All Rights Reserved.

#include "SanguoGameMode.h"
#include "SanguoPlayerController.h"
#include "SanguoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASanguoGameMode::ASanguoGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASanguoPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Models/Player/BP_Hero.BP_Hero_C"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Models/Player/BP_HeroController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}