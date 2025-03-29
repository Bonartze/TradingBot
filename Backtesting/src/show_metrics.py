import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


def extract_year(filename: str):
    pattern = r".*(\d{4}).*\.csv"
    match = re.match(pattern, filename)
    if match:
        return int(match.group(1))
    return None


def aggregate_yearly(input_data):
    yearly_profit = {}
    yearly_trades = {}

    if isinstance(input_data, list):
        file_list = input_data
    else:
        file_list = [os.path.join(input_data, f) for f in os.listdir(input_data) if f.endswith(".csv")]

    for file_path in file_list:
        filename = os.path.basename(file_path)
        year = extract_year(filename)
        if year is None:
            continue
        df = pd.read_csv(file_path)
        df_final = df[df["Check Type"] == "FINAL PROFIT"]
        if not df_final.empty:
            profit = pd.to_numeric(df_final["Current Price"].iloc[-1], errors="coerce")
        else:
            profit = 0
        trade_rows = df[df["Check Type"].isin(["BUY", "SELL"])]
        num_trades = len(trade_rows)
        yearly_profit[year] = yearly_profit.get(year, 0) + (profit if profit is not None else 0)
        yearly_trades[year] = yearly_trades.get(year, 0) + num_trades
    return yearly_profit, yearly_trades


def aggregate_monthly(input_data):
    """
    Считывает CSV-файлы из каталога (если input_data - строка) или из списка файлов (если input_data - list)
    и агрегирует прибыль и число сделок по месяцам.
    """
    monthly_profit = {}
    monthly_trades = {}
    pattern = r".*-(\d{4})-(\d{2})\.csv"

    if isinstance(input_data, list):
        file_list = input_data
    else:
        file_list = [os.path.join(input_data, f) for f in os.listdir(input_data) if f.endswith(".csv")]

    for file_path in file_list:
        filename = os.path.basename(file_path)
        match = re.match(pattern, filename)
        if match:
            month = int(match.group(2))
        else:
            continue
        df = pd.read_csv(file_path)
        df_final = df[df["Check Type"] == "FINAL PROFIT"]
        if not df_final.empty:
            profit = pd.to_numeric(df_final["Current Price"].iloc[-1], errors="coerce")
        else:
            profit = 0
        trade_rows = df[df["Check Type"].isin(["BUY", "SELL"])]
        num_trades = len(trade_rows)
        monthly_profit[month] = monthly_profit.get(month, 0) + (profit if profit is not None else 0)
        monthly_trades[month] = monthly_trades.get(month, 0) + num_trades
    return monthly_profit, monthly_trades


def plot_comparison(x_labels, static_vals, adaptive_vals, title, ylabel, output_path):
    """
    Строит сравнительный график: для каждого элемента x_labels две группы столбцов – статические и адаптивные значения.
    """
    x = np.arange(len(x_labels))
    width = 0.35
    plt.figure(figsize=(10, 6))
    plt.bar(x - width / 2, [static_vals.get(label, 0) for label in x_labels], width, label='Static', color='skyblue')
    plt.bar(x + width / 2, [adaptive_vals.get(label, 0) for label in x_labels], width, label='Adaptive', color='salmon')
    plt.xlabel("Period")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.xticks(x, x_labels)
    plt.legend()
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(output_path)
    plt.close()


def analyze_strategy(strategy_name: str, static_dir: str, adaptive_dir: str, output_root: str):
    """
    Для заданной стратегии агрегирует годовые показатели для статического и адаптивного тестирования,
    строит сравнительные графики и возвращает итоговые результаты.
    """
    strategy_out = os.path.join(output_root, strategy_name)
    os.makedirs(strategy_out, exist_ok=True)

    static_profit, static_trades = aggregate_yearly(static_dir)
    adaptive_profit, adaptive_trades = aggregate_yearly(adaptive_dir)

    years = sorted(set(static_profit.keys()) | set(adaptive_profit.keys()))

    plot_comparison(years, static_profit, adaptive_profit,
                    f"Total Profit by Year ({strategy_name})",
                    "Profit",
                    os.path.join(strategy_out, "profit_comparison.png"))
    plot_comparison(years, static_trades, adaptive_trades,
                    f"Number of Trades by Year ({strategy_name})",
                    "Number of Trades",
                    os.path.join(strategy_out, "trades_comparison.png"))

    return {
        "profit": {"static": static_profit, "adaptive": adaptive_profit},
        "trades": {"static": static_trades, "adaptive": adaptive_trades}
    }


