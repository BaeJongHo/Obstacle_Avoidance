---
name: ue-combat-system
description: Unreal Engine 5.7 전투 시스템 구현. 콤보 공격, 차지 공격, 데미지 처리, 래그돌, 리스폰 시스템 작성 시 사용
---

# UE 5.7 전투 시스템 스킬

## 목적
이 프로젝트의 전투 시스템 패턴에 맞춰 캐릭터, 적, 게임플레이 오브젝트를 생성한다.

## 전투 캐릭터 구조 (프로젝트 표준)

### 핵심 시스템

1. **콤보 공격 시스템** - AnimMontage 섹션 기반
```cpp
// 콤보 몽타주 및 섹션
UPROPERTY(EditAnywhere, Category = "Melee Attack|Combo")
UAnimMontage* ComboAttackMontage;

UPROPERTY(EditAnywhere, Category = "Melee Attack|Combo")
TArray<FName> ComboSectionNames;

UPROPERTY(EditAnywhere, Category = "Melee Attack|Combo",
    meta = (ClampMin = 0, ClampMax = 5, Units = "s"))
float ComboInputCacheTimeTolerance = 0.45f;

int32 ComboCount = 0;
```

2. **차지 공격 시스템** - Press/Hold/Release 패턴
```cpp
UPROPERTY(EditAnywhere, Category = "Melee Attack|Charged")
UAnimMontage* ChargedAttackMontage;

UPROPERTY(EditAnywhere, Category = "Melee Attack|Charged")
FName ChargeLoopSection;

UPROPERTY(EditAnywhere, Category = "Melee Attack|Charged")
FName ChargeAttackSection;

bool bIsChargingAttack = false;
bool bHasLoopedChargedAttack = false;
```

3. **데미지 시스템** - HP + 넉백 + 래그돌
```cpp
UPROPERTY(EditAnywhere, Category = "Damage", meta = (ClampMin = 0, ClampMax = 100))
float MaxHP = 5.0f;

UPROPERTY(VisibleAnywhere, Category = "Damage")
float CurrentHP = 0.0f;

// Sphere trace 기반 공격 감지
UPROPERTY(EditAnywhere, Category = "Melee Attack|Trace",
    meta = (ClampMin = 0, ClampMax = 500, Units = "cm"))
float MeleeTraceDistance = 75.0f;

UPROPERTY(EditAnywhere, Category = "Melee Attack|Trace",
    meta = (ClampMin = 0, ClampMax = 200, Units = "cm"))
float MeleeTraceRadius = 75.0f;

// 넉백
UPROPERTY(EditAnywhere, Category = "Melee Attack|Damage",
    meta = (ClampMin = 0, ClampMax = 1000, Units = "cm/s"))
float MeleeKnockbackImpulse = 250.0f;
```

4. **리스폰 시스템** - Timer 기반
```cpp
UPROPERTY(EditAnywhere, Category = "Respawn",
    meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
float RespawnTime = 3.0f;

FTimerHandle RespawnTimer;
```

## 입력 바인딩 패턴 (Enhanced Input)

```cpp
// 헤더에서 선언
UPROPERTY(EditAnywhere, Category = "Input")
UInputAction* ComboAttackAction;

UPROPERTY(EditAnywhere, Category = "Input")
UInputAction* ChargedAttackAction;

// SetupPlayerInputComponent에서 바인딩
void ACombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput =
        Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // 콤보 공격: Pressed
        EnhancedInput->BindAction(ComboAttackAction, ETriggerEvent::Started,
            this, &ACombatCharacter::ComboAttackPressed);

        // 차지 공격: Pressed + Released
        EnhancedInput->BindAction(ChargedAttackAction, ETriggerEvent::Started,
            this, &ACombatCharacter::ChargedAttackPressed);
        EnhancedInput->BindAction(ChargedAttackAction, ETriggerEvent::Completed,
            this, &ACombatCharacter::ChargedAttackReleased);
    }
}
```

## Blueprint/C++ 하이브리드 패턴

이 프로젝트는 C++로 로직을, Blueprint로 시각 효과를 처리:
```cpp
// C++에서 호출, Blueprint에서 구현
UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
void DealtDamage(float Damage, const FVector& ImpactPoint);

UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
void ReceivedDamage(float Damage, const FVector& ImpactPoint,
    const FVector& DamageDirection);

UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
void BP_ToggleCamera();
```

## 게임플레이 오브젝트 템플릿

### 데미지 볼륨/트리거
```cpp
UCLASS()
class A{Variant}LavaFloor : public AActor
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, Category = "Damage")
    float DamagePerSecond = 1.0f;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerVolume;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, ...);
};
```

### 체크포인트 볼륨
```cpp
UCLASS()
class A{Variant}CheckpointVolume : public AActor
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerVolume;

    UPROPERTY(EditAnywhere, Category = "Checkpoint")
    FVector RespawnLocation;

    UFUNCTION()
    void OnOverlapBegin(...);
};
```

### 파괴 가능 오브젝트 (인터페이스 구현)
```cpp
UCLASS()
class A{Variant}DamageableBox : public AActor, public ICombatDamageable
{
    GENERATED_BODY()

public:
    // ICombatDamageable 구현
    virtual void ApplyDamage(float Damage, AActor* DamageCauser,
        const FVector& DamageLocation, const FVector& DamageImpulse) override;
    virtual void HandleDeath() override;
    virtual void ApplyHealing(float Healing, AActor* Healer) override;
    virtual void NotifyDanger(const FVector& DangerLocation, AActor* DangerSource) override;
};
```

## 위험 감지 시스템 (EQS 연동)

```cpp
// 공격 전 주변 적에게 위험 알림
void ACombatCharacter::NotifyEnemiesOfIncomingAttack()
{
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(DangerTraceRadius);
    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * DangerTraceDistance;

    TArray<FHitResult> HitResults;
    GetWorld()->SweepMultiByChannel(HitResults, Start, End,
        FQuat::Identity, ECC_Pawn, SphereShape);

    for (const FHitResult& Hit : HitResults)
    {
        if (ICombatDamageable* Damageable = Cast<ICombatDamageable>(Hit.GetActor()))
        {
            Damageable->NotifyDanger(GetActorLocation(), this);
        }
    }
}
```

## AnimMontage 콜백 패턴

```cpp
// 델리게이트 선언 (헤더)
FOnMontageEnded OnAttackMontageEnded;

// 바인딩 (공격 시작 시)
void ACombatCharacter::ComboAttack()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && ComboAttackMontage)
    {
        AnimInstance->Montage_Play(ComboAttackMontage);
        OnAttackMontageEnded.BindUObject(this, &ACombatCharacter::AttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ComboAttackMontage);
    }
}

// 콜백 처리
void ACombatCharacter::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    bIsAttacking = false;
    ComboCount = 0;
}
```

## 체크리스트
1. ICombatDamageable/ICombatAttacker 인터페이스를 올바르게 구현했는가?
2. AnimMontage 델리게이트를 바인딩하고 해제했는가?
3. Enhanced Input으로 Started/Completed 이벤트를 분리했는가?
4. HP/데미지 값에 ClampMin/ClampMax를 설정했는가?
5. FTimerHandle을 사용하여 리스폰을 처리했는가?
6. BlueprintImplementableEvent로 시각 효과를 분리했는가?
7. Sphere Trace 파라미터(거리, 반경)를 UPROPERTY로 노출했는가?
