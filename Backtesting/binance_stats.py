import urllib.request
import zipfile
import os
import pandas as pd
import mplfinance as mpf
from Common.usable_metrics import *

os.makedirs("Data_zips", exist_ok=True)
os.makedirs("Data_extracted", exist_ok=True)
os.makedirs(f"Data_zips/{time_interval}", exist_ok=True)
os.makedirs(f"Data_extracted/{time_interval}", exist_ok=True)
os.makedirs(f"StatisticsPlots/{crypto_pair}", exist_ok=True)
os.makedirs(f"StatisticsPlots/{crypto_pair}/{time_interval}", exist_ok=True)

months = [f'{year}-{month:02d}' for year in range(2020, 2025) for month in range(1, 13)]
months = months[7:]
months = months[:months.index('2024-10') + 1]

for month in months:
    zip_file_path = f"Data_zips/{time_interval}/{crypto_pair}-{time_interval}-{month}.zip"
    csv_file_path = f"Data_extracted/{time_interval}/{crypto_pair}-{time_interval}-{month}.csv"

    try:
        urllib.request.urlretrieve(
            f"{binance_ref}/{time_interval}/{crypto_pair}-{time_interval}-{month}.zip",
            zip_file_path)

        with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
            zip_ref.extractall(f'./Data_extracted/{time_interval}')
    except:
        print(f"Data for {month} is not available, skipping.")

dfs = []
for month in months:
    csv_file_path = f"Data_extracted/{time_interval}/{crypto_pair}-{time_interval}-{month}.csv"
    print(csv_file_path)
    if os.path.exists(csv_file_path):
        df = pd.read_csv(csv_file_path, header=None)
        df.columns = parameters
        dfs.append(df)

df = pd.concat(dfs)

df['timestamp'] = pd.to_datetime(df['timestamp'], unit='ms')
df.set_index('timestamp', inplace=True)

mpf.plot(df, type='candle', style='charles', volume=True, title=f'{crypto_pair} Candlestick Chart ({time_interval})',
         ylabel='Price', ylabel_lower='Volume', mav=(3, 6, 9),
         savefig=f'./StatisticsPlots/{crypto_pair}/{time_interval}/{crypto_pair}_{time_interval}_candlestick_chart_{time_interval}.png')
