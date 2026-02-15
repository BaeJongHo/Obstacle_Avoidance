---
name: ue-interface
description: Unreal Engine 5.7 인터페이스 생성. UINTERFACE 매크로 기반 C++ 인터페이스 작성 시 사용
---

# UE 5.7 인터페이스 생성 스킬

## 목적
프로젝트 표준에 맞는 UE 인터페이스(UINTERFACE) 생성을 표준화한다.

## 기본 인터페이스 템플릿 (프로젝트 표준)

### 헤더 (.h)
```cpp
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "{InterfaceName}.generated.h"

/**
 *  {InterfaceName} interface
 *  {인터페이스 설명}
 */
UINTERFACE(MinimalAPI, NotBlueprintable)
class U{InterfaceName} : public UInterface
{
    GENERATED_BODY()
};

class I{InterfaceName}
{
    GENERATED_BODY()

public:
    /** {함수 설명} */
    UFUNCTION(BlueprintCallable, Category = "{카테고리}")
    virtual void {FunctionName}({Parameters}) = 0;
};
```

### 소스 (.cpp)
```cpp
#include "{InterfaceName}.h"
// UE 인터페이스는 보통 .cpp가 비어있음 (순수 가상 함수만 선언)
```

## 프로젝트 내 인터페이스 예시

### ICombatDamageable - 데미지/힐/사망 처리
```cpp
UINTERFACE(MinimalAPI, NotBlueprintable)
class UCombatDamageable : public UInterface
{
    GENERATED_BODY()
};

class ICombatDamageable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Damageable")
    virtual void ApplyDamage(float Damage, AActor* DamageCauser,
        const FVector& DamageLocation, const FVector& DamageImpulse) = 0;

    UFUNCTION(BlueprintCallable, Category = "Damageable")
    virtual void HandleDeath() = 0;

    UFUNCTION(BlueprintCallable, Category = "Damageable")
    virtual void ApplyHealing(float Healing, AActor* Healer) = 0;

    UFUNCTION(BlueprintCallable, Category = "Damageable")
    virtual void NotifyDanger(const FVector& DangerLocation, AActor* DangerSource) = 0;
};
```

### ICombatAttacker - 공격 실행
```cpp
class ICombatAttacker
{
    GENERATED_BODY()

public:
    virtual void DoAttackTrace(FName DamageSourceBone) = 0;
    virtual void CheckCombo() = 0;
    virtual void CheckChargedAttack() = 0;
};
```

### ICombatActivatable - 활성화 가능 오브젝트
```cpp
class ICombatActivatable
{
    GENERATED_BODY()

public:
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
};
```

### ISideScrollingInteractable - 상호작용 가능 오브젝트
```cpp
class ISideScrollingInteractable
{
    GENERATED_BODY()

public:
    virtual void Interact(AActor* Interactor) = 0;
};
```

## 인터페이스 구현 패턴 (다중 상속)

```cpp
// 여러 인터페이스를 동시에 구현
UCLASS(abstract)
class ACombatCharacter : public ACharacter,
    public ICombatAttacker,
    public ICombatDamageable
{
    GENERATED_BODY()

public:
    // ICombatAttacker 구현
    virtual void DoAttackTrace(FName DamageSourceBone) override;
    virtual void CheckCombo() override;
    virtual void CheckChargedAttack() override;

    // ICombatDamageable 구현
    virtual void ApplyDamage(float Damage, AActor* DamageCauser,
        const FVector& DamageLocation, const FVector& DamageImpulse) override;
    virtual void HandleDeath() override;
    virtual void ApplyHealing(float Healing, AActor* Healer) override;
    virtual void NotifyDanger(const FVector& DangerLocation, AActor* DangerSource) override;
};
```

## 인터페이스 사용 (캐스팅)

```cpp
// 인터페이스 구현 여부 확인
if (AActor* HitActor = HitResult.GetActor())
{
    if (ICombatDamageable* Damageable = Cast<ICombatDamageable>(HitActor))
    {
        Damageable->ApplyDamage(MeleeDamage, this, HitLocation, KnockbackImpulse);
    }
}

// Implements 확인
if (TargetActor->Implements<UCombatDamageable>())
{
    ICombatDamageable::Execute_ApplyDamage(TargetActor, Damage, this, Location, Impulse);
}
```

## UINTERFACE 지정자 가이드
- `MinimalAPI` - 최소 API 노출 (DLL export 최소화)
- `NotBlueprintable` - Blueprint에서 이 인터페이스를 구현할 수 없음
- `Blueprintable` - Blueprint에서도 구현 가능하게 허용
- `BlueprintType` - Blueprint 변수 타입으로 사용 가능
- `meta=(CannotImplementInterfaceInBlueprint)` - C++에서만 구현 가능

## 체크리스트
1. UINTERFACE와 IInterface 쌍을 모두 선언했는가?
2. `GENERATED_BODY()` 매크로를 양쪽 모두에 포함했는가?
3. .generated.h를 마지막 include로 추가했는가?
4. 순수 가상 함수(`= 0`)로 선언했는가?
5. 구현 클래스에서 `override` 키워드를 사용했는가?
6. BlueprintCallable을 필요한 함수에 적용했는가?
7. .cpp 파일을 생성했는가? (비어있어도 필요)
