// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Obstacle_AvoidanceCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UAnimMontage;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AObstacle_AvoidanceCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> DashAction;

	/** Slide Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SlideAction;

	// ── Dash ──

	/** Dash AnimMontage (BP에서 설정) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dash")
	TObjectPtr<UAnimMontage> DashMontage;

	/** Dash 이동 속도 (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dash")
	float DashSpeed = 1500.f;

	/** Dash 쿨다운 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dash")
	float DashCooldown = 1.f;

	// ── Slide ──

	/** Slide AnimMontage (BP에서 설정) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	TObjectPtr<UAnimMontage> SlideMontage;

	/** Slide 이동 속도 (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideSpeed = 800.f;

	/** Slide 쿨다운 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideCooldown = 1.5f;

	/** Slide 시 캡슐 반높이 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideCapsuleHalfHeight = 48.f;

public:

	/** Constructor */
	AObstacle_AvoidanceCharacter();

	FORCEINLINE bool GetIsDashing() const { return bIsDashing; }
	FORCEINLINE bool GetIsSliding() const { return bIsSliding; }

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// ── Dash ──

	void StartDash();
	void OnDashMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// ── Slide ──

	void StartSlide();
	void OnSlideMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:

	bool bIsDashing = false;
	bool bIsSliding = false;
	bool bCanDash = true;
	bool bCanSlide = true;

	float DefaultMaxWalkSpeed = 0.f;
	float DefaultGroundFriction = 0.f;

	FTimerHandle DashCooldownTimer;
	FTimerHandle SlideCooldownTimer;
};

