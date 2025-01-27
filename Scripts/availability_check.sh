#!/bin/bash

check_binance_status() {
  url="https://api.binance.com/api/v3/ping"
  response=$(curl -s -o /dev/null -w "%{http_code}" "$url")

  if [ "$response" -eq 200 ]; then
    echo "Binance API is available."
  else
    echo "Binance API isn't available. Condition code: $response"
  fi
}

check_binance_status
