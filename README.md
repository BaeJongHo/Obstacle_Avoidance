# Obstacle Avoidance - Unreal Engine 5.7 Sample Project

**100% Claude Code로 개발된 Unreal Engine 샘플 프로젝트**

이 프로젝트는 기획부터 C++ 코드 작성, 레벨 구성까지 모든 개발 과정을 [Claude Code](https://docs.anthropic.com/en/docs/claude-code)만으로 진행한 장애물 피하기 게임입니다. 사람이 직접 코드를 타이핑하거나 에디터에서 수동으로 배치하지 않고, Claude Code와의 대화만으로 완성했습니다.

## 프로젝트 개요

| 항목 | 내용 |
|------|------|
| 엔진 | Unreal Engine 5.7 |
| 언어 | C++ |
| 입력 시스템 | Enhanced Input System |
| AI 시스템 | StateTree |
| UI 시스템 | UMG (Unreal Motion Graphics) |

### Platforming - 장애물 달리기

3인칭 시점의 장애물 코스를 돌파하는 메인 게임 모드입니다.

- 다단계 스테이지 구성
- 코요테 타임, 이중 점프, 벽 점프 등 플랫포머 기본기 구현
- 대시, 슬라이딩 등 추가 이동 기능

**장애물 목록:**
| 장애물 | 설명 |
|--------|------|
| Moving Platform | X/Y축으로 왕복하는 이동 발판 |
| Ice Surface | 마찰력이 극도로 낮은 빙판 |
| Jump Pad | 플레이어를 공중으로 발사하는 점프대 |
| Conveyor Belt | UV 스크롤 애니메이션이 적용된 컨베이어 벨트 |
| Rotating Pillar | 회전하며 플레이어를 밀어내는 기둥 |
| Cannon | 주기적으로 포탄을 발사하는 대포 |
| Laser Beam | 토글 방식의 레이저 빔 |
| Trap Floor | 밟으면 무너졌다 재생성되는 함정 발판 |

## Claude Code로 개발한다는 것

이 프로젝트의 모든 C++ 코드, 클래스 설계, 아키텍처 결정은 Claude Code와의 대화를 통해 이루어졌습니다.

- C++ 헤더/소스 파일 작성 및 수정
- 클래스 구조 설계 및 리팩토링
- 버그 수정 및 디버깅
- Git 커밋 관리

MCP(Model Context Protocol) 서버를 통해 Unreal Editor와 연동하여 에디터 내 작업도 Claude Code로 수행했습니다.

- 액터 배치 및 트랜스폼 조정
- Blueprint 속성 설정
- 레벨 구성

## 필요 환경

- Unreal Engine 5.7
- Visual Studio 2022 또는 Rider
- Windows 10/11

## 라이선스

이 프로젝트는 학습 및 참고 목적의 샘플 프로젝트입니다.