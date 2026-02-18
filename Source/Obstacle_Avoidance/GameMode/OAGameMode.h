// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Obstacle_AvoidanceGameMode.h"
#include "OAGameMode.generated.h"

/**
 *  Obstacle avoidance game mode.
 *  Player controls all movement manually.
 */
UCLASS()
class AOAGameMode : public AObstacle_AvoidanceGameMode
{
	GENERATED_BODY()

public:

	AOAGameMode();
};
