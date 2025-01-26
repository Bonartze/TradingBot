import pandas as pd
import matplotlib.pyplot as plt
import os


def calculate_profit(df):
    profit = []
    last_buy_price = None

    for _, row in df.iterrows():
        if row["Check Type"] == "BUY":
            last_buy_price = row["Current Price"]
            profit.append(0)
        elif row["Check Type"] == "SELL" and last_buy_price is not None:
            current_profit = row["Current Price"] - last_buy_price
            profit.append(current_profit)
            last_buy_price = None
        else:
            profit.append(0)

    df["Profit"] = profit
    return df


def calculate_metrics(df):
    df["Cumulative Profit"] = df["Profit"].cumsum()

    cumulative_max = df["Cumulative Profit"].cummax()
    drawdown = cumulative_max - df["Cumulative Profit"]
    max_drawdown = drawdown.max()

    total_trades = len(df[df["Check Type"].isin(["BUY", "SELL"])])

    total_profit = df["Profit"].sum()
    avg_profit_per_trade = total_profit / total_trades if total_trades > 0 else 0

    profit_std = df["Profit"].std()

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
    os.makedirs(output_folder, exist_ok=True)

    plt.figure(figsize=(10, 6))
    plt.plot(df.index, df["Cumulative Profit"], label="Cumulative Profit")
    plt.title(f"Cumulative Profit Over Time ({year})")
    plt.xlabel("Time")
    plt.ylabel("Profit")
    plt.grid()
    plt.legend()
    plt.savefig(os.path.join(output_folder, f"cumulative_profit_{year}.jpg"))
    plt.close()

    plt.figure(figsize=(10, 6))
    plt.bar(df.index, df["Profit"], label="Profit per Trade")
    plt.title(f"Profit per Trade ({year})")
    plt.xlabel("Time")
    plt.ylabel("Profit")
    plt.grid()
    plt.legend()
    plt.savefig(os.path.join(output_folder, f"profit_per_trade_{year}.jpg"))
    plt.close()

    print(f"Metrics for {year}:")
    for key, value in metrics.items():
        print(f"{key}: {value}")


def analyze_backtesting_results(input_files, output_folder):
    overall_metrics = []
    for file in input_files:
        year = file.split("/")[-1].split(".")[0]
        print(f"Processing file: {file}")
        df = pd.read_csv(file)

        df["Current Price"] = pd.to_numeric(df["Current Price"], errors="coerce")
        df = calculate_profit(df)

        metrics = calculate_metrics(df)

        plot_metrics_over_time(df, metrics, year, output_folder)

        metrics["Year"] = year
        overall_metrics.append(metrics)

    overall_df = pd.DataFrame(overall_metrics)
    print("\nOverall Metrics:")
    print(overall_df)

    plt.figure(figsize=(10, 6))
    plt.bar(overall_df["Year"], overall_df["Total Profit"], label="Total Profit")
    plt.title("Total Profit by Year")
    plt.xlabel("Year")
    plt.ylabel("Profit")
    plt.grid()
    plt.legend()
    plt.savefig(os.path.join(output_folder, "total_profit_by_year.jpg"))
    plt.close()


input_files = [
    "../data/scalping/2020.csv",
    "../data/scalping/2021.csv",
    "../data/scalping/2022.csv",
    "../data/scalping/2023.csv",
    "../data/scalping/2024.csv"
]

output_folder = "../data/Scalping_results/"

analyze_backtesting_results(input_files, output_folder)
