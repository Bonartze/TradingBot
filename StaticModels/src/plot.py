import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score

# Пути к файлам
real_data_file = "../../Backtesting/data/Data_extracted/5m/BTCUSDT/2020/BTCUSDT-5m-2020-01.csv"
arima_forecast_file = "./arima_forecast.csv"
combined_forecast_file = "./combined_forecast.csv"

# 1) Загрузка реальных данных (Binance kline)
df = pd.read_csv(real_data_file, header=None)
df.columns = [
    'open_time', 'open', 'high', 'low', 'close', 'volume',
    'close_time', 'quote_asset_volume', 'num_trades',
    'taker_buy_base_asset_volume', 'taker_buy_quote_asset_volume', 'ignore'
]
df['open_time'] = pd.to_datetime(df['open_time'], unit='ms')
real_close = df['close'].astype(float)

# 2) Делим на train/test (90% - train, 10% - test)
train_ratio = 0.9
train_size = int(len(real_close) * train_ratio)
train_real = real_close.iloc[:train_size].reset_index(drop=True)
test_real = real_close.iloc[train_size:].reset_index(drop=True)

# 3) Загрузка прогнозов (должны содержать len(test_real) точек)
arima_forecast = pd.read_csv(arima_forecast_file, header=None).iloc[:, 0]
combined_forecast = pd.read_csv(combined_forecast_file, header=None).iloc[:, 0]

# Синхронизируем длины, если нужно
common_len = min(len(test_real), len(arima_forecast), len(combined_forecast))
test_real = test_real.iloc[:common_len].reset_index(drop=True)
arima_forecast = arima_forecast.iloc[:common_len].reset_index(drop=True)
combined_forecast = combined_forecast.iloc[:common_len].reset_index(drop=True)


# Функция для вычисления метрик, возвращающая строку
def get_metrics_str(y_true, y_pred, model_name):
    mae = mean_absolute_error(y_true, y_pred)
    rmse = np.sqrt(mean_squared_error(y_true, y_pred))
    mape = np.mean(np.abs((y_true - y_pred) / y_true)) * 100
    r2 = r2_score(y_true, y_pred)
    metrics = (
            f"=== {model_name} ===\n"
            f"MAE: {mae:.4f}\n"
            f"RMSE: {rmse:.4f}\n"
            f"MAPE: {mape:.2f}%\n"
            f"R²: {r2:.4f}\n"
            + "=" * 30 + "\n"
    )
    return metrics


# 4) Вычисляем метрики и формируем общий вывод
metrics_arima = get_metrics_str(test_real, arima_forecast, "ARIMA")
metrics_combined = get_metrics_str(test_real, combined_forecast, "ARIMA + GARCH")

all_metrics_output = metrics_arima + "\n" + metrics_combined

# Печатаем в консоль
print(all_metrics_output)

# Сохраняем эти результаты в файл (не .txt, а, например, .md)
with open("../ModelsMetrics/results.md", "w") as f:
    f.write(all_metrics_output)

# 5) Строим три subplot'а
fig, axes = plt.subplots(3, 1, figsize=(12, 10))

# --- (A) subplot 1: весь временной ряд (train+test) ---
axes[0].plot(df['open_time'], real_close, label="All data", color="black")
axes[0].set_title("Entire dataset (train + test)")
axes[0].set_xlabel("Time")
axes[0].set_ylabel("Price")
axes[0].legend()
axes[0].grid(True, linestyle="--", alpha=0.7)

# --- (B) subplot 2: только тест (реальные данные) ---
axes[1].plot(range(len(test_real)), test_real, label="Real Test Data", color="black")
axes[1].set_title("Test region (real data only)")
axes[1].set_xlabel("Time steps (test region)")
axes[1].set_ylabel("Price")
axes[1].legend()
axes[1].grid(True, linestyle="--", alpha=0.7)

# --- (C) subplot 3: разница (ARIMA+GARCH - ARIMA) ---
difference = combined_forecast - arima_forecast
axes[2].plot(range(len(difference)), difference, color="purple", label="(ARIMA+GARCH) - ARIMA")
axes[2].set_title("Difference between ARIMA+GARCH and ARIMA (test region)")
axes[2].set_xlabel("Time steps (test region)")
axes[2].set_ylabel("Price difference")
axes[2].legend()
axes[2].grid(True, linestyle="--", alpha=0.7)

plt.tight_layout()

# Сохраняем график в файл
plt.savefig("../ModelsMetrics/forecast_comparison.png")

plt.show()
