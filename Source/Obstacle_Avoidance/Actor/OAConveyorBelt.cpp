// Copyright Epic Games, Inc. All Rights Reserved.

#include "OAConveyorBelt.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/Character.h"

AOAConveyorBelt::AOAConveyorBelt()
{
	PrimaryActorTick.bCanEverTick = true;

	// Visual mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Load static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'")
	);
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
	}

	Mesh->SetWorldScale3D(FVector(3.0f, 1.5f, 0.2f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);

	// Overlap trigger box - positioned above the mesh surface
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(Mesh);
	OverlapBox->SetBoxExtent(FVector(150.0f, 75.0f, 30.0f));
	OverlapBox->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBox->SetGenerateOverlapEvents(true);
}

void AOAConveyorBelt::BeginPlay()
{
	Super::BeginPlay();

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AOAConveyorBelt::OnOverlapBegin);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &AOAConveyorBelt::OnOverlapEnd);

	// Create Dynamic Material Instance and set scroll speed once.
	// The material's internal Time node handles per-frame animation automatically.
	UMaterialInterface* BaseMaterial = Mesh->GetMaterial(0);
	if (BaseMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		Mesh->SetMaterial(0, DynamicMaterial);

		const float DirectionSign = bReverseDirection ? -1.0f : 1.0f;
		DynamicMaterial->SetScalarParameterValue(TEXT("ScrollSpeed"), UVScrollSpeed * DirectionSign);
	}
}

void AOAConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Push overlapping characters along belt direction
	if (OverlappingCharacters.Num() > 0)
	{
		const FVector BeltDirection = bReverseDirection ? -GetActorForwardVector() : GetActorForwardVector();
		const FVector Displacement = BeltDirection * BeltSpeed * DeltaTime;

		for (int32 i = OverlappingCharacters.Num() - 1; i >= 0; --i)
		{
			if (ACharacter* Character = OverlappingCharacters[i].Get())
			{
				Character->AddActorWorldOffset(Displacement);
			}
			else
			{
				OverlappingCharacters.RemoveAtSwap(i);
			}
		}
	}
}

void AOAConveyorBelt::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	OverlappingCharacters.AddUnique(Character);
}

void AOAConveyorBelt::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	OverlappingCharacters.RemoveSwap(Character);
}
