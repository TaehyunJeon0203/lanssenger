#!/bin/bash
set -e

# 1. macdeployqt로 번들화 및 dmg 생성
macdeployqt client_gui.app -dmg

# 2. 코드 서명 (ad-hoc)
codesign --deep --force --sign - client_gui.app

# 3. quarantine 속성 제거
xattr -dr com.apple.quarantine client_gui.app
xattr -dr com.apple.quarantine client_gui.dmg

echo "✅ dmg 파일이 준비되었습니다! 사용자는 dmg를 열고 복사 후 실행만 하면 됩니다." 