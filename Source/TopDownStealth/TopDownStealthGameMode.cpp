// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TopDownStealthGameMode.h"
#include "TopDownStealthPlayerController.h"
#include "TopDownStealthCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATopDownStealthGameMode::ATopDownStealthGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATopDownStealthPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}