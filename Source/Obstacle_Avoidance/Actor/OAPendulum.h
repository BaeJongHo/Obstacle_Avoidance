// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OAPendulum.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class USceneComponent;

/**
 * Pendulum swing obstacle.
 * Swings back and forth using math-driven motion (sine wave).
 * Pushes the player on overlap.
 */
UCLASS()
class AOAPendulum : public AActor
{
	GENERATED_BODY()

public:

	AOAPendulum();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

protected:

	/** Root pivot point (the top anchor of the pendulum) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> PivotPoint;

	/** Arm that swings - rotates around the pivot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SwingArm;

	/** Pendulum mesh (attached to the swing arm) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PendulumMesh;

	/** Collision sphere for hit detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> HitCollision;

	/** Maximum swing angle in degrees (from vertical). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum", meta = (ClampMin = "1.0", ClampMax = "90.0", Units = "deg"))
	float SwingAngle = 45.0f;

	/** Swing speed multiplier. Higher = faster oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum", meta = (ClampMin = "0.1", Units = "x"))
	float SwingSpeed = 1.5f;

	/** Force applied to the player when hit by the pendulum. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum", meta = (ClampMin = "0.0"))
	float PushForce = 1500.0f;

	/** Length of the pendulum arm (distance from pivot to mesh center). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum", meta = (ClampMin = "50.0", Units = "cm"))
	float ArmLength = 300.0f;

	/** Default static mesh for the pendulum head */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum")
	TObjectPtr<UStaticMesh> DefaultMesh;

private:

	/** Accumulated time for sine wave calculation */
	float ElapsedTime = 0.0f;

	UFUNCTION()
	void OnHitCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
