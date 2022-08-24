#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SideScrollerCharacter.generated.h"

UCLASS()
class ASideScrollerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASideScrollerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void NotifyJumpApex() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Category = "Character", BlueprintCallable)
	virtual void Move(float Value);

  UFUNCTION(Category = "Character", BlueprintCallable)
	virtual void StartJumping();

	UFUNCTION(Category = "Character", BlueprintCallable)
	virtual bool CanWallJump();

	virtual void StopJumping() override;

	UFUNCTION(Category = "Character", BlueprintCallable, Server, Reliable)
	virtual void WallJump();

	UFUNCTION(Category = "Character", BlueprintCallable)
	virtual bool CanDash() const;

	UFUNCTION(Category = "Character", BlueprintCallable, Server, Reliable)
	virtual void StartDashing();

	UFUNCTION(Category = "Character", BlueprintCallable, Server, Reliable)
	virtual void StopDashing();

protected:
	virtual void BeginPlay() override;
	virtual void InitCharacter();

	virtual void Jump() override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void Dash();
	virtual void StopDash();

	virtual void OnCoyoteTimerEnd();
	virtual void OnWallCoyoteTimerEnd();
	virtual void OnDashDurationTimerEnd();
	virtual void OnDashCooldownTimerEnd();

	bool IsCoyoteTime() const;
	bool IsWallCoyoteTime() const;
	bool IsJumpBufferTime() const;
	bool IsDashOnCooldown() const;

	virtual void UpdateIsTouchingWall();

	UPROPERTY(Category = "Character|Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float StopJumpGravityScaleModifier = 2.0f;

	UPROPERTY(Category = "Character|Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", ClampMax = "0.3", UIMax = "0.3"))
	float CoyoteTime = 0.1f;

	UPROPERTY(Category = "Character|Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", ClampMax = "0.3", UIMax = "0.3"))
  float JumpBufferTime = 0.2f;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bCanDash = true;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bStopDashOnButtonRelease = false;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bHideCharacterOnDash = true;

	UPROPERTY(Category = "Character|Dash", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bIsDashing = false;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  int32 MaxDashesInAir = 1;

	UPROPERTY(Category = "Character|Dash", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  int32 DashesInAirRemaining;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  float DashDuration = 0.1f;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  float DashCooldown = 0.2f;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  FVector DashInitialVelocity = FVector(5000.0f, 0.0f, 100.0f);

	UPROPERTY(Category = "Character|Wall Slide/Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UBoxComponent* WallDetectionBox;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Slide", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bCanWallSlide = true;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Slide", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  float WallSlideZVelocity = 250.0f;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bCanWallJump = true;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", ClampMax = "0.3", UIMax = "0.3"))
  float WallCoyoteTime = 0.3f;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bFaceWallJumpDirection = true;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector LaunchOffWallVelocity = FVector(1000.0f, 0.0f, 1250.0f);

	bool bWallJumpRequested = false;
	bool bIsTouchingWall = false;
	bool bIsSlidingOnWall = false;

	float OriginalGravityScale = 1.0f;

	FTimerHandle CoyoteTimer;
	FTimerHandle WallCoyoteTimer;
	FTimerHandle DashCooldownTimer;
	FTimerHandle DashDurationTimer;
	FTimerHandle JumpBufferTimer;
};
