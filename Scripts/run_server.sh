#!/bin/bash
mkdir -p ../TradingServer/build && cd ../TradingServer/build
cmake ../ && make -j`nproc` || echo "Error while building server"

nohup ./server > server.log 2>&1 & # run server as a demon process

disown

echo "Server started as a daemon. PID: $"