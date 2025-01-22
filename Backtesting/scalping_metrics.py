import pandas as pd
import matplotlib.pyplot as plt
import os

def calculate_profit(df):
    profit = []
    last_buy_price = None

    for _, row in df.iterrows():
        if row["Check Type"] == "BUY":
            last_buy_price = row["Current Price"]
            profit.append(0)  # При покупке прибыли нет
        elif row["Check Type"] == "SELL" and last_buy_price is not None:
            current_profit = row["Current Price"] - last_buy_price
            profit.append(current_profit)
            last_buy_price = None  # Сбрасываем цену после продажи
        else:
            profit.append(0)  # Для HOLD или BUY_CHECK записываем 0

    df["Profit"] = profit
    return df

def calculate_metrics(df):
    # Накопленная прибыль
    df["Cumulative Profit"] = df["Profit"].cumsum()

    # Максимальная просадка
    cumulative_max = df["Cumulative Profit"].cummax()
    drawdown = cumulative_max - df["Cumulative Profit"]
    max_drawdown = drawdown.max()

    # Количество сделок
    total_trades = len(df[df["Check Type"].isin(["BUY", "SELL"])])

    # Средняя прибыль на сделку
    total_profit = df["Profit"].sum()
    avg_profit_per_trade = total_profit / total_trades if total_trades > 0 else 0

    # Стандартное отклонение прибыли
    profit_std = df["Profit"].std()

    # Шарп-коэффициент (предполагая безрисковую ставку = 0)
    sharpe_ratio = total_profit / profit_std if profit_std > 0 else 0

    metrics = {
        "Total Profit": total_profit,
        "Total Trades": total_trades,
        "Average Profit per Trade": avg_profit_per_trade,
        "Max Drawdown": max_drawdown,
        "Sharpe Ratio": sharpe_ratio
    }

    return metrics

def plot_metrics_over_time(df, metrics, year, output_folder):
    # Создаем папку, если её нет
    os.makedirs(output_folder, exist_ok=True)

    # График накопленной прибыли
    plt.figure(figsize=(10, 6))
    plt.plot(df.index, df["Cumulative Profit"], label="Cumulative Profit")
    plt.title(f"Cumulative Profit Over Time ({year})")
    plt.xlabel("Time")
    plt.ylabel("Profit")
    plt.grid()
    plt.legend()
    plt.savefig(os.path.join(output_folder, f"cumulative_profit_{year}.jpg"))
    plt.close()

    # График прибыли на сделку
    plt.figure(figsize=(10, 6))
    plt.bar(df.index, df["Profit"], label="Profit per Trade")
    plt.title(f"Profit per Trade ({year})")
    plt.xlabel("Time")
    plt.ylabel("Profit")
    plt.grid()
    plt.legend()
    plt.savefig(os.path.join(output_folder, f"profit_per_trade_{year}.jpg"))
    plt.close()

    # Выводим метрики
    print(f"Metrics for {year}:")
    for key, value in metrics.items():
        print(f"{key}: {value}")

def analyze_backtesting_results(input_files, output_folder):
    overall_metrics = []
    for file in input_files:
        year = file.split("_")[-1].split(".")[0]  # Извлекаем год из имени файла
        print(f"Processing file: {file}")
        df = pd.read_csv(file)

        # Преобразуем необходимые столбцы в числовой формат
        df["Current Price"] = pd.to_numeric(df["Current Price"], errors="coerce")
        df = calculate_profit(df)

        # Рассчитываем метрики
        metrics = calculate_metrics(df)

        # Строим графики и сохраняем их как .jpg
        plot_metrics_over_time(df, metrics, year, output_folder)

        # Сохраняем метрики для общего вывода
        metrics["Year"] = year
        overall_metrics.append(metrics)

    # Общие метрики за все годы
    overall_df = pd.DataFrame(overall_metrics)
    print("\nOverall Metrics:")
    print(overall_df)

    # Итоговый график общей прибыли за все годы
    plt.figure(figsize=(10, 6))
    plt.bar(overall_df["Year"], overall_df["Total Profit"], label="Total Profit")
    plt.title("Total Profit by Year")
    plt.xlabel("Year")
    plt.ylabel("Profit")
    plt.grid()
    plt.legend()
    plt.savefig(os.path.join(output_folder, "total_profit_by_year.jpg"))
    plt.close()

# Список файлов для анализа
input_files = [
    "/home/ivan/scalping_metrics_2020.csv",
    "/home/ivan/scalping_metrics_2021.csv",
    "/home/ivan/scalping_metrics_2022.csv",
    "/home/ivan/scalping_metrics_2023.csv",
    "/home/ivan/scalping_metrics_2024.csv"
]

# Папка для сохранения графиков
output_folder = "./scalping_results"

# Запускаем анализ
analyze_backtesting_results(input_files, output_folder)
