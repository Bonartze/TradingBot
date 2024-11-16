import json
import sqlite3

data = {
    "request_type": "execute_strategy",
    "strategy": {
        "strategy_id": "1",
        "name": "Moving Average Crossover",
        "parameters": {
            "short_window": 20,
            "long_window": 50
        }
    },
    "market_data": {
        "symbol": "BTCUSDT",
        "time_frame": "1h",
        "start_date": "2023-01-01",
        "end_date": "2023-12-31"
    },
    "risk_management": {
        "max_drawdown": 0.1,
        "trade_volume_limit": 10000,
        "leverage_limit": 2
    },
    "notification": {
        "send_email": True,
        "email_address": "client@example.com"
    }
}

with open('request.json', 'w') as f:
    json.dump(data, f, indent=4)

con = sqlite3.connect("trading_data.db")
con = con.cursor()
cur.execute("INSERT  ")
