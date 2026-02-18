// Copyright Epic Games, Inc. All Rights Reserved.

#include "OATrapFloor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

AOATrapFloor::AOATrapFloor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Platform mesh (flat cube: 300x300x20)
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(SceneRoot);
	PlatformMesh->SetCollisionProfileName(TEXT("BlockAll"));
	PlatformMesh->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'")
	);
	if (CubeAsset.Succeeded())
	{
		PlatformMesh->SetStaticMesh(CubeAsset.Object);
	}

	// Default cube is 100x100x100. Scale to flat platform shape.
	PlatformMesh->SetRelativeScale3D(FVector(3.0f, 3.0f, 0.2f));

	// Overlap box for player detection (above platform surface)
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(SceneRoot);
	OverlapBox->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f));
	OverlapBox->SetBoxExtent(FVector(150.0f, 150.0f, 30.0f));
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBox->SetGenerateOverlapEvents(true);
}

void AOATrapFloor::BeginPlay()
{
	Super::BeginPlay();

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AOATrapFloor::OnOverlapBegin);
}

void AOATrapFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsFalling)
	{
		return;
	}

	// Accelerate downward (gravity: 980 cm/s²)
	FallSpeed += 980.0f * DeltaTime;
	const float DeltaFall = FallSpeed * DeltaTime;
	AddActorWorldOffset(FVector(0.0f, 0.0f, -DeltaFall));

	FallDistance += DeltaFall;
	if (FallDistance > 1000.0f)
	{
		Destroy();
	}
}

void AOATrapFloor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggered)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	bTriggered = true;
	GetWorldTimerManager().SetTimer(FallTimerHandle, this, &AOATrapFloor::StartFalling, FallDelay, false);
}

void AOATrapFloor::StartFalling()
{
	// Disable collision so the player falls through
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bIsFalling = true;
	SetActorTickEnabled(true);
}
