#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

BOOST_VERSION="1.86.0"
BOOST_DIR="boost_1_86_0"
BOOST_ARCHIVE="boost_${BOOST_VERSION//./_}.tar.gz"

echo "[INFO] Installing Boost ${BOOST_VERSION}..."
cd "$SCRIPT_DIR"
wget -c https://archives.boost.io/release/${BOOST_VERSION}/source/${BOOST_ARCHIVE}
tar -xzf ${BOOST_ARCHIVE}
cd ${BOOST_DIR}
./bootstrap.sh
sudo ./b2 install
cd "$SCRIPT_DIR"
echo "[INFO] Boost installed successfully."

echo "[INFO] Building TradingStrategies..."
mkdir -p "$PROJECT_ROOT/TradingStrategies/build"
cd "$PROJECT_ROOT/TradingStrategies/build"
cmake ..
make -j"$(nproc)"
sudo make install
echo "[INFO] TradingStrategies built and installed."

echo "[INFO] Building TradingEngine..."
mkdir -p "$PROJECT_ROOT/TradingEngine/build"
cd "$PROJECT_ROOT/TradingEngine/build"
cmake ..
make -j"$(nproc)"
echo "[INFO] TradingEngine built successfully."
