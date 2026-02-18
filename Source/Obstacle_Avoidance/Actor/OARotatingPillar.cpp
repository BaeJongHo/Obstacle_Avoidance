// Copyright Epic Games, Inc. All Rights Reserved.

#include "OARotatingPillar.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"

AOARotatingPillar::AOARotatingPillar()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Center axis pillar (vertical cylinder)
	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	PillarMesh->SetupAttachment(SceneRoot);
	PillarMesh->SetMobility(EComponentMobility::Movable);
	PillarMesh->SetCollisionProfileName(TEXT("BlockAll"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")
	);
	if (CylinderAsset.Succeeded())
	{
		PillarMesh->SetStaticMesh(CylinderAsset.Object);
	}

	// Default cylinder: 100cm tall, centered at origin. Position so bottom sits at Z=0.
	PillarMesh->SetRelativeLocation(FVector(0.0f, 0.0f, PillarHeight * 0.5f));
	PillarMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, PillarHeight / 100.0f));

	// Rotation pivot at the top of the pillar
	RotatingRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RotatingRoot"));
	RotatingRoot->SetupAttachment(SceneRoot);
	RotatingRoot->SetRelativeLocation(FVector(0.0f, 0.0f, PillarHeight));
	RotatingRoot->SetMobility(EComponentMobility::Movable);

	// Horizontal arm mesh - rotated 90 deg on Pitch so cylinder lies flat along X
	ArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(RotatingRoot);
	if (CylinderAsset.Succeeded())
	{
		ArmMesh->SetStaticMesh(CylinderAsset.Object);
	}
	ArmMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	ArmMesh->SetRelativeLocation(FVector(ArmLength * 0.5f, 0.0f, 0.0f));
	ArmMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, ArmLength / 100.0f));
	ArmMesh->SetMobility(EComponentMobility::Movable);
	ArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Hit collision box wrapping the arm
	HitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HitCollision"));
	HitCollision->SetupAttachment(RotatingRoot);
	HitCollision->SetRelativeLocation(FVector(ArmLength * 0.5f, 0.0f, 0.0f));
	HitCollision->SetBoxExtent(FVector(ArmLength * 0.5f, 30.0f, 30.0f));
	HitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitCollision->SetCollisionObjectType(ECC_WorldDynamic);
	HitCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitCollision->SetGenerateOverlapEvents(true);
}

void AOARotatingPillar::BeginPlay()
{
	Super::BeginPlay();

	UpdatePillarLayout();

	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &AOARotatingPillar::OnHitCollisionOverlapBegin);
}

void AOARotatingPillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Clockwise = negative yaw (top-down view)
	const float Direction = bClockwise ? -1.0f : 1.0f;
	const float DeltaYaw = RotationSpeed * Direction * DeltaTime;

	RotatingRoot->AddRelativeRotation(FRotator(0.0f, DeltaYaw, 0.0f));
}

#if WITH_EDITOR
void AOARotatingPillar::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdatePillarLayout();
}
#endif

void AOARotatingPillar::UpdatePillarLayout()
{
	// Pillar: center at half height, scale Z to match height
	PillarMesh->SetRelativeLocation(FVector(0.0f, 0.0f, PillarHeight * 0.5f));
	PillarMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, PillarHeight / 100.0f));

	// Rotation pivot sits on top of the pillar
	RotatingRoot->SetRelativeLocation(FVector(0.0f, 0.0f, PillarHeight));

	// Arm: offset by half length so one end connects to the pillar top
	ArmMesh->SetRelativeLocation(FVector(ArmLength * 0.5f, 0.0f, 0.0f));
	ArmMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, ArmLength / 100.0f));

	// Collision box matches arm position and size
	HitCollision->SetRelativeLocation(FVector(ArmLength * 0.5f, 0.0f, 0.0f));
	HitCollision->SetBoxExtent(FVector(ArmLength * 0.5f, 30.0f, 30.0f));
}

void AOARotatingPillar::OnHitCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (!HitCharacter)
	{
		return;
	}

	// Knockback direction: from pillar center outward toward player
	const FVector PillarLocation = GetActorLocation();
	const FVector PlayerLocation = HitCharacter->GetActorLocation();
	FVector KnockbackDirection = (PlayerLocation - PillarLocation).GetSafeNormal2D();

	// Add slight upward component for natural knockback feel
	KnockbackDirection.Z = 0.3f;
	KnockbackDirection.Normalize();

	HitCharacter->LaunchCharacter(KnockbackDirection * KnockbackForce, true, true);
}
