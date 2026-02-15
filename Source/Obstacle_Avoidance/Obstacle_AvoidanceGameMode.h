// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Obstacle_AvoidanceGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AObstacle_AvoidanceGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AObstacle_AvoidanceGameMode();
};



