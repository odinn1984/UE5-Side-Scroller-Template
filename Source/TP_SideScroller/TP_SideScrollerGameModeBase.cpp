// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_SideScrollerGameModeBase.h"
#include "SKSideScrollerCharacter.h"
#include "PaperSideScrollerCharacter.h"
#include "Pixel2DSideScrollerCharacter.h"
#include "SideScrollerPlayerController.h"

ATP_SideScrollerGameModeBase::ATP_SideScrollerGameModeBase()
{
  DefaultPawnClass = ASKSideScrollerCharacter::StaticClass();
  PlayerControllerClass = ASideScrollerPlayerController::StaticClass();
}
