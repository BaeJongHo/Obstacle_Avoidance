// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OAMovingPlatform.generated.h"

class UStaticMeshComponent;

/**
 * Moving platform obstacle that oscillates between two points.
 * Moves along either the X axis (forward/backward) or Y axis (left/right)
 * based on a configurable distance from the spawn location.
 */
UCLASS()
class AOAMovingPlatform : public AActor
{
	GENERATED_BODY()

public:

	AOAMovingPlatform();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:

	/** Platform mesh component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** If true, moves along Y axis (left/right). If false, moves along X axis (forward/backward). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Moving Platform")
	bool bMoveLeftRight = true;

	/** Total distance the platform travels from the start position (in one direction). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Moving Platform", meta = (ClampMin = "0.0", Units = "cm"))
	float MoveDistance = 300.0f;

	/** Movement speed in cm/s. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Moving Platform", meta = (ClampMin = "0.0", Units = "cm/s"))
	float MoveSpeed = 200.0f;

private:

	/** Cached start location */
	FVector StartLocation;

	/** Current movement direction multiplier (1 or -1) */
	float Direction = 1.0f;

	/** Current progress along the movement path (0 to MoveDistance) */
	float CurrentDistance = 0.0f;
};
