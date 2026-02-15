#!/bin/bash
# =============================================================================
# TestAI MCP Server Setup Script
# =============================================================================
# 이 스크립트는 TestAI 프로젝트의 MCP 서버들을 설치/복구합니다.
#
# 사용법:
#   cd <ProjectRoot>
#   bash MCP/setup.sh
# =============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "============================================"
echo "  TestAI MCP Server Setup"
echo "============================================"
echo ""
echo "Project Root: $PROJECT_ROOT"
echo ""

# -----------------------------------------------
# 1. UnrealClaude 내장 MCP (이미 설치됨)
# -----------------------------------------------
echo "[1/3] UnrealClaude MCP Bridge 확인..."
MCP_BRIDGE="$PROJECT_ROOT/Plugins/UnrealClaude/Resources/mcp-bridge"

if [ -f "$MCP_BRIDGE/index.js" ]; then
    echo "  ✅ UnrealClaude MCP Bridge 발견"
    if [ ! -d "$MCP_BRIDGE/node_modules" ]; then
        echo "  📦 의존성 설치 중..."
        cd "$MCP_BRIDGE" && npm install --production
    else
        echo "  ✅ 의존성 이미 설치됨"
    fi
else
    echo "  ❌ UnrealClaude 플러그인을 먼저 설치하세요"
    echo "     https://github.com/Natfii/UnrealClaude"
fi
echo ""

# -----------------------------------------------
# 2. Unreal Analyzer MCP (코드 분석)
# -----------------------------------------------
echo "[2/3] Unreal Analyzer MCP 설치..."
ANALYZER_DIR="$PROJECT_ROOT/MCP/unreal-analyzer-mcp"

if [ ! -d "$ANALYZER_DIR" ]; then
    echo "  📥 저장소 클론 중..."
    git clone --depth 1 https://github.com/ayeletstudioindia/unreal-analyzer-mcp.git "$ANALYZER_DIR"
fi

if [ ! -d "$ANALYZER_DIR/node_modules" ]; then
    echo "  📦 의존성 설치 중..."
    cd "$ANALYZER_DIR" && npm install
fi

if [ ! -f "$ANALYZER_DIR/build/index.js" ]; then
    echo "  🔨 빌드 중..."
    cd "$ANALYZER_DIR" && npx tsc
fi

if [ -f "$ANALYZER_DIR/build/index.js" ]; then
    echo "  ✅ Unreal Analyzer MCP 준비 완료"
else
    echo "  ❌ 빌드 실패. 수동으로 확인하세요."
fi
echo ""

# -----------------------------------------------
# 3. Runreal Unreal MCP (Python Remote Execution)
# -----------------------------------------------
echo "[3/3] Runreal Unreal MCP 확인..."
echo "  ℹ️  npx로 실행되므로 별도 설치 불필요"
echo "  ℹ️  사용 시 자동 다운로드됩니다"
echo "  ⚠️  UE 에디터에서 'Python Editor Script Plugin'과"
echo "     'Remote Execution' 옵션을 활성화해야 합니다"
echo ""

# -----------------------------------------------
# 설정 확인
# -----------------------------------------------
echo "============================================"
echo "  설정 확인"
echo "============================================"

if [ -f "$PROJECT_ROOT/.claude/settings.json" ]; then
    echo "✅ .claude/settings.json 존재"
else
    echo "❌ .claude/settings.json 없음 - 생성 필요"
fi

if [ -f "$PROJECT_ROOT/CLAUDE.md" ]; then
    echo "✅ CLAUDE.md 존재"
else
    echo "❌ CLAUDE.md 없음"
fi

echo ""
echo "============================================"
echo "  설치 완료!"
echo "============================================"
echo ""
echo "사용 방법:"
echo "  1. UE 에디터를 실행하세요 (UnrealClaude 플러그인 활성화)"
echo "  2. Claude Code를 프로젝트 루트에서 실행하세요"
echo "  3. MCP 서버가 자동으로 연결됩니다"
echo ""
echo "MCP 서버 목록:"
echo "  - unrealclaude  : 에디터 통합 (액터, Blueprint, 애니메이션 등)"
echo "  - unreal-analyzer: 코드 분석 (클래스 계층, 패턴 감지)"
echo "  - unreal-python  : Python 스크립트 실행 (Remote Execution)"
echo ""
