// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OARotatingPillar.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USceneComponent;

/**
 * Rotating pillar obstacle.
 * A ground pillar with a horizontal arm on top that rotates around Z-axis.
 * Forms an "ㄱ" shape. Pushes the player on overlap.
 */
UCLASS()
class AOARotatingPillar : public AActor
{
	GENERATED_BODY()

public:

	AOARotatingPillar();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:

	/** Root scene component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	/** Center axis pillar mesh (vertical) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PillarMesh;

	/** Rotation pivot at the top of the pillar */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RotatingRoot;

	/** Horizontal arm mesh (rotated 90 deg, forms ㄱ shape) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ArmMesh;

	/** Collision box for hit detection on the arm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> HitCollision;

	/** Height of the center axis pillar. Controls whether the arm targets upper or lower body. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating Pillar", meta = (ClampMin = "10.0", UIMin = "10.0", Units = "cm"))
	float PillarHeight = 150.0f;

	/** Rotation speed of the arm in degrees per second. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating Pillar", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "deg/s"))
	float RotationSpeed = 90.0f;

	/** If true, the arm rotates clockwise (top-down view). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating Pillar")
	bool bClockwise = true;

	/** Force applied to the player when hit by the rotating arm. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating Pillar", meta = (ClampMin = "0.0"))
	float KnockbackForce = 1500.0f;

	/** Length of the horizontal arm. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating Pillar", meta = (ClampMin = "50.0", UIMin = "50.0", Units = "cm"))
	float ArmLength = 300.0f;

private:

	/** Update component transforms based on current property values. */
	void UpdatePillarLayout();

	UFUNCTION()
	void OnHitCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
