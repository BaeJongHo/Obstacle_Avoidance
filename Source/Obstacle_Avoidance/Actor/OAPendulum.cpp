// Copyright Epic Games, Inc. All Rights Reserved.

#include "OAPendulum.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"

AOAPendulum::AOAPendulum()
{
	PrimaryActorTick.bCanEverTick = true;

	// Pivot point - the fixed anchor at the top
	PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PivotPoint"));
	SetRootComponent(PivotPoint);

	// Swing arm - this component rotates to create the pendulum motion
	SwingArm = CreateDefaultSubobject<USceneComponent>(TEXT("SwingArm"));
	SwingArm->SetupAttachment(PivotPoint);

	// Pendulum mesh - offset downward by ArmLength
	PendulumMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PendulumMesh"));
	PendulumMesh->SetupAttachment(SwingArm);
	PendulumMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -300.0f));
	PendulumMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PendulumMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PendulumMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PendulumMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Load default mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Script/Engine.StaticMesh'/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Light_Post.SM_Obstacle_Light_Post'")
	);
	if (MeshAsset.Succeeded())
	{
		PendulumMesh->SetStaticMesh(MeshAsset.Object);
		DefaultMesh = MeshAsset.Object;
	}

	// Hit collision sphere around the pendulum head
	HitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("HitCollision"));
	HitCollision->SetupAttachment(PendulumMesh);
	HitCollision->SetSphereRadius(100.0f);
	HitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitCollision->SetCollisionObjectType(ECC_WorldDynamic);
	HitCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitCollision->SetGenerateOverlapEvents(true);
}

void AOAPendulum::BeginPlay()
{
	Super::BeginPlay();

	// Ensure mesh is assigned (BP subclass may have cleared the CDO value)
	if (PendulumMesh && !PendulumMesh->GetStaticMesh() && DefaultMesh)
	{
		PendulumMesh->SetStaticMesh(DefaultMesh);
	}

	// Update mesh offset to match ArmLength
	PendulumMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -ArmLength));

	// Bind overlap event
	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &AOAPendulum::OnHitCollisionOverlapBegin);
}

void AOAPendulum::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Accumulate time
	ElapsedTime += DeltaTime * SwingSpeed;

	// Calculate current angle using sine wave: angle = SwingAngle * sin(time)
	const float CurrentAngle = SwingAngle * FMath::Sin(ElapsedTime);

	// Apply rotation to the swing arm (rotate around Y axis for forward/backward swing)
	SwingArm->SetRelativeRotation(FRotator(CurrentAngle, 0.0f, 0.0f));
}

void AOAPendulum::OnHitCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (!HitCharacter)
	{
		return;
	}

	// Calculate push direction: from pendulum center to player
	const FVector PendulumLocation = PendulumMesh->GetComponentLocation();
	const FVector PlayerLocation = HitCharacter->GetActorLocation();
	FVector PushDirection = (PlayerLocation - PendulumLocation).GetSafeNormal2D();

	// Add upward component
	PushDirection.Z = 0.3f;
	PushDirection.Normalize();

	HitCharacter->LaunchCharacter(PushDirection * PushForce, true, true);
}
