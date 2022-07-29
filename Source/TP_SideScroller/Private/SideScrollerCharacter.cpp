#include "SideScrollerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ASideScrollerCharacter::ASideScrollerCharacter(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;

  bUseControllerRotationYaw = false;
  bUseControllerRotationPitch = false;
  bUseControllerRotationRoll = false;

  JumpMaxHoldTime = 0.35f;
  JumpMaxCount = 1;
  
  GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
  GetCharacterMovement()->bConstrainToPlane = true;
  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->bUseControllerDesiredRotation = false;
  GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f);
  GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
  GetCharacterMovement()->GravityScale = 3.0f;
  GetCharacterMovement()->AirControl = 1.0f;
  GetCharacterMovement()->JumpZVelocity = 500.0f;
  GetCharacterMovement()->GroundFriction = 50.0f;
  GetCharacterMovement()->FallingLateralFriction = 1.0f;
  GetCharacterMovement()->MaxAcceleration = 8000.0f;

  GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);

  WallDetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Jumpable Wall Trigger"));
  WallDetectionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
  WallDetectionBox->SetWorldLocation(FVector(25.0f, 0.0f, 0.0f));
  WallDetectionBox->SetBoxExtent(FVector(25.0, 25.0, 50.0f));
  WallDetectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  WallDetectionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
  WallDetectionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);

  InitCharacter();

  SetReplicates(true);
  SetReplicateMovement(true);
}

void ASideScrollerCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (GetVelocity().Z < 0)
  {
    GetCharacterMovement()->GravityScale = OriginalGravityScale * FallingGravityScaleModifier;

    if (bCanWallSlide && bIsTouchingWall)
    {
      GetCharacterMovement()->Velocity.Z = -WallSlideZVelocity;
    }
  }
  else
  {
    GetCharacterMovement()->GravityScale = OriginalGravityScale;
  }
}

void ASideScrollerCharacter::NotifyJumpApex()
{
  Super::NotifyJumpApex();
  
  
}

void ASideScrollerCharacter::Landed(const FHitResult& Hit)
{
  Super::Landed(Hit);

  if (bIsDashing)
  {
    StopDash();
  }
  
  DashesInAirRemaining = MaxDashesInAir;
}

void ASideScrollerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
  Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

  if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
  {
    if (PrevMovementMode == EMovementMode::MOVE_Walking)
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
  if (bIsTouchingWall && GetCharacterMovement()->IsFalling())
  {
    if (bFaceWallJumpDirection)
    {
      Move(-GetActorForwardVector().X);
    }
    
    WallJump();
  }
  else
  {
    Jump();
  }
}

void ASideScrollerCharacter::WallJump_Implementation()
{
  FVector LaunchBackVelocity = -GetActorForwardVector() * LaunchOffWallVelocity.X;
  FVector LaunchUpVelocity = GetActorUpVector() * LaunchOffWallVelocity.Z;
  
  LaunchCharacter((LaunchBackVelocity + LaunchUpVelocity), true, true);
}

bool ASideScrollerCharacter::CanDash() const
{
  if (!bIsDashing && GetWorld()->GetTimerManager().GetTimerRemaining(DashCooldownTimer) <= 0.0f)
  {
    if (GetCharacterMovement()->IsFalling() && DashesInAirRemaining <= 0)
    {
      return false;
    }

    return true;
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

  WallDetectionBox->OnComponentBeginOverlap.AddDynamic(this, &ASideScrollerCharacter::OnJumpableWallTouch);
  WallDetectionBox->OnComponentEndOverlap.AddDynamic(this, &ASideScrollerCharacter::OnJumpableWallLeave);
}

void ASideScrollerCharacter::InitCharacter()
{
  OriginalGravityScale = GetCharacterMovement()->GravityScale;
  DashesInAirRemaining = MaxDashesInAir;
}

void ASideScrollerCharacter::Jump()
{
  GetCharacterMovement()->bNotifyApex = true;

  Super::Jump();
}

bool ASideScrollerCharacter::CanJumpInternal_Implementation() const
{
  if (GetWorld()->GetTimerManager().GetTimerRemaining(CoyoteTimer) > 0.0f) {
    return true;
  }

  return Super::CanJumpInternal_Implementation();
}

void ASideScrollerCharacter::Dash()
{
  if (CanDash())
  {
    FVector Direction = GetActorForwardVector() + GetActorUpVector();

    bIsDashing = true;

    if (GetCharacterMovement()->IsFalling())
    {
      DashesInAirRemaining--;
    }

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

void ASideScrollerCharacter::OnDashDurationTimerEnd()
{
  StopDash();
}

void ASideScrollerCharacter::OnDashCooldownTimerEnd()
{
}

void ASideScrollerCharacter::OnJumpableWallTouch(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  bIsTouchingWall = true;
}

void ASideScrollerCharacter::OnJumpableWallLeave(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  bIsTouchingWall = false;
}
