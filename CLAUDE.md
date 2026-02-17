# Obstacle_Avoidance - Unreal Engine 5.7 프로젝트

## 프로젝트 개요
- 프로젝트 이름: Obstacle_Avoidance
- 엔진 버전: Unreal Engine 5.7
- 주요 시스템: Enhanced Input, StateTree AI, UMG UI
- 플러그인: UnrealClaude (MCP 통합), AIAssistant, StateTree, GameplayStateTree

## 아키텍처
- 싱글 모듈 구조 (`TestAI`)
- Variant 패턴으로 게임 모드 분리 (Combat, Platforming, SideScrolling)
- 각 Variant는 독립적인 Character, GameMode, PlayerController를 가짐
- 인터페이스 기반 기능 분리 (ICombatDamageable, ICombatAttacker 등)
- StateTree 기반 AI 시스템

## 모듈 의존성
Core, CoreUObject, Engine, InputCore, EnhancedInput, AIModule, StateTreeModule, GameplayStateTreeModule, UMG, Slate

## 프로젝트 구조
```
Source/TestAI/
├── TestAI.Build.cs              # 모듈 빌드 설정
├── TestAI.h / .cpp              # 모듈 정의
├── TestAICharacter              # 기본 캐릭터
├── TestAIGameMode               # 기본 게임모드
├── TestAIPlayerController       # 기본 플레이어 컨트롤러
├── MyActor                      # 커스텀 액터
├── PhysicsBoxActor              # 물리 박스 액터
├── ProjectileSpawnerComponent   # 투사체 스포너 컴포넌트
├── Variant_Combat/              # 전투 게임 모드
│   ├── CombatCharacter          # 콤보/차지 공격 기능 포함 캐릭터
│   ├── CombatGameMode
│   ├── CombatPlayerController
│   ├── AI/                      # StateTree 기반 AI
│   │   ├── CombatAIController
│   │   ├── CombatEnemy / Spawner
│   │   ├── CombatStateTreeUtility
│   │   └── EnvQueryContext_*    # EQS 컨텍스트
│   ├── Animation/               # 애니메이션 관련
│   ├── Gameplay/                # 게임플레이 오브젝트
│   │   ├── CombatActivationVolume
│   │   ├── CombatCheckpointVolume
│   │   ├── CombatDamageableBox / Dummy
│   │   └── CombatLavaFloor
│   ├── Interfaces/              # 인터페이스
│   │   ├── ICombatActivatable
│   │   ├── ICombatAttacker
│   │   └── ICombatDamageable
│   └── UI/                      # 위젯
│       └── CombatLifeBar
├── Variant_Platforming/         # 플랫포밍 게임 모드
│   ├── PlatformingCharacter
│   ├── PlatformingGameMode
│   ├── PlatformingPlayerController
│   └── Animation/
└── Variant_SideScrolling/       # 사이드스크롤 게임 모드
    ├── SideScrollingCharacter
    ├── SideScrollingGameMode
    ├── SideScrollingPlayerController
    ├── SideScrollingCameraManager
    ├── AI/                      # StateTree 기반 AI
    │   ├── SideScrollingAIController
    │   ├── SideScrollingNPC
    │   └── SideScrollingStateTreeUtility
    ├── Gameplay/                # 게임플레이 오브젝트
    │   ├── SideScrollingJumpPad
    │   ├── SideScrollingMovingPlatform
    │   ├── SideScrollingPickup
    │   └── SideScrollingSoftPlatform
    ├── Interfaces/
    │   └── ISideScrollingInteractable
    └── UI/
        └── SideScrollingUI
```

## 코딩 표준

### 1. 클래스 네이밍 - "OA" Prefix 필수
모든 C++ 클래스명 앞에 반드시 `OA` 접두사를 붙인다. UE 타입 접두사(A, U, F, I, E) 뒤에 OA를 붙인다.
- Actor: `AOAPlayerCharacter`, `AOAEnemySpawner`
- UObject: `UOAInventorySystem`, `UOACombatLifeBar`
- Struct: `FOADamageInfo`, `FOASpawnData`
- Interface: `UOADamageable` / `IOADamageable`
- Enum: `EOACharacterState`

### 2. 폴더 구조 - 상속 기반 클래스 분류
클래스를 상속받는 부모 클래스 기준으로 폴더를 나눠서 배치한다.
```
Source/TestAI/
├── Actor/                  # AActor 상속 클래스
├── Character/              # ACharacter 상속 클래스
├── Controller/             # AController (APlayerController, AAIController) 상속 클래스
├── GameMode/               # AGameModeBase 상속 클래스
├── Component/              # UActorComponent 상속 클래스
├── Widget/                 # UUserWidget 상속 클래스
├── Interface/              # UInterface 상속 클래스
├── AnimInstance/            # UAnimInstance 상속 클래스
├── FunctionLibrary/        # UBlueprintFunctionLibrary 상속 클래스
└── Data/                   # UObject, FStruct, Enum 등
```

