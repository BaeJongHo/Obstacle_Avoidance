---
name: ue-blueprint-node
description: Blueprint에 노출할 C++ 함수 및 프로퍼티 생성. BlueprintCallable, BlueprintPure, BlueprintImplementableEvent 함수 작성 시 사용
---

# UE 5.7 Blueprint 노출 함수/프로퍼티 생성 스킬

## 목적
C++ 코드를 Blueprint에 올바르게 노출하는 UPROPERTY/UFUNCTION 작성을 표준화한다.

## UFUNCTION 지정자 가이드

### BlueprintCallable (실행 핀 있음, 사이드 이펙트 허용)
```cpp
UFUNCTION(BlueprintCallable, Category = "TestAI|카테고리")
void DoSomething();

// 월드 컨텍스트가 필요한 static 함수
UFUNCTION(BlueprintCallable, Category = "TestAI|Utilities",
    meta = (WorldContext = "WorldContextObject"))
static void UtilityFunction(const UObject* WorldContextObject, FVector Location);
```

### BlueprintPure (실행 핀 없음, const 함수)
```cpp
UFUNCTION(BlueprintPure, Category = "TestAI|Utilities")
static bool IsValidTarget(AActor* Target);

// inline getter 패턴 (프로젝트 표준)
FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
```

### BlueprintImplementableEvent (Blueprint에서 구현)
```cpp
// 프로젝트 패턴: Blueprint에서 시각적/효과 처리
UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
void DealtDamage(float Damage, const FVector& ImpactPoint);

UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
void ReceivedDamage(float Damage, const FVector& ImpactPoint, const FVector& DamageDirection);
```

### BlueprintNativeEvent (C++ 기본 구현 + Blueprint 오버라이드)
```cpp
UFUNCTION(BlueprintNativeEvent, Category = "TestAI|Gameplay")
void OnInteract(AActor* Interactor);
virtual void OnInteract_Implementation(AActor* Interactor);
```

## UPROPERTY 지정자 가이드

### 컴포넌트 (프로젝트 표준)
```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components",
    meta = (AllowPrivateAccess = "true"))
USpringArmComponent* CameraBoom;
```

### 편집 가능 + 범위 제한 (프로젝트 표준)
```cpp
UPROPERTY(EditAnywhere, Category = "Damage", meta = (ClampMin = 0, ClampMax = 100))
float MaxHP = 5.0f;

UPROPERTY(EditAnywhere, Category = "Melee Attack|Trace",
    meta = (ClampMin = 0, ClampMax = 500, Units = "cm"))
float MeleeTraceDistance = 75.0f;
```

### 입력 액션 (Enhanced Input 패턴)
```cpp
UPROPERTY(EditAnywhere, Category = "Input")
UInputAction* JumpAction;
```

### 에셋 참조 (UE 5.x 권장)
```cpp
UPROPERTY(EditAnywhere)
TObjectPtr<UTexture2D> Texture;       // Hard 참조

UPROPERTY(EditAnywhere)
TSoftObjectPtr<UStaticMesh> MeshAsset; // Soft 참조
```

## 유용한 meta 지정자
- `AllowPrivateAccess = "true"` - private 멤버를 Blueprint에 노출
- `ClampMin / ClampMax` - 에디터에서 값 범위 제한
- `Units = "cm"` / `Units = "s"` - 에디터에 단위 표시
- `WorldContext = "WorldContextObject"` - static 함수에 월드 컨텍스트 전달
- `AutoCreateRefTerm = "Param"` - 자동 레퍼런스 생성
- `AdvancedDisplay = "Param1, Param2"` - 고급 핀으로 숨김
- `CompactNodeTitle = "Text"` - 컴팩트 노드 제목
- `MakeStructureDefaultValue` - 구조체 기본값

## 카테고리 네이밍 규칙 (프로젝트 표준)
- 최상위: `"Components"`, `"Input"`, `"Damage"`, `"Combat"`, `"Camera"`, `"Respawn"`
- 하위 카테고리: `"Melee Attack|Trace"`, `"Melee Attack|Damage"`, `"Melee Attack|Combo"`, `"Melee Attack|Charged"`

## 체크리스트
1. UFUNCTION/UPROPERTY에 Category 지정했는가?
2. const 함수는 BlueprintPure를 사용했는가?
3. 수치 값에 ClampMin/ClampMax를 설정했는가?
4. 거리/시간 값에 Units meta를 추가했는가?
5. private 컴포넌트에 AllowPrivateAccess를 설정했는가?
6. .generated.h 헤더를 마지막에 include 했는가?
