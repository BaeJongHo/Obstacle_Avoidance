// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OAJumpPad.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 * Jump Pad obstacle.
 * Launches characters upward when they step onto the pad surface.
 * Uses an overlap trigger to call LaunchCharacter with a configurable force.
 */
UCLASS()
class AOAJumpPad : public AActor
{
	GENERATED_BODY()

public:

	AOAJumpPad();

protected:

	virtual void BeginPlay() override;

	/** Visual mesh for the jump pad */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Overlap trigger volume that detects when the character steps on the pad */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> OverlapBox;

	/** Upward launch force applied to the character (cm/s). Higher = launches higher. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Pad", meta = (ClampMin = "0.0"))
	float LaunchForce = 1500.0f;

	/** If true, overrides the character's current Z velocity instead of adding to it */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Pad")
	bool bOverrideZVelocity = true;

private:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
