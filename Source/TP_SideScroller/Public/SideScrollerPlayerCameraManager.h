#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "SideScrollerPlayerCameraManager.generated.h"

UCLASS()
class ASideScrollerPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ASideScrollerPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

	virtual float GetNewXLocation() const;
	virtual float GetNewZLocation() const;

	UPROPERTY(Category = "Camera", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  FVector CameraPosition = FVector(0.0f, 1000.0f, 0.0f);

	UPROPERTY(Category = "Camera", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator CameraRotation = FRotator(0.0f, -90.0f, 0.0f);

	UPROPERTY(Category = "Camera", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  FVector Offset = FVector::ZeroVector;

	UPROPERTY(Category = "Follow", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bEnableFollow = true;

	UPROPERTY(Category = "Follow", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", ClampMax = "10.0", UIMax = "10.0"))
  float SmoothingSpeed = 6.9f;

	UPROPERTY(Category = "Follow|Deadzone", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UBoxComponent* Deadzone;

	UPROPERTY(Category = "Follow|Deadzone", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bEnableDeadzoneHorizontal = true;

	UPROPERTY(Category = "Follow|Deadzone", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bEnableDeadzoneVertical = true;

	UPROPERTY(Category = "Follow|Deadzone", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  bool bFixedZ = false;
};
