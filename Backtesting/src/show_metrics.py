import os
import re
import pandas as pd
import matplotlib.pyplot as plt


def extract_year(filename: str):
    pattern = r".*(\d{4}).*\.csv"
    match = re.match(pattern, filename)
    if match:
        return int(match.group(1))
    return None


def extract_month(filename: str):
    pattern = r".*-(\d{4})-(\d{2})\.csv"
    match = re.match(pattern, filename)
    if match:
        return int(match.group(2))
    return None


def aggregate_yearly(strategy_files: list):
    yearly_profit = {}
    yearly_trades = {}

    for file_path in strategy_files:
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


def aggregate_monthly(strategy_files: list):
    monthly_profit = {}
    monthly_trades = {}

    for file_path in strategy_files:
        filename = os.path.basename(file_path)
        month = extract_month(filename)
        if month is None:
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


def plot_bar(data: dict, title: str, xlabel: str, ylabel: str, output_path: str, kind="bar"):
    keys = sorted(data.keys())
    values = [data[k] for k in keys]

    plt.figure(figsize=(10, 6))
    if kind == "bar":
        plt.bar(keys, values, color='skyblue')
    else:
        plt.plot(keys, values, marker='o', linestyle='-', color='green')
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.xticks(keys)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(output_path)


def analyze_backtesting_results(strategy_files: dict, output_root: str):
    os.makedirs(output_root, exist_ok=True)
    overall_results = {}

    for strategy_name, input_files in strategy_files.items():
        print(f"\n--- Processing {strategy_name} ---")
        strategy_output_folder = os.path.join(output_root, strategy_name)
        os.makedirs(strategy_output_folder, exist_ok=True)

        if strategy_name != "ArimaGarchAdaptive":
            yearly_profit, yearly_trades = aggregate_yearly(input_files)
            overall_results[strategy_name] = {"profit": yearly_profit, "trades": yearly_trades}

            print(f"Yearly Profit for {strategy_name}:")
            for year, profit in sorted(yearly_profit.items()):
                print(f"Year {year}: Profit = {profit}")
            print(f"\nYearly Trades for {strategy_name}:")
            for year, trades in sorted(yearly_trades.items()):
                print(f"Year {year}: Trades = {trades}")

            plot_bar(yearly_profit, f"Total Profit by Year ({strategy_name})", "Year", "Profit",
                     os.path.join(strategy_output_folder, "total_profit_by_year.jpg"), kind="bar")
            plot_bar(yearly_trades, f"Number of Trades by Year ({strategy_name})", "Year", "Trades",
                     os.path.join(strategy_output_folder, "number_of_trades_by_year.jpg"), kind="line")
        else:
            monthly_profit, monthly_trades = aggregate_monthly(input_files)
            overall_results[strategy_name] = {"profit": monthly_profit, "trades": monthly_trades}

            print(f"Monthly Profit for {strategy_name}:")
            for month, profit in sorted(monthly_profit.items()):
                print(f"Month {month}: Profit = {profit}")
            print(f"\nMonthly Trades for {strategy_name}:")
            for month, trades in sorted(monthly_trades.items()):
                print(f"Month {month}: Trades = {trades}")

            plot_bar(monthly_profit, f"Total Profit by Month (2020) ({strategy_name})", "Month", "Profit",
                     os.path.join(strategy_output_folder, "total_profit_by_month.jpg"), kind="bar")
            plot_bar(monthly_trades, f"Number of Trades by Month (2020) ({strategy_name})", "Month", "Trades",
                     os.path.join(strategy_output_folder, "number_of_trades_by_month.jpg"), kind="line")

    return overall_results


if __name__ == "__main__":
    strategy_files = {
        "scalping": [
            "../data/scalping/2020.csv",
            "../data/scalping/2021.csv",
            "../data/scalping/2022.csv",
            "../data/scalping/2023.csv",
            "../data/scalping/2024.csv"
        ],
        "mean_reversion": [
            "../data/mean_reversion/2020.csv",
            "../data/mean_reversion/2021.csv",
            "../data/mean_reversion/2022.csv",
            "../data/mean_reversion/2023.csv",
            "../data/mean_reversion/2024.csv"
        ],
        "bayesian": [
            "../data/bayesian/2020.csv",
            "../data/bayesian/2021.csv",
            "../data/bayesian/2022.csv",
            "../data/bayesian/2023.csv",
            "../data/bayesian/2024.csv"
        ],
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
    overall_results = analyze_backtesting_results(strategy_files, output_root)
