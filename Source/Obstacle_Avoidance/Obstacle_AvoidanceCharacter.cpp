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
#include "InputAction.h"
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
	// Blueprint에서 미할당된 에셋을 런타임 로드 (Live Coding 후 Blueprint 미갱신 대응)
	if (!DashAction)
	{
		DashAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Dash.IA_Dash"));
	}
	if (!SlideAction)
	{
		SlideAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Slide.IA_Slide"));
	}
	if (!DashMontage)
	{
		DashMontage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Jump/AM_Dash.AM_Dash"));
	}
	if (!SlideMontage)
	{
		SlideMontage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/FreeAnimationLibrary/Animations/Slide/AM_Slide.AM_Slide"));
	}

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

// ── Death ──

void AObstacle_AvoidanceCharacter::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// Stop any ongoing montages
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.2f);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		DisableInput(PC);
	}
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
	DefaultBrakingDeceleration = GetCharacterMovement()->BrakingDecelerationWalking;
	DefaultCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	DefaultMeshRelativeLocation = GetMesh()->GetRelativeLocation();

	// 캡슐 높이를 줄이고, 줄어든 만큼 메시를 위로 올려서 바닥에 빠지지 않게 함
	const float HeightDiff = DefaultCapsuleHalfHeight - SlideCapsuleHalfHeight;
	GetCapsuleComponent()->SetCapsuleHalfHeight(SlideCapsuleHalfHeight);
	GetMesh()->SetRelativeLocation(DefaultMeshRelativeLocation + FVector(0.f, 0.f, HeightDiff));

	// 캡슐 복원 플래그 초기화
	bRestoringCapsule = false;

	// 마찰·감속 제거 + MaxWalkSpeed를 SlideSpeed로 올려야 Velocity가 클램프되지 않음
	GetCharacterMovement()->GroundFriction = 0.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = SlideSpeed;

	// LaunchCharacter로 전방 속도 부여 (Walking 모드의 내부 감속을 우회)
	const FVector SlideDirection = GetActorForwardVector();
	LaunchCharacter(SlideDirection * SlideSpeed, true, true);

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

	// 속도 복원
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

	// 마찰·감속을 점진적으로 복원 (절반 값으로 시작)
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction * 0.5f;
	GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDeceleration * 0.3f;

	// 캡슐 높이 + 메시 위치를 Tick에서 점진적으로 복원 시작
	TargetCapsuleHalfHeight = DefaultCapsuleHalfHeight;
	TargetMeshRelativeLocation = DefaultMeshRelativeLocation;
	bRestoringCapsule = true;

	// 일정 시간 후 마찰·감속을 완전 복원
	GetWorldTimerManager().SetTimer(SlideRecoveryTimer, this,
		&AObstacle_AvoidanceCharacter::FinishSlideRecovery, SlideRecoveryTime, false);

	// 쿨다운 타이머
	GetWorldTimerManager().SetTimer(SlideCooldownTimer, [this]()
	{
		bCanSlide = true;
	}, SlideCooldown, false);
}

void AObstacle_AvoidanceCharacter::FinishSlideRecovery()
{
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDeceleration;
}

void AObstacle_AvoidanceCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRestoringCapsule)
	{
		const float CurrentHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		const float NewHalfHeight = FMath::FInterpTo(CurrentHalfHeight, TargetCapsuleHalfHeight, DeltaTime, CapsuleInterpSpeed);

		// 메시 위치도 동일한 속도로 보간
		const FVector CurrentMeshLoc = GetMesh()->GetRelativeLocation();
		const FVector NewMeshLoc = FMath::VInterpTo(CurrentMeshLoc, TargetMeshRelativeLocation, DeltaTime, CapsuleInterpSpeed);

		if (FMath::IsNearlyEqual(NewHalfHeight, TargetCapsuleHalfHeight, 0.5f))
		{
			// 목표에 도달 — 정확한 값으로 설정하고 종료
			GetCapsuleComponent()->SetCapsuleHalfHeight(TargetCapsuleHalfHeight);
			GetMesh()->SetRelativeLocation(TargetMeshRelativeLocation);
			bRestoringCapsule = false;
		}
		else
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(NewHalfHeight);
			GetMesh()->SetRelativeLocation(NewMeshLoc);
		}
	}
}

