// Copyright Epic Games, Inc. All Rights Reserved.

#include "OAGoalVolume.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AOAGoalVolume::AOAGoalVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	SetRootComponent(OverlapBox);
	OverlapBox->SetBoxExtent(FVector(100.f, 250.f, 200.f));
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBox->SetGenerateOverlapEvents(true);
}

void AOAGoalVolume::BeginPlay()
{
	Super::BeginPlay();

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AOAGoalVolume::OnOverlapBegin);
}

void AOAGoalVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggered)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character || !Character->IsPlayerControlled())
	{
		return;
	}

	bTriggered = true;

	// Stop character movement immediately
	if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
	{
		CMC->StopMovementImmediately();
		CMC->DisableMovement();
	}

	// Disable player input
	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (PC)
	{
		Character->DisableInput(PC);
	}

	// Begin the camera orbit sequence instead of a timed delay
	BeginCameraOrbit(Character, PC);
}

void AOAGoalVolume::BeginCameraOrbit(ACharacter* InCharacter, APlayerController* InPC)
{
	OrbitTargetCharacter = InCharacter;
	CachedPlayerController = InPC;
	OrbitElapsedTime = 0.f;

	// Find and configure the spring arm for orbit control
	USpringArmComponent* Boom = InCharacter->FindComponentByClass<USpringArmComponent>();
	if (Boom)
	{
		CachedCameraBoom = Boom;

		// Capture current yaw so the orbit starts seamlessly
		StartYawAngle = Boom->GetComponentRotation().Yaw;

		// Take manual control of the boom rotation
		Boom->bUsePawnControlRotation = false;
		Boom->bEnableCameraLag = false;
		Boom->bEnableCameraRotationLag = false;
		Boom->TargetArmLength = OrbitRadius;
	}

	SetActorTickEnabled(true);
}

void AOAGoalVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraOrbit(DeltaTime);
}

void AOAGoalVolume::UpdateCameraOrbit(float DeltaTime)
{
	if (!OrbitTargetCharacter.IsValid() || !CachedCameraBoom.IsValid())
	{
		FinishCameraOrbit();
		return;
	}

	OrbitElapsedTime += DeltaTime;

	if (OrbitElapsedTime >= OrbitDuration)
	{
		FinishCameraOrbit();
		return;
	}

	// 0 → 1 progress with ease-in-out for a cinematic feel
	const float RawAlpha = OrbitElapsedTime / OrbitDuration;
	const float Alpha = FMath::InterpEaseInOut(0.f, 1.f, RawAlpha, 2.f);

	// Full 360-degree sweep from the captured starting angle
	const float CurrentYaw = StartYawAngle + Alpha * 360.f;

	CachedCameraBoom->SetWorldRotation(FRotator(OrbitPitch, CurrentYaw, 0.f));
}

void AOAGoalVolume::FinishCameraOrbit()
{
	SetActorTickEnabled(false);
	TransitionToNextLevel();
}

void AOAGoalVolume::TransitionToNextLevel()
{
	if (!NextLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, NextLevelName);
	}
}
