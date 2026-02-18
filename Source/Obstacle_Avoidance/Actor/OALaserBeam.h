// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OALaserBeam.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USceneComponent;

/**
 * Laser beam obstacle.
 * Two pillars with a beam that toggles on/off periodically.
 * Kills the player on contact.
 */
UCLASS()
class AOALaserBeam : public AActor
{
	GENERATED_BODY()

public:

	AOALaserBeam();

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:

	/** Root scene component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	/** Left pillar mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LeftPillar;

	/** Right pillar mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RightPillar;

	/** Beam visual mesh (toggled on/off) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BeamMesh;

	/** Beam collision (toggled on/off) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BeamCollision;

	/** Distance between the two pillars. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Laser Beam", meta = (ClampMin = "100.0", UIMin = "100.0", Units = "cm"))
	float PillarDistance = 500.0f;

	/** Total on/off cycle time. Beam is ON for the first half, OFF for the second half. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Laser Beam", meta = (ClampMin = "0.2", UIMin = "0.2", Units = "s"))
	float BeamCycle = 3.0f;

private:

	bool bBeamActive = true;

	FTimerHandle BeamTimerHandle;

	void ToggleBeam();

	/** Update component transforms based on current property values. */
	void UpdateLayout();

	UFUNCTION()
	void OnBeamOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
