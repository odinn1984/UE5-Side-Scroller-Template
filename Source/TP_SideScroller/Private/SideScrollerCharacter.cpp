#include "SideScrollerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ASideScrollerCharacter::ASideScrollerCharacter(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;

  bUseControllerRotationYaw = false;
  bUseControllerRotationPitch = false;
  bUseControllerRotationRoll = false;

  JumpMaxHoldTime = 0.0f;
  JumpMaxCount = 1;
  
  GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
  GetCharacterMovement()->bConstrainToPlane = true;
  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->bUseControllerDesiredRotation = false;
  GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f);
  GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
  GetCharacterMovement()->GravityScale = 3.0f;
  GetCharacterMovement()->AirControl = 1.0f;
  GetCharacterMovement()->JumpZVelocity = 1250.0f;
  GetCharacterMovement()->GroundFriction = 100.0f;
  GetCharacterMovement()->FallingLateralFriction = 1.0f;
  GetCharacterMovement()->MaxAcceleration = 8000.0f;

  GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);

  WallDetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Jumpable Wall Trigger"));
  WallDetectionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
  WallDetectionBox->SetWorldLocation(FVector(25.0f, 0.0f, 0.0f));
  WallDetectionBox->SetBoxExtent(FVector(25.0, 25.0, 50.0f));
  WallDetectionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  InitCharacter();

  SetReplicates(true);
  SetReplicateMovement(true);
}

void ASideScrollerCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  UpdateIsTouchingWall();

  if (GetVelocity().Z < 0)
  {
    if (bCanWallSlide && bIsTouchingWall)
    {
      bIsSlidingOnWall = true;

      if (bWallJumpRequested)
      {
        bWallJumpRequested = false;
        GetCharacterMovement()->Velocity.Z = 0.0f;
      }
      else
      {
        GetCharacterMovement()->Velocity.Z = -250.0f;
      }
    }
    else if (IsCoyoteTime())
    {
      GetCharacterMovement()->Velocity.Z = 0.0f;
    }
    else
    {
      bIsSlidingOnWall = false;
    }
  }
}

void ASideScrollerCharacter::NotifyJumpApex()
{
  Super::NotifyJumpApex();
}

void ASideScrollerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
  Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

  if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
  {
    if (PrevMovementMode == EMovementMode::MOVE_Walking)
    {
      if (!bIsDashing)
      {
        GetWorldTimerManager().SetTimer(
          CoyoteTimer,
          this,
          &ASideScrollerCharacter::OnCoyoteTimerEnd,
          CoyoteTime
        );
      }
    }
  }
  else if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
  {
    if (PrevMovementMode == EMovementMode::MOVE_Falling)
    {
      DashesInAirRemaining = MaxDashesInAir;
      GetCharacterMovement()->GravityScale = OriginalGravityScale;

      if (IsJumpBufferTime())
      {
        GetWorld()->GetTimerManager().ClearTimer(JumpBufferTimer);
        Jump();
      }
    }
  }
}

void ASideScrollerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASideScrollerCharacter::Move(float Value)
{
  Value = FMath::Clamp(Value, -1.0f, 1.0f);

  if (Value != 0.0f)
  {
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    AddMovementInput(Direction, Value > 0.0f ? 1.0f : -1.0f);
  }
}

void ASideScrollerCharacter::StartJumping()
{
  if (CanWallJump())
  {
    if (bFaceWallJumpDirection)
    {
      Move(-GetActorForwardVector().X);
    }
    
    WallJump();
  }
  else if (CanJump())
  {
    Jump();
  }
  else
  {
    GetWorldTimerManager().SetTimer(
      JumpBufferTimer,
      this,
      &ASideScrollerCharacter::OnWallCoyoteTimerEnd,
      JumpBufferTime
    );
  }
}

bool ASideScrollerCharacter::CanWallJump()
{
  return (
    bIsTouchingWall && 
    bCanWallJump && 
    GetCharacterMovement()->IsFalling()
    ) || IsWallCoyoteTime();
}

void ASideScrollerCharacter::StopJumping()
{
  Super::StopJumping();
  
  GetCharacterMovement()->GravityScale = OriginalGravityScale * StopJumpGravityScaleModifier;
}

void ASideScrollerCharacter::WallJump_Implementation()
{
  FVector forwardVector = IsWallCoyoteTime() ?
    GetActorForwardVector() : -GetActorForwardVector();

  bWallJumpRequested = true;
  GetCharacterMovement()->GravityScale = OriginalGravityScale;

  LaunchCharacter(
    (forwardVector + GetActorUpVector()) * LaunchOffWallVelocity,
    true, 
    true
  );
}

