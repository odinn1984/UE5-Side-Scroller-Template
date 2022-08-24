#pragma once

#include "CoreMinimal.h"
#include "Pixel2DCharacter.h"
#include "SideScrollerCharacter.h"
#include "Pixel2DSideScrollerCharacter.generated.h"

UCLASS()
class TP_SIDESCROLLER_API APixel2DSideScrollerCharacter : public ASideScrollerCharacter
{
	GENERATED_BODY()

public:
	APixel2DSideScrollerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(Category = "Character", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  class UPixel2DComponent* PixelComponent;
};
