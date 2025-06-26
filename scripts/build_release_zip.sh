#!/bin/bash
set -e

# 0. build 디렉토리의 앱 번들을 루트로 복사
rm -rf client_gui.app
cp -R build/client_gui.app .

# 1. macdeployqt로 번들화
macdeployqt client_gui.app

# 2. QtDBus.framework 수동 복사 (의존성 누락 방지)
if [ -d /opt/homebrew/opt/qt@6/lib/QtDBus.framework ]; then
  sudo cp -R /opt/homebrew/opt/qt@6/lib/QtDBus.framework client_gui.app/Contents/Frameworks/
  sudo chown -R $(whoami) client_gui.app/Contents/Frameworks/QtDBus.framework
fi

# 2-1. QtDBus가 필요로 하는 libdbus-1.3.dylib도 번들에 복사
if [ -f /opt/homebrew/opt/dbus/lib/libdbus-1.3.dylib ]; then
  cp /opt/homebrew/opt/dbus/lib/libdbus-1.3.dylib client_gui.app/Contents/Frameworks/
  
  # 2-2. QtDBus.framework의 libdbus-1.3.dylib 의존성 경로를 번들 내부로 변경
  install_name_tool -change /opt/homebrew/opt/dbus/lib/libdbus-1.3.dylib @executable_path/../Frameworks/libdbus-1.3.dylib client_gui.app/Contents/Frameworks/QtDBus.framework/Versions/A/QtDBus
  
  # 2-3. libdbus-1.3.dylib의 ID를 번들 내부 경로로 변경
  install_name_tool -id @executable_path/../Frameworks/libdbus-1.3.dylib client_gui.app/Contents/Frameworks/libdbus-1.3.dylib
fi

# 3. 코드 서명 (ad-hoc)
codesign --deep --force --sign - client_gui.app

# 4. quarantine 속성 제거
sudo xattr -dr com.apple.quarantine client_gui.app

# 5. 기존 zip 삭제 후 새로 압축
rm -f client_gui.zip
zip -r client_gui.zip client_gui.app

# 6. zip 파일에도 quarantine 제거
sudo xattr -dr com.apple.quarantine client_gui.zip

echo "✅ client_gui.zip 파일이 준비되었습니다! 사용자는 압축 해제 후 바로 실행하면 됩니다." 