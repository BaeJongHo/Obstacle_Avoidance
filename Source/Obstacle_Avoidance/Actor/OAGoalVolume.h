// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OAGoalVolume.generated.h"

class UBoxComponent;
class USpringArmComponent;

/**
 * Goal volume placed at the end of a stage.
 * When the player overlaps, movement stops immediately and a camera orbit
 * sequence plays around the player. The game transitions to the next level
 * once the orbit completes.
 */
UCLASS()
class AOAGoalVolume : public AActor
{
	GENERATED_BODY()

public:

	AOAGoalVolume();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Overlap trigger covering the path width */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> OverlapBox;

	/** Name of the next level to open (e.g. "Lvl_ObstacleLevel2") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal")
	FName NextLevelName;

	// ── Camera Orbit Settings ──

	/** Duration of the 360-degree camera orbit (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal|Camera Orbit", meta = (ClampMin = "0.5", Units = "s"))
	float OrbitDuration = 3.f;

	/** Orbit radius from the player character (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal|Camera Orbit", meta = (ClampMin = "100.0", Units = "cm"))
	float OrbitRadius = 500.f;

	/** Camera pitch during orbit — negative looks down at the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal|Camera Orbit", meta = (ClampMin = "-89.0", ClampMax = "89.0", Units = "deg"))
	float OrbitPitch = -20.f;

private:

	bool bTriggered = false;

	// ── Orbit Runtime State ──

	float OrbitElapsedTime = 0.f;
	float StartYawAngle = 0.f;

	TWeakObjectPtr<ACharacter> OrbitTargetCharacter;
	TWeakObjectPtr<APlayerController> CachedPlayerController;
	TWeakObjectPtr<USpringArmComponent> CachedCameraBoom;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void BeginCameraOrbit(ACharacter* InCharacter, APlayerController* InPC);
	void UpdateCameraOrbit(float DeltaTime);
	void FinishCameraOrbit();
	void TransitionToNextLevel();
};
