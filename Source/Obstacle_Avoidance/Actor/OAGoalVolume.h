// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OAGoalVolume.generated.h"

class UBoxComponent;

/**
 * Goal volume placed at the end of a stage.
 * When the player overlaps, movement stops immediately and the game
 * transitions to the next level after a configurable delay.
 */
UCLASS()
class AOAGoalVolume : public AActor
{
	GENERATED_BODY()

public:

	AOAGoalVolume();

protected:

	virtual void BeginPlay() override;

	/** Overlap trigger covering the path width */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> OverlapBox;

	/** Name of the next level to open (e.g. "Lvl_ObstacleLevel2") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal")
	FName NextLevelName;

	/** Seconds to wait before opening the next level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal", meta = (ClampMin = "0.0", Units = "s"))
	float TransitionDelay = 3.f;

private:

	bool bTriggered = false;
	FTimerHandle TransitionTimerHandle;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void TransitionToNextLevel();
};
