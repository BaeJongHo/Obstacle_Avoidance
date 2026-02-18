// Copyright Epic Games, Inc. All Rights Reserved.

#include "OACannonball.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Engine/StaticMesh.h"

AOACannonball::AOACannonball()
{
	PrimaryActorTick.bCanEverTick = false;
	InitialLifeSpan = 10.0f;

	// Collision sphere (root) - blocks world geometry, overlaps pawns
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(20.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CollisionSphere->SetGenerateOverlapEvents(true);
	SetRootComponent(CollisionSphere);

	// Visual mesh (no collision)
	CannonballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonballMesh"));
	CannonballMesh->SetupAttachment(CollisionSphere);
	CannonballMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CannonballMesh->SetRelativeScale3D(FVector(0.4f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")
	);
	if (SphereAsset.Succeeded())
	{
		CannonballMesh->SetStaticMesh(SphereAsset.Object);
	}

	// Projectile movement - parabolic trajectory with gravity
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(CollisionSphere);
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
}

void AOACannonball::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentHit.AddDynamic(this, &AOACannonball::OnHit);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AOACannonball::OnOverlapBegin);
}

void AOACannonball::Initialize(const FVector& InDirection, float InSpeed, float InKnockbackForce)
{
	KnockbackForce = InKnockbackForce;
	ProjectileMovement->InitialSpeed = InSpeed;
	ProjectileMovement->MaxSpeed = InSpeed;
	ProjectileMovement->Velocity = InDirection * InSpeed;
}

void AOACannonball::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	Destroy();
}

void AOACannonball::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (HitCharacter)
	{
		// Knockback direction: from cannonball toward player
		FVector KnockbackDir = (HitCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
		KnockbackDir.Z = 0.3f;
		KnockbackDir.Normalize();

		HitCharacter->LaunchCharacter(KnockbackDir * KnockbackForce, true, true);
	}

	Destroy();
}
