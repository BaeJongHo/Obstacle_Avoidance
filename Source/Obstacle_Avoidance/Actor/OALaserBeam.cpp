// Copyright Epic Games, Inc. All Rights Reserved.

#include "OALaserBeam.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "Obstacle_AvoidanceCharacter.h"
#include "TimerManager.h"

AOALaserBeam::AOALaserBeam()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")
	);

	// ── Left pillar ──
	LeftPillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftPillar"));
	LeftPillar->SetupAttachment(SceneRoot);
	if (CylinderAsset.Succeeded())
	{
		LeftPillar->SetStaticMesh(CylinderAsset.Object);
	}
	// Thin, tall pillar (30cm diameter, 200cm tall). Bottom at Z=0.
	LeftPillar->SetRelativeScale3D(FVector(0.3f, 0.3f, 2.0f));
	LeftPillar->SetRelativeLocation(FVector(0.0f, -PillarDistance * 0.5f, 100.0f));
	LeftPillar->SetCollisionProfileName(TEXT("BlockAll"));

	// ── Right pillar ──
	RightPillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightPillar"));
	RightPillar->SetupAttachment(SceneRoot);
	if (CylinderAsset.Succeeded())
	{
		RightPillar->SetStaticMesh(CylinderAsset.Object);
	}
	RightPillar->SetRelativeScale3D(FVector(0.3f, 0.3f, 2.0f));
	RightPillar->SetRelativeLocation(FVector(0.0f, PillarDistance * 0.5f, 100.0f));
	RightPillar->SetCollisionProfileName(TEXT("BlockAll"));

	// ── Beam mesh (thin cube stretched between pillars) ──
	BeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeamMesh"));
	BeamMesh->SetupAttachment(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'")
	);
	if (CubeAsset.Succeeded())
	{
		BeamMesh->SetStaticMesh(CubeAsset.Object);
	}
	BeamMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	BeamMesh->SetRelativeScale3D(FVector(0.05f, PillarDistance / 100.0f, 0.05f));
	BeamMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeamMesh->SetMobility(EComponentMobility::Movable);

	// ── Beam collision ──
	BeamCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BeamCollision"));
	BeamCollision->SetupAttachment(SceneRoot);
	BeamCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	BeamCollision->SetBoxExtent(FVector(20.0f, PillarDistance * 0.5f, 20.0f));
	BeamCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BeamCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BeamCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BeamCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BeamCollision->SetGenerateOverlapEvents(true);
}

void AOALaserBeam::BeginPlay()
{
	Super::BeginPlay();

	UpdateLayout();

	BeamCollision->OnComponentBeginOverlap.AddDynamic(this, &AOALaserBeam::OnBeamOverlapBegin);

	// Start beam cycle timer (toggles every half-cycle)
	GetWorldTimerManager().SetTimer(BeamTimerHandle, this, &AOALaserBeam::ToggleBeam, BeamCycle * 0.5f, true);
}

#if WITH_EDITOR
void AOALaserBeam::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateLayout();
}
#endif

void AOALaserBeam::ToggleBeam()
{
	bBeamActive = !bBeamActive;

	BeamMesh->SetVisibility(bBeamActive);
	BeamCollision->SetCollisionEnabled(bBeamActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AOALaserBeam::UpdateLayout()
{
	// Pillar positions
	LeftPillar->SetRelativeLocation(FVector(0.0f, -PillarDistance * 0.5f, 100.0f));
	RightPillar->SetRelativeLocation(FVector(0.0f, PillarDistance * 0.5f, 100.0f));

	// Beam mesh scale (Y stretches to fill pillar gap)
	BeamMesh->SetRelativeScale3D(FVector(0.05f, PillarDistance / 100.0f, 0.05f));

	// Beam collision extent
	BeamCollision->SetBoxExtent(FVector(20.0f, PillarDistance * 0.5f, 20.0f));
}

void AOALaserBeam::OnBeamOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	AObstacle_AvoidanceCharacter* Character = Cast<AObstacle_AvoidanceCharacter>(OtherActor);
	if (Character && !Character->IsDead())
	{
		Character->Die();
	}
}
