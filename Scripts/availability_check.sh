#!/bin/bash

urls=(
  "https://api.binance.com/api/v3/ping"
  "https://api.kraken.com/0/public/SystemStatus"
  "https://www.okx.com/api/v5/public/time"
)

check_exchange_status() {
  local url="$1"

  response=$(curl -s -o /dev/null -w "%{http_code}" "$url")

  if [ "$response" -eq 200 ]; then
    echo "$url API is available."
  else
    echo "$url API isn't available. Condition code: $response"
  fi
}


for url in "${urls[@]}"
do
  check_exchange_status "$url"
done


