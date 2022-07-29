#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SideScrollerPlayerController.generated.h"

UCLASS()
class ASideScrollerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASideScrollerPlayerController();

	virtual void SetupInputComponent() override;

	virtual void Move(const struct FInputActionValue& Value);
	virtual void StartJumping(const struct FInputActionValue& Value);
	virtual void StopJumping(const struct FInputActionValue& Value);
	virtual void StartDashing(const struct FInputActionValue& Value);
	virtual void StopDashing(const struct FInputActionValue& Value);

private:
	UPROPERTY(Category = "Input|Actions", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UInputAction* MoveInputAction;

	UPROPERTY(Category = "Input|Actions", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UInputAction* StartJumpInputAction;

	UPROPERTY(Category = "Input|Actions", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UInputAction* StopJumpInputAction;

	UPROPERTY(Category = "Input|Actions", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UInputAction* StartDashingAction;

	UPROPERTY(Category = "Input|Actions", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UInputAction* StopDashingAction;

	UPROPERTY(Category = "Input|Context", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UInputMappingContext* DefaultInputContext;
};