def analyze_backtesting_results(strategy_files: dict, output_root: str):
    overall_results = {}
    for strategy_name, input_data in strategy_files.items():
        print(f"\n--- Processing {strategy_name} ---")

        if strategy_name == "ArimaGarchAdaptive":

            aggregated_profit, aggregated_trades = aggregate_monthly(input_data)
            x_labels = sorted(aggregated_profit.keys())
        else:
            aggregated_profit, aggregated_trades = aggregate_yearly(input_data)
            x_labels = sorted(aggregated_profit.keys())
        overall_results[strategy_name] = {"profit": aggregated_profit, "trades": aggregated_trades}
        print(f"Aggregated Profit for {strategy_name}:")
        for label, profit in sorted(aggregated_profit.items()):
            print(f"Period {label}: Profit = {profit}")
        print(f"Aggregated Trades for {strategy_name}:")
        for label, trades in sorted(aggregated_trades.items()):
            print(f"Period {label}: Trades = {trades}")

        if strategy_name != "ArimaGarchAdaptive":
            plot_comparison(x_labels, aggregated_profit, aggregated_profit,
                            f"Total Profit by Year ({strategy_name})", "Profit",
                            os.path.join(output_root, strategy_name, "profit_comparison_overall.jpg"))
            plot_comparison(x_labels, aggregated_trades, aggregated_trades,
                            f"Total Trades by Year ({strategy_name})", "Number of Trades",
                            os.path.join(output_root, strategy_name, "trades_comparison_overall.jpg"))
        else:
            plt.figure(figsize=(10, 6))
            plt.bar(x_labels, [aggregated_profit.get(label, 0) for label in x_labels], color='skyblue')
            plt.xlabel("Month")
            plt.ylabel("Profit")
            plt.title("Total Profit by Month (ArimaGarchAdaptive)")
            plt.xticks(x_labels)
            plt.grid(axis='y', linestyle='--', alpha=0.7)
            plt.tight_layout()
            plt.savefig(os.path.join(output_root, strategy_name, "profit_comparison.png"))
            plt.close()

            plt.figure(figsize=(10, 6))
            plt.bar(x_labels, [aggregated_trades.get(label, 0) for label in x_labels], color='salmon')
            plt.xlabel("Month")
            plt.ylabel("Number of Trades")
            plt.title("Total Trades by Month (ArimaGarchAdaptive)")
            plt.xticks(x_labels)
            plt.grid(axis='y', linestyle='--', alpha=0.7)
            plt.tight_layout()
            plt.savefig(os.path.join(output_root, strategy_name, "trades_comparison.png"))
            plt.close()
    return overall_results


if __name__ == "__main__":

    strategy_files = {
        "scalping": "../data/scalping/",
        "mean_reversion": "../data/mean_reversion/",
        "bayesian": "../data/bayesian/",

        "ArimaGarchAdaptive": [
            "../data/arima_garch/2020_BTCUSDT-5m-2020-01.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-02.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-03.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-04.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-05.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-06.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-07.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-08.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-09.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-10.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-11.csv",
            "../data/arima_garch/2020_BTCUSDT-5m-2020-12.csv",
        ]
    }

    output_root = "../data/backtesting_results/"
    os.makedirs(output_root, exist_ok=True)

    strategy_dirs = {
        "scalping": {
            "static": "../data/scalping/static/",
            "adaptive": "../data/scalping/adaptive/"
        },
        "mean_reversion": {
            "static": "../data/mean_reversion/static/",
            "adaptive": "../data/mean_reversion/adaptive/"
        },
        "bayesian": {
            "static": "../data/bayesian/static/",
            "adaptive": "../data/bayesian/adaptive/"
        },
        "ArimaGarchAdaptive": {
            "adaptive": "../data/arima_garch/"
        }
    }

    overall_results = {}

    for strat in ["scalping", "mean_reversion", "bayesian"]:
        print(f"\nProcessing strategy: {strat}")
        overall_results[strat] = analyze_strategy(strat, strategy_dirs[strat]["static"],
                                                  strategy_dirs[strat]["adaptive"], output_root)

    if "ArimaGarchAdaptive" in strategy_dirs:
        monthly_profit, monthly_trades = aggregate_monthly(strategy_files["ArimaGarchAdaptive"])
        overall_results["ArimaGarchAdaptive"] = {"profit": monthly_profit, "trades": monthly_trades}
        arima_out = os.path.join(output_root, "ArimaGarchAdaptive")
        os.makedirs(arima_out, exist_ok=True)
        months = sorted(monthly_profit.keys())
        plt.figure(figsize=(10, 6))
        plt.bar(months, [monthly_profit.get(m, 0) for m in months], color='skyblue')
        plt.xlabel("Month")
        plt.ylabel("Profit")
        plt.title("Total Profit by Month (ArimaGarchAdaptive)")
        plt.xticks(months)
        plt.grid(axis='y', linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(arima_out, "profit_comparison.png"))
        plt.close()

        plt.figure(figsize=(10, 6))
        plt.bar(months, [monthly_trades.get(m, 0) for m in months], color='salmon')
        plt.xlabel("Month")
        plt.ylabel("Number of Trades")
        plt.title("Total Trades by Month (ArimaGarchAdaptive)")
        plt.xticks(months)
        plt.grid(axis='y', linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(arima_out, "trades_comparison.png"))
        plt.close()

    print("Overall Results:")
    for strat, res in overall_results.items():
        print(f"Strategy: {strat}")
        print("Profit:", res["profit"])
        print("Trades:", res["trades"])
