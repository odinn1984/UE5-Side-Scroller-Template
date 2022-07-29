#include "SideScrollerPlayerCameraManager.h"
#include "Components/BoxComponent.h"

ASideScrollerPlayerCameraManager::ASideScrollerPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
	SetFOV(90.0f);
	SetOrthoWidth(2000.0f);

	Deadzone = CreateDefaultSubobject<UBoxComponent>(TEXT("Deadzone"));
	Deadzone->SetBoxExtent(FVector(128.0f, 0.0f, 64.0f));
	Deadzone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Deadzone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ASideScrollerPlayerCameraManager::BeginPlay()
{
  Super::BeginPlay();

	FMinimalViewInfo InitialPOV;

	InitialPOV.Location = CameraPosition + Offset;
	InitialPOV.Rotation = CameraRotation;
	InitialPOV.ProjectionMode = ECameraProjectionMode::Perspective;

	SetCameraCachePOV(InitialPOV);
}

void ASideScrollerPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
  Super::UpdateViewTargetInternal(OutVT, DeltaTime);

	FVector CurrentLocation = GetCameraLocation();
	FVector NewLocation = CameraPosition + Offset;

	if (bEnableFollow)
	{
		float NewXLocation = GetNewXLocation();
		float NewZLocation = GetNewZLocation();

		if (OutVT.Target->GetVelocity().X == 0.0f)
		{
			NewXLocation = OutVT.Target->GetActorLocation().X;
		}

		if (OutVT.Target->GetVelocity().Z == 0.0f)
		{
			NewZLocation = OutVT.Target->GetActorLocation().Z;
		}

    NewLocation = FVector(
			NewXLocation,
			CurrentLocation.Y,
			NewZLocation
		) + Offset;
	}

	OutVT.POV.Location = FMath::Lerp<FVector>(
		CurrentLocation,
		NewLocation,
		SmoothingSpeed * DeltaTime);
	OutVT.POV.Rotation = GetCameraRotation();
	OutVT.POV.ProjectionMode = GetCameraCachePOV().ProjectionMode;
}

float ASideScrollerPlayerCameraManager::GetNewXLocation() const
{
	FVector CurrentLocation = GetCameraLocation();

	if (GetViewTarget() == nullptr)
	{
		return CurrentLocation.X;
	}

	FVector TargetLocation = GetViewTarget()->GetActorLocation();

	if (!bEnableDeadzoneHorizontal || !bEnableFollow)
	{
		return TargetLocation.X;
	}

	if (TargetLocation.X - Deadzone->GetScaledBoxExtent().X > CurrentLocation.X)
	{
		return TargetLocation.X - Deadzone->GetScaledBoxExtent().X;
	}
	else if (TargetLocation.X + Deadzone->GetScaledBoxExtent().X < CurrentLocation.X)
	{
		return TargetLocation.X + Deadzone->GetScaledBoxExtent().X;
	}

	return CurrentLocation.X;
}

float ASideScrollerPlayerCameraManager::GetNewZLocation() const
{
	FVector CurrentLocation = GetCameraLocation();

	if (GetViewTarget() == nullptr)
	{
		return CurrentLocation.Z;
	}

	FVector TargetLocation = GetViewTarget()->GetActorLocation();

	if (!bEnableDeadzoneVertical || !bEnableFollow)
	{
		return TargetLocation.Z;
	}

	if (bFixedZ)
	{
		return CurrentLocation.Z;
	}

	if (TargetLocation.Z - Deadzone->GetScaledBoxExtent().Z > CurrentLocation.Z)
	{
		return TargetLocation.Z - Deadzone->GetScaledBoxExtent().Z;
	}
	else if (TargetLocation.Z + Deadzone->GetScaledBoxExtent().Z < CurrentLocation.Z)
	{
		return TargetLocation.Z + Deadzone->GetScaledBoxExtent().Z;
	}

	return CurrentLocation.Z;
}
