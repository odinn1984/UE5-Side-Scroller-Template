#pragma once

#include "CoreMinimal.h"
#include "SideScrollerCharacter.h"
#include "PaperCharacter.h"
#include "PaperSideScrollerCharacter.generated.h"

UCLASS()
class APaperSideScrollerCharacter : public ASideScrollerCharacter
{
	GENERATED_BODY()

public:
	APaperSideScrollerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class UPaperFlipbookComponent* Sprite;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  class USMStateMachineComponent* AnimationStateMachine;
};