### 3. 변수명/함수명 - Epic C++ 코딩 표준 준수
언리얼 엔진 공식 Epic C++ 코딩 표준(https://dev.epicgames.com/documentation/ko-kr/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine)을 엄격히 따른다.
- 변수: PascalCase (예: `MaxHealth`, `AttackDamage`)
- bool: `b` 접두사 (예: `bIsAttacking`, `bCanMove`)
- 함수: PascalCase 동사형 (예: `GetHealth()`, `ApplyDamage()`, `IsAlive()`)
- 파라미터: PascalCase (예: `InDamage`, `OutResult` - In/Out 접두사 권장)
- 로컬 변수: PascalCase
- 열거형 값: PascalCase (예: `EOACharacterState::Idle`)
- 모든 타입에 UE 접두사 사용: `int32`, `float`, `FString`, `FVector`, `TArray<>`, `TMap<>`, `TObjectPtr<>`

### 4. Blueprint 에셋 네이밍 - Prefix 규칙
- Blueprint 클래스: `BP_` 접두사 (예: `BP_OAPlayerCharacter`, `BP_OAEnemySpawner`)
- AnimBlueprint 클래스: `ABP_` 접두사 (예: `ABP_OAPlayerCharacter`, `ABP_OAEnemy`)
- Widget Blueprint: `WBP_` 접두사 (예: `WBP_OAHealthBar`)
- 기타 에셋 Prefix 참고: `M_` (Material), `T_` (Texture), `SK_` (SkeletalMesh), `SM_` (StaticMesh), `AM_` (AnimMontage)

### 5. 최적화 우선 원칙
코드 작성 시 성능 최적화를 최우선으로 고려한다.
- `FORCEINLINE` 적극 활용 (간단한 getter/setter)
- `const` 참조로 불필요한 복사 방지 (예: `const FVector&`, `const FString&`)
- `TObjectPtr<>` 사용 (UE 5.x 가비지 컬렉션 최적화)
- Tick 함수 최소화: 가능하면 이벤트/델리게이트 기반으로 처리, 불필요한 Tick은 `PrimaryActorTick.bCanEverTick = false`
- `UPROPERTY()` 미사용 UObject 포인터 금지 (GC 누수 방지)
- 대량 연산은 SweepMulti보다 Overlap 우선 검토
- 문자열 비교 대신 `FGameplayTag` 또는 `FName` 사용
- `TArray` 사전 Reserve: `Array.Reserve(ExpectedSize)` 활용
- `FindComponentByClass<>()` 반복 호출 대신 캐싱
- Blueprint 노출은 필요한 것만: 무분별한 `BlueprintCallable` 지양

## MCP 서버 구성

### 설치된 MCP 서버
```
.claude/settings.json    # MCP 서버 설정
MCP/
├── unreal-analyzer-mcp/ # 코드 분석 서버 (소스 빌드)
├── unreal-mcp/          # Python Remote Execution 참조
└── setup.sh             # 설치/복구 스크립트
```

### 1. UnrealClaude (내장 - 에디터 통합)
- 위치: `Plugins/UnrealClaude/Resources/mcp-bridge/`
- 기능: 에디터 제어 (액터 생성/이동/삭제, Blueprint 수정, 애니메이션, 머티리얼, Enhanced Input, 레벨 관리)
- 컨텍스트: animation, blueprint, slate, actor, assets, replication, enhanced_input, character, material, statetree, parallel_workflows
- 포트: `http://localhost:3000`

### 2. Unreal Analyzer (코드 분석)
- 위치: `MCP/unreal-analyzer-mcp/`
- 기능: C++ 클래스 분석, 상속 계층 매핑, 코드 패턴 감지, 서브시스템 분석
- 플러그인 불필요 (소스 코드만 분석)

### 3. Runreal Unreal MCP (Python 스크립트)
- 실행: `npx -y @runreal/unreal-mcp`
- 기능: UE Python Remote Execution을 통한 에디터 스크립팅
- 필요 설정: UE 에디터에서 Python Editor Script Plugin + Remote Execution 활성화
- 플러그인 불필요

### MCP 서버 재설치
```bash
bash MCP/setup.sh
```

## 주의사항
- 모든 Variant 폴더의 Include Path가 Build.cs에 등록되어 있음
- `TObjectPtr<>` 스마트 포인터 사용 (UE 5.x 권장)
- AnimMontage 기반 공격 시스템 (Combo + Charged)
- `FOnMontageEnded` 델리게이트로 애니메이션 완료 감지
- `FTimerHandle`로 리스폰 등 지연 처리