bool ASideScrollerCharacter::CanDash() const
{
  if (!bIsDashing && !IsDashOnCooldown())
  {
    return !(GetCharacterMovement()->IsFalling() && DashesInAirRemaining <= 0);
  }

  return false;
}

void ASideScrollerCharacter::StartDashing_Implementation()
{
  if (CanDash())
  {
    Dash();
  }
}

void ASideScrollerCharacter::StopDashing_Implementation()
{
  if (bIsDashing && bStopDashOnButtonRelease)
  {
    StopDash();
  }
}

void ASideScrollerCharacter::BeginPlay()
{
  Super::BeginPlay();

  InitCharacter();
}

void ASideScrollerCharacter::InitCharacter()
{
  OriginalGravityScale = GetCharacterMovement()->GravityScale;
  DashesInAirRemaining = MaxDashesInAir;
}

void ASideScrollerCharacter::Jump()
{
  GetCharacterMovement()->bNotifyApex = true;
  GetCharacterMovement()->GravityScale = OriginalGravityScale;

  Super::Jump();
}

bool ASideScrollerCharacter::CanJumpInternal_Implementation() const
{
  return Super::CanJumpInternal_Implementation() || 
    (GetCharacterMovement()->Velocity.Z == 0.0f && IsCoyoteTime());
}

void ASideScrollerCharacter::Dash()
{
  if (CanDash())
  {
    FVector Direction = GetActorForwardVector() + GetActorUpVector();

    bIsDashing = true;
    DashesInAirRemaining--;

    LaunchCharacter(Direction * DashInitialVelocity, true, true);

    GetWorld()->GetTimerManager().SetTimer(
      DashDurationTimer,
      this,
      &ASideScrollerCharacter::OnDashDurationTimerEnd,
      DashDuration
    );
  }
}

void ASideScrollerCharacter::StopDash()
{
  bIsDashing = false;

  GetCharacterMovement()->StopMovementImmediately();
  GetWorld()->GetTimerManager().SetTimer(
    DashCooldownTimer,
    this,
    &ASideScrollerCharacter::OnDashCooldownTimerEnd,
    DashCooldown
  );
}

void ASideScrollerCharacter::OnCoyoteTimerEnd()
{
}

void ASideScrollerCharacter::OnWallCoyoteTimerEnd()
{
}

void ASideScrollerCharacter::OnDashDurationTimerEnd()
{
  StopDash();
}

void ASideScrollerCharacter::OnDashCooldownTimerEnd()
{
}

bool ASideScrollerCharacter::IsCoyoteTime() const
{
  return GetWorld()->GetTimerManager().GetTimerRemaining(CoyoteTimer) > 0.0f;
}

bool ASideScrollerCharacter::IsWallCoyoteTime() const
{
  return GetWorld()->GetTimerManager().GetTimerRemaining(WallCoyoteTimer) > 0.0f;
}

bool ASideScrollerCharacter::IsJumpBufferTime() const
{
  return GetWorld()->GetTimerManager().GetTimerRemaining(JumpBufferTimer) > 0.0f;
}

bool ASideScrollerCharacter::IsDashOnCooldown() const
{
  return GetWorld()->GetTimerManager().GetTimerRemaining(DashCooldownTimer) > 0.0f;
}

void ASideScrollerCharacter::UpdateIsTouchingWall()
{
  bool bPrevIsTouchingWall = bIsTouchingWall;

  FVector boxExtent = WallDetectionBox->GetUnscaledBoxExtent();
  FVector boxLocation = (GetActorForwardVector() * WallDetectionBox->GetRelativeLocation()) + GetActorLocation();
  FCollisionShape wallJumpCollisionBox = FCollisionShape::MakeBox(boxExtent);

  FHitResult OutHit;

  bIsTouchingWall = GetWorld()->SweepSingleByChannel(
    OutHit,
    boxLocation,
    boxLocation,
    GetActorRotation().Quaternion(),
    ECollisionChannel::ECC_GameTraceChannel1,
    wallJumpCollisionBox
  );

  if (!bIsTouchingWall && bPrevIsTouchingWall)
  {
    GetWorldTimerManager().SetTimer(
      WallCoyoteTimer,
      this,
      &ASideScrollerCharacter::OnWallCoyoteTimerEnd,
      WallCoyoteTime
    );
  }
}
