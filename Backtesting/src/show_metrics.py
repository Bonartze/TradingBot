import pandas as pd
import matplotlib.pyplot as plt
import os

def analyze_backtesting_results(strategy_files, output_root):
    overall_results = {}

    for strategy_name, input_files in strategy_files.items():
        print(f"\n--- Processing {strategy_name} ---")
        yearly_profits = []
        strategy_output_folder = os.path.join(output_root, strategy_name)
        os.makedirs(strategy_output_folder, exist_ok=True)

        for file_path in input_files:
            filename = os.path.basename(file_path)  # например, "2021.csv"
            year = os.path.splitext(filename)[0]    # "2021"
            print(f"Processing file: {file_path}")

            df = pd.read_csv(file_path)
            # Фильтруем строки с FINAL PROFIT
            df_final = df[df["Check Type"] == "FINAL PROFIT"]

            if not df_final.empty:
                # Берём последнее значение профита и преобразуем его к числовому типу
                total_profit_for_year = pd.to_numeric(df_final["Current Price"].iloc[-1], errors="coerce")
            else:
                total_profit_for_year = 0

            yearly_profits.append((year, total_profit_for_year))

        df_results = pd.DataFrame(yearly_profits, columns=["Year", "Total Profit"])
        overall_results[strategy_name] = df_results

        print(f"\nOverall results for {strategy_name}:")
        print(df_results)

        # Построение графика
        plt.figure(figsize=(10, 6))
        plt.bar(df_results["Year"], df_results["Total Profit"], label="Total Profit")
        plt.title(f"Total Profit by Year ({strategy_name})")
        plt.xlabel("Year")
        plt.ylabel("Profit")
        plt.grid(True)
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(strategy_output_folder, "total_profit_by_year.jpg"))
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
