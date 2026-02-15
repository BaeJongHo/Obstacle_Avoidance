---
name: ue-statetree-ai
description: Unreal Engine 5.7 StateTree 기반 AI 시스템 생성. AIController, StateTree 컴포넌트, EQS, AI NPC 작성 시 사용
---

# UE 5.7 StateTree AI 시스템 스킬

## 목적
StateTree 기반 AI 캐릭터 및 컨트롤러 생성을 표준화한다. 이 프로젝트는 BehaviorTree 대신 StateTree를 사용한다.

## AIController 템플릿 (프로젝트 표준)

### 헤더 (.h)
```cpp
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "{Variant}AIController.generated.h"

class UStateTreeAIComponent;

/**
 *  A basic AI Controller capable of running StateTree
 */
UCLASS(abstract)
class A{Variant}AIController : public AAIController
{
    GENERATED_BODY()

    /** StateTree Component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UStateTreeAIComponent* StateTreeAI;

public:
    A{Variant}AIController();
};
```

### 소스 (.cpp)
```cpp
#include "{Variant}AIController.h"
#include "Components/StateTreeAIComponent.h"

A{Variant}AIController::A{Variant}AIController()
{
    StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAI"));
}
```

## AI Enemy/NPC 템플릿

### 헤더
```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// 필요한 인터페이스 include
#include "{Variant}Enemy.generated.h"

UCLASS(abstract)
class A{Variant}Enemy : public ACharacter //, public I{Interface}
{
    GENERATED_BODY()

public:
    A{Variant}Enemy();

protected:
    /** AI가 사용할 속성 */
    UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = 0, ClampMax = 100))
    float DetectionRange = 500.0f;

    UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = 0, ClampMax = 100))
    float AttackRange = 150.0f;

    virtual void BeginPlay() override;
};
```

## StateTree Utility 함수 템플릿

StateTree Task/Condition에서 사용할 유틸리티 함수를 BlueprintCallable로 정의:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "{Variant}StateTreeUtility.generated.h"

/**
 *  StateTree에서 사용할 유틸리티 함수 모음
 */
UCLASS()
class U{Variant}StateTreeUtility : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** 타겟까지의 거리 계산 */
    UFUNCTION(BlueprintPure, Category = "AI|Utility")
    static float GetDistanceToTarget(const AActor* Source, const AActor* Target);

    /** 시야 내에 있는지 확인 */
    UFUNCTION(BlueprintPure, Category = "AI|Utility")
    static bool IsInLineOfSight(const AActor* Source, const AActor* Target);
};
```

## EQS (Environment Query System) 컨텍스트 템플릿

```cpp
#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_{Name}.generated.h"

/**
 *  EQS 컨텍스트: {설명}
 */
UCLASS()
class UEnvQueryContext_{Name} : public UEnvQueryContext
{
    GENERATED_BODY()

public:
    virtual void ProvideContext(FEnvQueryInstance& QueryInstance,
        FEnvQueryContextData& ContextData) const override;
};
```

### 소스
```cpp
#include "EnvQueryContext_{Name}.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_{Name}::ProvideContext(
    FEnvQueryInstance& QueryInstance,
    FEnvQueryContextData& ContextData) const
{
    // 예: 플레이어 위치를 컨텍스트로 제공
    // UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerActor);
}
```

## Enemy Spawner 패턴

```cpp
UCLASS()
class A{Variant}EnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    A{Variant}EnemySpawner();

protected:
    /** 스폰할 적 클래스 */
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<A{Variant}Enemy> EnemyClass;

    /** 최대 동시 스폰 수 */
    UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 1, ClampMax = 20))
    int32 MaxEnemies = 3;

    /** 스폰 간격 */
    UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 0.1, ClampMax = 30, Units = "s"))
    float SpawnInterval = 5.0f;

    FTimerHandle SpawnTimer;

    virtual void BeginPlay() override;
    void SpawnEnemy();
};
```

## 필수 모듈 의존성
Build.cs에 다음이 포함되어야 함:
- `"AIModule"` - AAIController, Navigation
- `"StateTreeModule"` - StateTree 핵심
- `"GameplayStateTreeModule"` - Gameplay StateTree 통합

## 체크리스트
1. AIController에 `UStateTreeAIComponent` 추가했는가?
2. Build.cs에 StateTreeModule, GameplayStateTreeModule 의존성이 있는가?
3. Enemy 클래스를 `abstract`로 선언했는가? (Blueprint 상속용)
4. EQS 컨텍스트의 `ProvideContext()`를 올바르게 구현했는가?
5. 스포너에 `TSubclassOf<>` 로 스폰 클래스를 지정했는가?
