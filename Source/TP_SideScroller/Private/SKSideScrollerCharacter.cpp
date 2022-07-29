#include "SKSideScrollerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ASKSideScrollerCharacter::ASKSideScrollerCharacter(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;

  USkeletalMesh* DefaultMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
  UAnimBlueprint* DefaultAnimationBP = LoadObject<UAnimBlueprint>(nullptr, TEXT("AnimBlueprint'/Game/Characters/Mannequins/Animations/ABP_Quinn.ABP_Quinn'"));

  if (DefaultMesh != nullptr)
  {
    GetMesh()->SetSkeletalMesh(DefaultMesh);
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
    GetMesh()->SetWorldScale3D(FVector(1.35f, 1.35f, 1.35f));
  }

  if (DefaultAnimationBP != nullptr)
  {
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    GetMesh()->SetAnimInstanceClass(DefaultAnimationBP->GetAnimBlueprintGeneratedClass());
  }

  InitCharacter();
}

void ASKSideScrollerCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void ASKSideScrollerCharacter::BeginPlay()
{
  Super::BeginPlay();
}
