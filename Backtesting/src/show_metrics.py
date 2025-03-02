import pandas as pd
import matplotlib.pyplot as plt
import os

print("Current working directory:", os.getcwd())

def analyze_backtesting_results(strategy_files, output_root):
    overall_results = {}

    for strategy_name, input_files in strategy_files.items():
        print(f"\n--- Processing {strategy_name} ---")
        yearly_profits = []
        yearly_trade_counts = []
        yearly_avg_trade_prices = []

        strategy_output_folder = os.path.join(output_root, strategy_name)
        os.makedirs(strategy_output_folder, exist_ok=True)

        for file_path in input_files:
            filename = os.path.basename(file_path)
            year = os.path.splitext(filename)[0]
            print(f"Processing file: {file_path}")

            df = pd.read_csv(file_path)

            df_final = df[df["Check Type"] == "FINAL PROFIT"]
            if not df_final.empty:
                total_profit = pd.to_numeric(df_final["Current Price"].iloc[-1], errors="coerce")
            else:
                total_profit = 0

            trade_rows = df[df["Check Type"].isin(["BUY", "SELL"])]
            num_trades = len(trade_rows)
            if num_trades > 0:
                avg_trade_price = pd.to_numeric(trade_rows["Current Price"], errors="coerce").mean()
            else:
                avg_trade_price = 0

            yearly_profits.append((year, total_profit))
            yearly_trade_counts.append((year, num_trades))
            yearly_avg_trade_prices.append((year, avg_trade_price))

        df_profit = pd.DataFrame(yearly_profits, columns=["Year", "Total Profit"])
        df_trades = pd.DataFrame(yearly_trade_counts, columns=["Year", "Number of Trades"])
        df_avg_price = pd.DataFrame(yearly_avg_trade_prices, columns=["Year", "Average Trade Price"])

        overall_results[strategy_name] = {
            "profit": df_profit,
            "trades": df_trades,
            "avg_price": df_avg_price
        }

        print(f"\nOverall results for {strategy_name}:")
        print(df_profit)
        print(df_trades)
        print(df_avg_price)

        plt.figure(figsize=(10, 6))
        plt.bar(df_profit["Year"], df_profit["Total Profit"], label="Total Profit", color='skyblue')
        plt.title(f"Total Profit by Year ({strategy_name})")
        plt.xlabel("Year")
        plt.ylabel("Profit")
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(strategy_output_folder, "total_profit_by_year.jpg"))
        plt.close()

        plt.figure(figsize=(10, 6))
        plt.plot(df_trades["Year"], df_trades["Number of Trades"], marker='o', linestyle='-', color='green', label="Number of Trades")
        plt.title(f"Number of Trades by Year ({strategy_name})")
        plt.xlabel("Year")
        plt.ylabel("Trades")
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(strategy_output_folder, "number_of_trades_by_year.jpg"))
        plt.close()

        # График средней цены торгов
        plt.figure(figsize=(10, 6))
        plt.plot(df_avg_price["Year"], df_avg_price["Average Trade Price"], marker='o', linestyle='-', color='orange', label="Average Trade Price")
        plt.title(f"Average Trade Price by Year ({strategy_name})")
        plt.xlabel("Year")
        plt.ylabel("Average Trade Price")
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(strategy_output_folder, "average_trade_price_by_year.jpg"))
        plt.close()

    return overall_results


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
    ]
}

output_root = "../data/backtesting_results/"
all_results = analyze_backtesting_results(strategy_files, output_root)
