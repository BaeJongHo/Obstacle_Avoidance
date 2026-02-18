// Copyright Epic Games, Inc. All Rights Reserved.

#include "OAJumpPad.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/Character.h"
#include "Obstacle_AvoidanceCharacter.h"

AOAJumpPad::AOAJumpPad()
{
	PrimaryActorTick.bCanEverTick = false;

	// Visual mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Load static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")
	);
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
	}

	// Load material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(
		TEXT("/Script/Engine.Material'/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial'")
	);
	if (MaterialAsset.Succeeded())
	{
		Mesh->SetMaterial(0, MaterialAsset.Object);
	}

	Mesh->SetWorldScale3D(FVector(1.5f, 1.5f, 0.3f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);

	// Overlap trigger box - positioned above the mesh surface
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(Mesh);
	OverlapBox->SetBoxExtent(FVector(80.0f, 80.0f, 30.0f));
	OverlapBox->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBox->SetGenerateOverlapEvents(true);
}

void AOAJumpPad::BeginPlay()
{
	Super::BeginPlay();

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AOAJumpPad::OnOverlapBegin);
}

void AOAJumpPad::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	AObstacle_AvoidanceCharacter* OACharacter = Cast<AObstacle_AvoidanceCharacter>(Character);
	if (OACharacter)
	{
		OACharacter->SetJumpPadLaunched();
	}

	const FVector LaunchVelocity(0.0f, 0.0f, LaunchForce);
	Character->LaunchCharacter(LaunchVelocity, false, bOverrideZVelocity);
}
