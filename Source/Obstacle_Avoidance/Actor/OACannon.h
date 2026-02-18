// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OACannon.generated.h"

class UStaticMeshComponent;
class USceneComponent;

/**
 * Cannon obstacle.
 * A base pedestal with an angled barrel that periodically fires cannonballs.
 * Cannonballs follow parabolic trajectory and knock back the player on hit.
 */
UCLASS()
class AOACannon : public AActor
{
	GENERATED_BODY()

public:

	AOACannon();

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:

	/** Root scene component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	/** Base pedestal mesh (vertical cylinder) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	/** Pivot for barrel rotation (sits on top of base) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> BarrelPivot;

	/** Barrel mesh (cylinder angled by BarrelAngle) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BarrelMesh;

	/** Spawn point at the muzzle end of the barrel */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> MuzzlePoint;

	/** Barrel elevation angle in degrees. 0 = horizontal, 90 = straight up. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (ClampMin = "-90.0", ClampMax = "90.0", Units = "deg"))
	float BarrelAngle = 45.0f;

	/** Initial speed of the cannonball. Combined with BarrelAngle for parabolic trajectory. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (ClampMin = "0.0", Units = "cm/s"))
	float LaunchSpeed = 2000.0f;

	/** Time interval between cannon shots in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (ClampMin = "0.1", Units = "s"))
	float FireInterval = 2.0f;

	/** Force applied to the player when hit by a cannonball. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (ClampMin = "0.0"))
	float KnockbackForce = 1500.0f;

	/** Length of the cannon barrel. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (ClampMin = "50.0", UIMin = "50.0", Units = "cm"))
	float BarrelLength = 200.0f;

private:

	FTimerHandle FireTimerHandle;

	void FireCannonball();

	/** Update component transforms based on current property values. */
	void UpdateCannonLayout();
};
