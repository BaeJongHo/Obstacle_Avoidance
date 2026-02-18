// Copyright Epic Games, Inc. All Rights Reserved.

#include "OAGoalVolume.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AOAGoalVolume::AOAGoalVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// Overlap box as root — covers the full path width
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
	if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
	{
		Character->DisableInput(PC);
	}

	// Schedule level transition
	GetWorldTimerManager().SetTimer(
		TransitionTimerHandle, this, &AOAGoalVolume::TransitionToNextLevel,
		TransitionDelay, false);
}

void AOAGoalVolume::TransitionToNextLevel()
{
	if (!NextLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, NextLevelName);
	}
}
