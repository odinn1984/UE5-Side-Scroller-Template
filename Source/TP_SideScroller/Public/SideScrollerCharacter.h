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
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Category = "Character", BlueprintCallable)
	virtual void Move(float Value);

	UFUNCTION(Category = "Character", BlueprintCallable)
	virtual void StartJumping();

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
	virtual void OnDashDurationTimerEnd();
	virtual void OnDashCooldownTimerEnd();

	UFUNCTION()
	void OnJumpableWallTouch(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnJumpableWallLeave(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(Category = "Character|Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float FallingGravityScaleModifier = 2.0f;

	UPROPERTY(Category = "Character|Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
	float CoyoteTime = 0.3f;

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
  float DashDuration = 0.069f;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  float DashCooldown = 0.2f;

	UPROPERTY(Category = "Character|Dash", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  FVector DashInitialVelocity = FVector(15000.0f, 0.0f, 350.0f);

	UPROPERTY(Category = "Character|Wall Slide/Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UBoxComponent* WallDetectionBox;

	UPROPERTY(Category = "Character|Wall Slide/Jump", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bIsTouchingWall = false;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Slide", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bCanWallSlide = true;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Slide", BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
  bool bIsSlidingOnWall = false;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Slide", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  float WallSlideZVelocity = 250.0f;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bCanWallJump = true;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bFaceWallJumpDirection = true;

	UPROPERTY(Category = "Character|Wall Slide/Jump|Wall Jump", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector LaunchOffWallVelocity = FVector(1250.0f, 0.0f, 1500.0f);

	float OriginalGravityScale = 1.0f;

	FTimerHandle CoyoteTimer;
	FTimerHandle DashCooldownTimer;
	FTimerHandle DashDurationTimer;

};
