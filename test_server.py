import requests
import json

url = "http://localhost:8080/application/json"
headers = {"Content-Type": "application/json"}

payload = {
    "strategy": "arima garch",
    "exchange_api": {
        "exchange": "Binance",
        "api_key": "YOUR_API_KEY_HERE",
        "secret_key": "YOUR_SECRET_KEY_HERE"
    },
    "filepath": "aa",
    "parameters": {
        "window_size": 10,
        "sma_short": 5.0,
        "sma_long": 10.0,
        "rsi_oversold": 30.0,
        "rsi_overbought": 29.0,
        "stop_loss": 0.05
    },
    "symbol": "BTCUSDT",
    "data_frame": "5m",
    "is_dynamic": False,
    "balance": 1000.0
}

response = requests.post(url, headers=headers, data=json.dumps(payload))
print(response.status_code)
print(response.text)
