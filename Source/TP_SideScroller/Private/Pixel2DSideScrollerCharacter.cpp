#include "Pixel2DSideScrollerCharacter.h"
#include "Pixel2DComponent.h"
#include "Pixel2DAnimBlueprint.h"
#include "GameFramework/CharacterMovementComponent.h"

APixel2DSideScrollerCharacter::APixel2DSideScrollerCharacter(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName))
{
  PrimaryActorTick.bCanEverTick = true;

  PixelComponent = CreateOptionalDefaultSubobject<UPixel2DComponent>(TEXT("Pixel2D"));

  if (PixelComponent != nullptr)
  {
    PixelComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    PixelComponent->SetWorldScale3D(0.4f * FVector::OneVector);
    PixelComponent->SetWorldLocation(FVector(0.0f, 0.0f, -5.0f));
    PixelComponent->SetCollisionProfileName(TEXT("CharacterMesh"));
    PixelComponent->SetGenerateOverlapEvents(false);
    PixelComponent->SetIsReplicated(true);

    PixelComponent->AlwaysLoadOnClient = true;
    PixelComponent->AlwaysLoadOnServer = true;
    PixelComponent->bOwnerNoSee = false;
    PixelComponent->bAffectDynamicIndirectLighting = true;
    PixelComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;

    UPixel2DAnimBlueprint* DefaultAnimationBP =
      LoadObject<UPixel2DAnimBlueprint>(nullptr, TEXT("Pixel2DAnimBlueprint'/Game/Characters/SwordHero/Animations/SwordHero_AnimBP.SwordHero_AnimBP'"));

    if (DefaultAnimationBP != nullptr)
    {
      PixelComponent->SetAnimInstance(DefaultAnimationBP->GeneratedClass);
    }
  }
}

void APixel2DSideScrollerCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void APixel2DSideScrollerCharacter::PostInitializeComponents()
{
  Super::PostInitializeComponents();

  if (IsValid(this))
  {
    if (PixelComponent)
    {
      if (PixelComponent->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
      {
        PixelComponent->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
      }
    }
  }
}

void APixel2DSideScrollerCharacter::BeginPlay()
{
  Super::BeginPlay();
}
