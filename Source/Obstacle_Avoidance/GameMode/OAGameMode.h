// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Obstacle_AvoidanceGameMode.h"
#include "OAGameMode.generated.h"

class AObstacle_AvoidanceCharacter;

/**
 *  Auto-running obstacle avoidance game mode.
 *  Continuously moves the player character forward (like holding W).
 */
UCLASS()
class AOAGameMode : public AObstacle_AvoidanceGameMode
{
	GENERATED_BODY()

public:

	AOAGameMode();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetAutoRunEnabled(bool bEnabled) { bAutoRunEnabled = bEnabled; }

protected:

	/** Auto-run speed multiplier (1.0 = full forward input) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AutoRun")
	float AutoRunInputScale = 1.f;

	/** Whether auto-run is currently active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AutoRun")
	bool bAutoRunEnabled = true;
};
