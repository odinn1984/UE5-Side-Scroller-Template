#include "PaperSideScrollerCharacter.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SMStateMachineComponent.h"
#include "Blueprints/SMBlueprint.h"

APaperSideScrollerCharacter::APaperSideScrollerCharacter(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName))
{
  PrimaryActorTick.bCanEverTick = true;

  Sprite = CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));

  if (Sprite != nullptr)
  {
    Sprite->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    Sprite->SetWorldScale3D(0.4f * FVector::OneVector);
    Sprite->SetWorldLocation(FVector(0.0f, 0.0f, -5.0f));
    Sprite->SetCollisionProfileName(TEXT("CharacterMesh"));
    Sprite->SetGenerateOverlapEvents(false);
    Sprite->SetIsReplicated(true);

    Sprite->AlwaysLoadOnClient = true;
    Sprite->AlwaysLoadOnServer = true;
    Sprite->bOwnerNoSee = false;
    Sprite->bAffectDynamicIndirectLighting = true;
    Sprite->PrimaryComponentTick.TickGroup = TG_PrePhysics;

    UPaperFlipbook* DefaultFlipbook = LoadObject<UPaperFlipbook>(
      nullptr,
      TEXT("PaperFlipbook'/Game/Characters/SwordHero/Animations/Flipbooks/Flipbook_Sword_Hero_Idle.Flipbook_Sword_Hero_Idle'"));

    if (DefaultFlipbook != nullptr)
    {
      Sprite->SetFlipbook(DefaultFlipbook);
    }
  }

  AnimationStateMachine = CreateOptionalDefaultSubobject<USMStateMachineComponent>(TEXT("Animation State Machine"));
  AnimationStateMachine->bStartOnBeginPlay = true;
  AnimationStateMachine->bInitializeOnBeginPlay = true;
  AnimationStateMachine->SetIsReplicated(true);

  USMBlueprint* DefaultAnimSM = LoadObject<USMBlueprint>(
    nullptr,
    TEXT("SMBlueprint'/Game/Characters/SwordHero/Animations/BP_SwordHero_AnimationStateMachine.BP_SwordHero_AnimationStateMachine'"));

  if (DefaultAnimSM != nullptr)
  {
    AnimationStateMachine->StateMachineClass = DefaultAnimSM->GeneratedClass;
  }
}

void APaperSideScrollerCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void APaperSideScrollerCharacter::PostInitializeComponents()
{
  Super::PostInitializeComponents();

  if (IsValid(this))
  {
    if (Sprite)
    {
      if (Sprite->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
      {
        Sprite->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
      }
    }
  }
}

void APaperSideScrollerCharacter::BeginPlay()
{
  Super::BeginPlay();
}
