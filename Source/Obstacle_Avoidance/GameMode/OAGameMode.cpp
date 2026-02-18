// Fill out your copyright notice in the Description page of Project Settings.

#include "OAGameMode.h"
#include "Obstacle_AvoidanceCharacter.h"
#include "GameFramework/PlayerController.h"

AOAGameMode::AOAGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOAGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bAutoRunEnabled)
	{
		return;
	}

	// Apply forward movement to all player characters
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			continue;
		}

		AObstacle_AvoidanceCharacter* Character = Cast<AObstacle_AvoidanceCharacter>(PC->GetPawn());
		if (!Character || Character->IsDead())
		{
			continue;
		}

		// DoMove(Right, Forward) - Forward=1.0 is like holding W
		Character->DoMove(0.f, AutoRunInputScale);
	}
}
