#include "SideScrollerPlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "SideScrollerCharacter.h"
#include "SideScrollerPlayerCameraManager.h"

ASideScrollerPlayerController::ASideScrollerPlayerController()
{
  MoveInputAction = LoadObject<UInputAction>(
    nullptr, 
    TEXT("InputAction'/Game/SideScroller/Inputs/InputAction_Move.InputAction_Move'"));
  StartJumpInputAction = LoadObject<UInputAction>(
    nullptr, 
    TEXT("InputAction'/Game/SideScroller/Inputs/InputAction_StartJumping.InputAction_StartJumping'"));
  StopJumpInputAction = LoadObject<UInputAction>(
    nullptr,
    TEXT("InputAction'/Game/SideScroller/Inputs/InputAction_StopJumping.InputAction_StopJumping'"));
  StartDashingAction = LoadObject<UInputAction>(
    nullptr,
    TEXT("InputAction'/Game/SideScroller/Inputs/InputAction_StartDashing.InputAction_StartDashing'"));
  StopDashingAction = LoadObject<UInputAction>(
    nullptr,
    TEXT("InputAction'/Game/Inputs/InputAction_StopDashing.InputAction_StopDashing'"));

  DefaultInputContext = LoadObject<UInputMappingContext>(
    nullptr, 
    TEXT("InputMappingContext'/Game/SideScroller/Inputs/InputMapping_SideScroller_Player.InputMapping_SideScroller_Player'"));;

  PlayerCameraManagerClass = ASideScrollerPlayerCameraManager::StaticClass();
}

void ASideScrollerPlayerController::SetupInputComponent()
{
  Super::SetupInputComponent();

  UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

  check(PlayerEnhancedInputComponent != nullptr);
  check(Subsystem != nullptr);

  Subsystem->ClearAllMappings();
  Subsystem->AddMappingContext(DefaultInputContext, 0);

  if (MoveInputAction)
  {
    PlayerEnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ASideScrollerPlayerController::Move);
  }
  
  if (StartJumpInputAction)
  {
    PlayerEnhancedInputComponent->BindAction(StartJumpInputAction, ETriggerEvent::Triggered, this, &ASideScrollerPlayerController::StartJumping);
  }

  if (StopJumpInputAction)
  {
    PlayerEnhancedInputComponent->BindAction(StopJumpInputAction, ETriggerEvent::Triggered, this, &ASideScrollerPlayerController::StopJumping);
  }

  if (StartDashingAction)
  {
    PlayerEnhancedInputComponent->BindAction(StartDashingAction, ETriggerEvent::Triggered, this, &ASideScrollerPlayerController::StartDashing);
  }

  if (StopDashingAction)
  {
    PlayerEnhancedInputComponent->BindAction(StopDashingAction, ETriggerEvent::Triggered, this, &ASideScrollerPlayerController::StopDashing);
  }
}

void ASideScrollerPlayerController::Move(const FInputActionValue& Value)
{
  ASideScrollerCharacter* ControllerPawn = Cast<ASideScrollerCharacter>(GetPawn());

  if (ControllerPawn != nullptr)
  {
    ControllerPawn->Move(Value.GetMagnitude());
  }
}

void ASideScrollerPlayerController::StartJumping(const FInputActionValue& Value)
{
  ASideScrollerCharacter* ControllerPawn = Cast<ASideScrollerCharacter>(GetPawn());

  if (ControllerPawn != nullptr)
  {
    ControllerPawn->StartJumping();
  }
}

void ASideScrollerPlayerController::StopJumping(const FInputActionValue& Value)
{
  ASideScrollerCharacter* ControllerPawn = Cast<ASideScrollerCharacter>(GetPawn());

  if (ControllerPawn != nullptr)
  {
    ControllerPawn->StopJumping();
  }
}

void ASideScrollerPlayerController::StartDashing(const FInputActionValue& Value)
{
  ASideScrollerCharacter* ControllerPawn = Cast<ASideScrollerCharacter>(GetPawn());

  if (ControllerPawn != nullptr)
  {
    ControllerPawn->StartDashing();
  }
}

void ASideScrollerPlayerController::StopDashing(const FInputActionValue& Value)
{
  ASideScrollerCharacter* ControllerPawn = Cast<ASideScrollerCharacter>(GetPawn());

  if (ControllerPawn != nullptr)
  {
    ControllerPawn->StopDashing();
  }
}