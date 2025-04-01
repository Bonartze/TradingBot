#!/bin/bash
chmod +x ./boost_install
./boost_install
mkdir -p ../TradingStrategies/build && cd ../TradingStrategies/build

cmake ../ && \
make -j`nproc` || echo "Error while generating strategy API" && \
sudo make install

echo "Strategy API is generated"


mkdir -p ../../TradingEngine/build && \
cd ../../TradingEngine/build
cmake ../ && \
make -j`nproc` || echo "Error while building server"
