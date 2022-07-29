#pragma once

#include "CoreMinimal.h"
#include "SideScrollerCharacter.h"
#include "SKSideScrollerCharacter.generated.h"

UCLASS()
class ASKSideScrollerCharacter : public ASideScrollerCharacter
{
	GENERATED_BODY()

public:
	ASKSideScrollerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
