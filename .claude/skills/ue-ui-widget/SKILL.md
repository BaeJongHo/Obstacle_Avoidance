---
name: ue-ui-widget
description: Unreal Engine 5.7 UMG UI 위젯 생성. UserWidget, WidgetComponent, HUD 위젯 작성 시 사용
---

# UE 5.7 UMG UI 위젯 스킬

## 목적
프로젝트 표준에 맞는 UMG 위젯과 Widget Component 기반 UI를 생성한다.

## 기본 UserWidget 템플릿 (프로젝트 표준)

### 헤더 (.h)
```cpp
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "{WidgetName}.generated.h"

/**
 *  {위젯 설명}
 */
UCLASS(abstract)
class U{WidgetName} : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Blueprint에서 구현할 시각적 업데이트 */
    UFUNCTION(BlueprintImplementableEvent, Category = "{카테고리}")
    void Set{PropertyName}(float Value);

    UFUNCTION(BlueprintImplementableEvent, Category = "{카테고리}")
    void Set{ColorProperty}(FLinearColor Color);
};
```

### 예시: CombatLifeBar (프로젝트 실제 코드)
```cpp
UCLASS(abstract)
class UCombatLifeBar : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 0-1 퍼센트 값으로 라이프바 설정 */
    UFUNCTION(BlueprintImplementableEvent, Category = "Life Bar")
    void SetLifePercentage(float Percent);

    /** 라이프바 색상 설정 */
    UFUNCTION(BlueprintImplementableEvent, Category = "Life Bar")
    void SetBarColor(FLinearColor Color);
};
```

## WidgetComponent로 월드 공간 UI 표시

### 캐릭터 헤더에서 선언
```cpp
class UWidgetComponent;

UCLASS(abstract)
class ACombatCharacter : public ACharacter
{
    // ...

    /** 위젯 컴포넌트 (월드 공간) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* LifeBar;

protected:
    /** 위젯 인스턴스 참조 */
    UPROPERTY(EditAnywhere, Category = "Damage")
    TObjectPtr<UCombatLifeBar> LifeBarWidget;

    /** 위젯 색상 설정 */
    UPROPERTY(EditAnywhere, Category = "Damage")
    FLinearColor LifeBarColor;
};
```

### 생성자에서 컴포넌트 생성
```cpp
ACombatCharacter::ACombatCharacter()
{
    LifeBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("LifeBar"));
    LifeBar->SetupAttachment(RootComponent);
    LifeBar->SetDrawSize(FVector2D(150.0f, 15.0f));
    LifeBar->SetWidgetSpace(EWidgetSpace::Screen); // 또는 World
    LifeBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}
```

### BeginPlay에서 위젯 초기화
```cpp
void ACombatCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 위젯 인스턴스 가져오기
    if (LifeBar)
    {
        LifeBarWidget = Cast<UCombatLifeBar>(LifeBar->GetUserWidgetObject());
        if (LifeBarWidget)
        {
            LifeBarWidget->SetBarColor(LifeBarColor);
            LifeBarWidget->SetLifePercentage(1.0f);
        }
    }
}
```

### 데미지 시 업데이트
```cpp
void ACombatCharacter::ApplyDamage(...)
{
    CurrentHP -= Damage;
    if (LifeBarWidget)
    {
        LifeBarWidget->SetLifePercentage(CurrentHP / MaxHP);
    }
}
```

## HUD 위젯 패턴 (화면 공간)

### PlayerController에서 HUD 생성
```cpp
void A{Variant}PlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && HUDWidgetClass)
    {
        HUDWidget = CreateWidget<U{Variant}UI>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }
}
```

### HUD 위젯 클래스
```cpp
UCLASS(abstract)
class U{Variant}UI : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateScore(int32 NewScore);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateHealth(float HealthPercent);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void ShowMessage(const FText& Message, float Duration);
};
```

## C++에서 Slate 위젯 바인딩 (고급)

### BindWidget으로 Blueprint 위젯 참조
```cpp
UCLASS()
class UMyWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    /** Blueprint에서 같은 이름의 위젯과 자동 바인딩 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TitleText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidgetOptional))
    UImage* IconImage; // 선택적 바인딩

    virtual void NativeConstruct() override;
};
```

## 프로젝트의 C++/Blueprint 분업 패턴

이 프로젝트에서 위젯은 다음과 같이 분업:

| C++ | Blueprint |
|-----|-----------|
| 위젯 클래스 정의 (abstract) | 시각적 레이아웃 |
| BlueprintImplementableEvent 선언 | 이벤트 구현 (애니메이션, 효과) |
| TObjectPtr로 참조 보유 | 위젯 블루프린트 생성 |
| 데이터 업데이트 호출 | 실제 렌더링 |

## 필수 모듈 의존성
Build.cs에 다음이 포함되어야 함:
- `"UMG"` - UserWidget, WidgetComponent
- `"Slate"` - Slate 기본 위젯 (SNew 등 사용 시)

## 체크리스트
1. 위젯 클래스를 `abstract`로 선언했는가? (Blueprint 상속 필수)
2. BlueprintImplementableEvent로 시각적 업데이트를 분리했는가?
3. WidgetComponent에 SetDrawSize, SetWidgetSpace를 설정했는가?
4. BeginPlay에서 GetUserWidgetObject()로 위젯 참조를 가져왔는가?
5. Build.cs에 UMG 의존성이 있는가?
6. PlayerController에서 IsLocalController() 확인 후 위젯을 생성했는가?
7. FLinearColor로 색상을 UPROPERTY 노출했는가?
