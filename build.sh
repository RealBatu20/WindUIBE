#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
: "${ANDROID_NDK_HOME:?Point ANDROID_NDK_HOME at NDK r28c}"
: "${ANDROID_API:=26}"
BIN="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin"
CXX="$BIN/aarch64-linux-android${ANDROID_API}-clang++"
test -x "$CXX" || { echo "Missing NDK compiler: $CXX" >&2; exit 1; }
mkdir -p build dist
python3 tools/make_bridge_dex.py build/bridge.dex
python3 tools/embed.py
"$CXX" -std=c++17 -O2 -fPIC -shared -DNDEBUG -fvisibility=hidden \
 -fno-exceptions -fno-rtti -unwindlib=none -Wl,-z,max-page-size=16384 -Wl,--no-undefined \
 -Ibuild src/loader.cpp src/demo.cpp -llog -o dist/libWindUI.so
"$BIN/llvm-strip" --strip-debug dist/libWindUI.so
python3 tools/package.py
