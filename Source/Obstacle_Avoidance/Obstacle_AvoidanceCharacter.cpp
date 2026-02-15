// Copyright Epic Games, Inc. All Rights Reserved.

#include "Obstacle_AvoidanceCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Animation/AnimMontage.h"
#include "Obstacle_Avoidance.h"

AObstacle_AvoidanceCharacter::AObstacle_AvoidanceCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AObstacle_AvoidanceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AObstacle_AvoidanceCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AObstacle_AvoidanceCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AObstacle_AvoidanceCharacter::Look);

		// Dash
		UE_LOG(LogObstacle_Avoidance, Warning, TEXT("SetupInput: DashAction=%s, SlideAction=%s, DashMontage=%s, SlideMontage=%s"),
			DashAction ? *GetNameSafe(DashAction) : TEXT("NULL"),
			SlideAction ? *GetNameSafe(SlideAction) : TEXT("NULL"),
			DashMontage ? *GetNameSafe(DashMontage) : TEXT("NULL"),
			SlideMontage ? *GetNameSafe(SlideMontage) : TEXT("NULL"));

		if (DashAction)
		{
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AObstacle_AvoidanceCharacter::StartDash);
		}
		else
		{
			UE_LOG(LogObstacle_Avoidance, Error, TEXT("DashAction is NULL! Assign it in the Blueprint."));
		}

		// Slide
		if (SlideAction)
		{
			EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &AObstacle_AvoidanceCharacter::StartSlide);
		}
		else
		{
			UE_LOG(LogObstacle_Avoidance, Error, TEXT("SlideAction is NULL! Assign it in the Blueprint."));
		}
	}
	else
	{
		UE_LOG(LogObstacle_Avoidance, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AObstacle_AvoidanceCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AObstacle_AvoidanceCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AObstacle_AvoidanceCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AObstacle_AvoidanceCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AObstacle_AvoidanceCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AObstacle_AvoidanceCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

// ── Dash ──

void AObstacle_AvoidanceCharacter::StartDash()
{
	UE_LOG(LogObstacle_Avoidance, Warning, TEXT("StartDash called! bCanDash=%d, bIsDashing=%d, bIsSliding=%d, OnGround=%d, DashMontage=%s"),
		bCanDash, bIsDashing, bIsSliding,
		GetCharacterMovement()->IsMovingOnGround(),
		DashMontage ? *GetNameSafe(DashMontage) : TEXT("NULL"));

	if (!bCanDash || bIsDashing || bIsSliding)
	{
		return;
	}

	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}

	if (!DashMontage)
	{
		UE_LOG(LogObstacle_Avoidance, Warning, TEXT("DashMontage is not set on %s"), *GetNameSafe(this));
		return;
	}

	bIsDashing = true;
	bCanDash = false;

	// 캐릭터 전방 방향으로 LaunchCharacter
	const FVector DashDirection = GetActorForwardVector();
	LaunchCharacter(DashDirection * DashSpeed, true, true);

	// 몽타주 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(DashMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AObstacle_AvoidanceCharacter::OnDashMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, DashMontage);
	}
}

void AObstacle_AvoidanceCharacter::OnDashMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsDashing = false;

	// 쿨다운 타이머
	GetWorldTimerManager().SetTimer(DashCooldownTimer, [this]()
	{
		bCanDash = true;
	}, DashCooldown, false);
}

// ── Slide ──

void AObstacle_AvoidanceCharacter::StartSlide()
{
	UE_LOG(LogObstacle_Avoidance, Warning, TEXT("StartSlide called! bCanSlide=%d, bIsSliding=%d, bIsDashing=%d, OnGround=%d, SlideMontage=%s"),
		bCanSlide, bIsSliding, bIsDashing,
		GetCharacterMovement()->IsMovingOnGround(),
		SlideMontage ? *GetNameSafe(SlideMontage) : TEXT("NULL"));

	if (!bCanSlide || bIsSliding || bIsDashing)
	{
		return;
	}

	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}

	if (!SlideMontage)
	{
		UE_LOG(LogObstacle_Avoidance, Warning, TEXT("SlideMontage is not set on %s"), *GetNameSafe(this));
		return;
	}

	bIsSliding = true;
	bCanSlide = false;

	// 기본값 저장
	DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction;

	// Crouch 시스템으로 캡슐 축소 (위치 보정 자동 처리)
	GetCharacterMovement()->CrouchedHalfHeight = SlideCapsuleHalfHeight;
	Crouch();

	// 마찰 감소 + 속도 증가
	GetCharacterMovement()->GroundFriction = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = SlideSpeed;

	// 전방으로 속도 부여
	const FVector SlideDirection = GetActorForwardVector();
	GetCharacterMovement()->Velocity = SlideDirection * SlideSpeed;

	// 몽타주 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(SlideMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AObstacle_AvoidanceCharacter::OnSlideMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, SlideMontage);
	}
}

void AObstacle_AvoidanceCharacter::OnSlideMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsSliding = false;

	// UnCrouch로 캡슐 복원 (위치 보정 자동 처리, 덜컹거림 없음)
	UnCrouch();
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;

	// 쿨다운 타이머
	GetWorldTimerManager().SetTimer(SlideCooldownTimer, [this]()
	{
		bCanSlide = true;
	}, SlideCooldown, false);
}
