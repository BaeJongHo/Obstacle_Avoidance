// Copyright Epic Games, Inc. All Rights Reserved.

#include "OACannon.h"
#include "OACannonball.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"

AOACannon::AOACannon()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Base pedestal (short, wide cylinder)
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(SceneRoot);
	BaseMesh->SetCollisionProfileName(TEXT("BlockAll"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")
	);
	if (CylinderAsset.Succeeded())
	{
		BaseMesh->SetStaticMesh(CylinderAsset.Object);
	}

	// Default cylinder is 100cm tall. Center at Z=50 so bottom sits at Z=0.
	BaseMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	BaseMesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 1.0f));

	// Barrel pivot at the top of the base
	BarrelPivot = CreateDefaultSubobject<USceneComponent>(TEXT("BarrelPivot"));
	BarrelPivot->SetupAttachment(SceneRoot);
	BarrelPivot->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	BarrelPivot->SetRelativeRotation(FRotator(BarrelAngle, 0.0f, 0.0f));
	BarrelPivot->SetMobility(EComponentMobility::Movable);

	// Barrel mesh (cylinder rotated 90 deg to lie along pivot's X axis)
	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	BarrelMesh->SetupAttachment(BarrelPivot);
	if (CylinderAsset.Succeeded())
	{
		BarrelMesh->SetStaticMesh(CylinderAsset.Object);
	}
	BarrelMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	BarrelMesh->SetRelativeLocation(FVector(BarrelLength * 0.5f, 0.0f, 0.0f));
	BarrelMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, BarrelLength / 100.0f));
	BarrelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BarrelMesh->SetMobility(EComponentMobility::Movable);

	// Muzzle point at the end of the barrel
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(BarrelPivot);
	MuzzlePoint->SetRelativeLocation(FVector(BarrelLength, 0.0f, 0.0f));
}

void AOACannon::BeginPlay()
{
	Super::BeginPlay();

	UpdateCannonLayout();

	// Start periodic firing
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AOACannon::FireCannonball, FireInterval, true);
}

#if WITH_EDITOR
void AOACannon::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateCannonLayout();
}
#endif

void AOACannon::FireCannonball()
{
	const FVector SpawnLocation = MuzzlePoint->GetComponentLocation();
	const FRotator SpawnRotation = MuzzlePoint->GetComponentRotation();
	const FVector LaunchDirection = BarrelPivot->GetForwardVector();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AOACannonball* Cannonball = GetWorld()->SpawnActor<AOACannonball>(
		AOACannonball::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

	if (Cannonball)
	{
		Cannonball->Initialize(LaunchDirection, LaunchSpeed, KnockbackForce);
	}
}

void AOACannon::UpdateCannonLayout()
{
	// Barrel pivot angle
	BarrelPivot->SetRelativeRotation(FRotator(BarrelAngle, 0.0f, 0.0f));

	// Barrel mesh: centered along barrel length
	BarrelMesh->SetRelativeLocation(FVector(BarrelLength * 0.5f, 0.0f, 0.0f));
	BarrelMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, BarrelLength / 100.0f));

	// Muzzle at the end of the barrel
	MuzzlePoint->SetRelativeLocation(FVector(BarrelLength, 0.0f, 0.0f));
}
