// Copyright Epic Games, Inc. All Rights Reserved.

#include "OAMovingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AOAMovingPlatform::AOAMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Load the platform static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_Plane.SM_Plane'")
	);
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
	}

	// Platform should block all
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void AOAMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	CurrentDistance = 0.0f;
	Direction = 1.0f;
}

void AOAMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveDistance <= 0.0f || MoveSpeed <= 0.0f)
	{
		return;
	}

	// Advance along the path
	CurrentDistance += MoveSpeed * DeltaTime * Direction;

	// Reverse direction when reaching either end
	if (CurrentDistance >= MoveDistance)
	{
		CurrentDistance = MoveDistance;
		Direction = -1.0f;
	}
	else if (CurrentDistance <= -MoveDistance)
	{
		CurrentDistance = -MoveDistance;
		Direction = 1.0f;
	}

	// Compute the new location
	FVector NewLocation = StartLocation;
	if (bMoveLeftRight)
	{
		NewLocation.Y += CurrentDistance;
	}
	else
	{
		NewLocation.X += CurrentDistance;
	}

	SetActorLocation(NewLocation);
}
