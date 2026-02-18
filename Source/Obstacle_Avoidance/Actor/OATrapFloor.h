// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OATrapFloor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 * Trap floor obstacle.
 * A walkable platform that collapses after the player steps on it.
 * Falls away after a configurable delay.
 */
UCLASS()
class AOATrapFloor : public AActor
{
	GENERATED_BODY()

public:

	AOATrapFloor();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

protected:

	/** Root scene component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	/** Walkable platform mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PlatformMesh;

	/** Overlap trigger for detecting the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> OverlapBox;

	/** Time in seconds before the platform falls after being stepped on. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap Floor", meta = (ClampMin = "0.0", Units = "s"))
	float FallDelay = 1.5f;

private:

	bool bTriggered = false;
	bool bIsFalling = false;
	float FallSpeed = 0.0f;
	float FallDistance = 0.0f;

	FTimerHandle FallTimerHandle;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void StartFalling();
};
