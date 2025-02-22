import urllib.request
import zipfile
import os
import pandas as pd
import mplfinance as mpf
import sys

binance_ref = "https://data.binance.vision/data/spot/monthly/klines"

crypto_pair = sys.argv[1]  # "BTCUSDT"
time_interval = sys.argv[2]  # "15m"
year = sys.argv[3]
parameters = ['timestamp', 'open', 'high', 'low', 'close', 'volume', 'close_time', 'quote_asset_volume',
              'number_of_trades', 'taker_buy_base', 'taker_buy_quote', 'ignore']

os.makedirs("../data/Data_zips", exist_ok=True)
os.makedirs("../data/Data_extracted", exist_ok=True)
os.makedirs(f"../data/Data_zips/{time_interval}/{crypto_pair}/{year}", exist_ok=True)
os.makedirs(f"../data/Data_extracted/{time_interval}/{crypto_pair}/{year}", exist_ok=True)
os.makedirs(f"../data/StatisticsPlots/{crypto_pair}", exist_ok=True)
os.makedirs(f"../data/StatisticsPlots/{crypto_pair}/{time_interval}/{year}", exist_ok=True)

months = [f'{year}-{month:02d}' for month in range(1, 13)]

for month in months:
    zip_file_path = f"../data/Data_zips/{time_interval}/{crypto_pair}/{year}/{crypto_pair}-{time_interval}-{month}.zip"
    csv_file_path = f"../data/Data_extracted/{time_interval}/{crypto_pair}/{year}/{crypto_pair}-{time_interval}-{month}.csv"

    try:
        urllib.request.urlretrieve(
            f"{binance_ref}/{crypto_pair}/{time_interval}/{crypto_pair}-{time_interval}-{month}.zip",
            zip_file_path)

        with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
            zip_ref.extractall(f'../data/Data_extracted/{time_interval}/{crypto_pair}/{year}')
    except Exception as e:
        print(f"Data for {month} is not available or could not be processed. Skipping. Error: {e}")

dfs = []
for month in months:
    csv_file_path = f"../data/Data_extracted/{time_interval}/{crypto_pair}/{year}/{crypto_pair}-{time_interval}-{month}.csv"
    if os.path.exists(csv_file_path):
        df = pd.read_csv(csv_file_path, header=None)
        df.columns = parameters
        dfs.append(df)

df = pd.concat(dfs)

df['timestamp'] = pd.to_datetime(df['timestamp'], unit='ms')
df.set_index('timestamp', inplace=True)

mpf.plot(df, type='candle', style='charles', volume=True,
         title=f'{crypto_pair} Candlestick Chart ({time_interval})',
         ylabel='Price', ylabel_lower='Volume', mav=(3, 6, 9),
         savefig=f'../data/StatisticsPlots/{crypto_pair}/{time_interval}/{crypto_pair}_{time_interval}_candlestick_chart.png')
