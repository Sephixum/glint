#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DIR="${SCRIPT_DIR}/install"
SOURCE_DIR="${SCRIPT_DIR}/src"

# 1. Clone ONLY the specific tag (no history, no bloat)
if [ ! -d "$SOURCE_DIR" ]; then
    git clone --depth 1 --branch release-3.4.12 https://github.com/libsdl-org/SDL.git "$SOURCE_DIR"
fi

# 2. Remove the 200MB of tests/examples you don't need
rm -rf "$SOURCE_DIR/test" "$SOURCE_DIR/examples" "$SOURCE_DIR/android-project" "$SOURCE_DIR/Xcode"

# 3. Build out-of-source with CMake (fastest, most reliable)
mkdir -p "$SOURCE_DIR/build"
cd "$SOURCE_DIR/build"

cmake .. \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
    -DBUILD_SHARED_LIBS=OFF \
    -DSDL_TESTS=OFF \
    -DSDL_EXAMPLES=OFF \
    -DSDL_INSTALL_TESTS=OFF

make -j$(nproc)
make install

# 4. Generate the flags file (absolute paths!)
cat > "${SCRIPT_DIR}/sdl3.gvf" <<EOF
-I${INSTALL_DIR}/include/SDL3
-L${INSTALL_DIR}/lib
-lSDL3
EOF

echo "SDL3 built and flags written to ${SCRIPT_DIR}/sdl3.gvf"
