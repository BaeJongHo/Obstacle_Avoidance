// Copyright Epic Games, Inc. All Rights Reserved.

#include "OAIceSurface.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AOAIceSurface::AOAIceSurface()
{
	PrimaryActorTick.bCanEverTick = false;

	// Visual mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Load the road static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Script/Engine.StaticMesh'/Game/RoadBlockoutKit/Meshes/SM_Road.SM_Road'")
	);
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
	}

	// Load material instance for blue ice look
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(
		TEXT("/Script/Engine.MaterialInstanceConstant'/Game/RoadBlockoutKit/Materials/MI_Blocking_WoodenFence.MI_Blocking_WoodenFence'")
	);
	if (MaterialAsset.Succeeded())
	{
		Mesh->SetMaterial(0, MaterialAsset.Object);
	}

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);

	// Overlap trigger box - positioned above the mesh surface
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(Mesh);
	OverlapBox->SetBoxExtent(FVector(500.0f, 500.0f, 50.0f));
	OverlapBox->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBox->SetGenerateOverlapEvents(true);
}

void AOAIceSurface::BeginPlay()
{
	Super::BeginPlay();

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AOAIceSurface::OnOverlapBegin);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &AOAIceSurface::OnOverlapEnd);
}

void AOAIceSurface::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
	if (!Movement)
	{
		return;
	}

	// Cache the character's current movement values before overriding
	TWeakObjectPtr<ACharacter> WeakChar(Character);
	if (!OriginalParamsMap.Contains(WeakChar))
	{
		FOAOriginalMovementParams Params;
		Params.GroundFriction = Movement->GroundFriction;
		Params.BrakingDecelerationWalking = Movement->BrakingDecelerationWalking;
		OriginalParamsMap.Add(WeakChar, Params);
	}

	// Apply ice surface friction values
	Movement->GroundFriction = Friction;
	Movement->BrakingDecelerationWalking = BrakingDeceleration;
}

void AOAIceSurface::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
	if (!Movement)
	{
		return;
	}

	// Restore the character's original movement values
	TWeakObjectPtr<ACharacter> WeakChar(Character);
	if (const FOAOriginalMovementParams* Params = OriginalParamsMap.Find(WeakChar))
	{
		Movement->GroundFriction = Params->GroundFriction;
		Movement->BrakingDecelerationWalking = Params->BrakingDecelerationWalking;
		OriginalParamsMap.Remove(WeakChar);
	}
}
