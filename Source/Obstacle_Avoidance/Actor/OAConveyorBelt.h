// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OAConveyorBelt.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UMaterialInstanceDynamic;

/**
 * Conveyor Belt obstacle.
 * Pushes characters along the belt direction while they stand on it.
 * Uses Dynamic Material Instance to scroll UVs, creating a visual flow effect
 * without physically moving the mesh.
 */
UCLASS()
class AOAConveyorBelt : public AActor
{
	GENERATED_BODY()

public:

	AOAConveyorBelt();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Visual mesh for the conveyor belt */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Overlap trigger volume that detects when the character is on the belt */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> OverlapBox;

	/** Belt movement speed applied to characters (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conveyor Belt", meta = (ClampMin = "0.0"))
	float BeltSpeed = 300.0f;

	/** If true, belt moves in reverse direction (local -X instead of +X) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conveyor Belt")
	bool bReverseDirection = false;

	/** UV scroll speed for the visual flow effect. Adjusts how fast the texture appears to move. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conveyor Belt", meta = (ClampMin = "0.0"))
	float UVScrollSpeed = 0.5f;

private:

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	TArray<TWeakObjectPtr<ACharacter>> OverlappingCharacters;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
