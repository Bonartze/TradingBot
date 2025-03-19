#!/bin/bash
mkdir -p ../TradingEngine/build && \
cd ../TradingEngine/build
cmake ../ && \
make -j`nproc` || echo "Error while building server"

# generating trading strategy API

mkdir -p ../TradingStrategies/build && cd ../TradingStrategies/build

cmake ../ && \
make -j`nproc` || echo "Error while generating strategy API" && \
sudo make install

echo "Strategy API is generated"

# running server as a demon process

nohup ./server > server.log 2>&1 &

disown

echo "Server started as a daemon. PID: $"