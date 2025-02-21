import subprocess

for year in range(2020, 2026):
    result = subprocess.run(
        ["python3", "binance_stats.py", "BTCUSDT", "5m", str(year)],
        capture_output=True,
        text=True
    )
