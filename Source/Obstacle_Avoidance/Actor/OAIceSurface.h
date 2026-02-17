// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OAIceSurface.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 * Ice / Slippery Surface obstacle.
 * Drastically reduces character ground friction when they walk on this surface,
 * making movement control difficult.
 * Uses an overlap trigger to modify CharacterMovementComponent parameters at runtime.
 */
UCLASS()
class AOAIceSurface : public AActor
{
	GENERATED_BODY()

public:

	AOAIceSurface();

protected:

	virtual void BeginPlay() override;

	/** Visual mesh for the ice surface */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Overlap trigger volume that detects when the character is on the surface */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> OverlapBox;

	/** Ground friction applied to the character while on the ice surface. Lower = more slippery. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ice Surface", meta = (ClampMin = "0.0", ClampMax = "8.0"))
	float Friction = 0.05f;

	/** Braking deceleration while walking on ice (cm/s^2). Lower = harder to stop. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ice Surface", meta = (ClampMin = "0.0"))
	float BrakingDeceleration = 100.0f;

private:

	/** Cached original movement values per character (to restore on exit) */
	struct FOAOriginalMovementParams
	{
		float GroundFriction;
		float BrakingDecelerationWalking;
	};
	TMap<TWeakObjectPtr<ACharacter>, FOAOriginalMovementParams> OriginalParamsMap;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
