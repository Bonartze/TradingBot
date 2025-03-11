#include "../include/scalping_backtesting.h"
#include "../../Logger/include/Logger.h"
#include "../../TradingStrategies/Common/include/Common.h"
#include "../../TradingStrategies/Common/include/TradingStrategy.h"
#include <filesystem>

constexpr double START_BALANCE = 1000.0;
constexpr size_t START_YEAR = 2020;
constexpr size_t END_YEAR = 2024;
constexpr size_t WINDOW_SIZE = 10;
constexpr size_t SCALPING_SMA_SHORT = 5.0;
constexpr size_t SCALPING_SMA_LONG = 10.0;
constexpr size_t SCALPING_RSI_OVERSOLD = 30.0;
constexpr size_t SCALPING_RSI_OVERBOUGHT = 29.0;
constexpr size_t EXPECTED_PROFIT_MARGIN = 30;
constexpr std::array<double, 5> EXPECTED_PROFIT = {40, 2, -20, 60, -80};

void ScalpingBacktesting::SetUp() {
    // data_dir можно установить здесь, если нужно
}

void ScalpingBacktesting::TearDown() {
}

namespace {
    void processYearData(const std::string &data_dir, ScalpingStr &scalp, double &total_profit, size_t &total_trades, CSVLogger &csv_logger) {
        std::vector<std::filesystem::path> files;
        for (const auto &entry: std::filesystem::directory_iterator(data_dir)) {
            if (entry.path().extension() == ".csv") {
                files.push_back(entry.path());
            }
        }
        std::sort(files.begin(), files.end(),
                  [](const std::filesystem::path &a, const std::filesystem::path &b) {
                      return a.filename().string() < b.filename().string();
                  });
        Logger(LogLevel::INFO) << "Starting backtesting for directory: " << data_dir
                << ". Total files: " << files.size();
        for (const auto &file: files) {
            Logger(LogLevel::INFO) << "Processing file: " << file;
            auto start_time = std::chrono::high_resolution_clock::now();
            auto candles = loadCandles(file);
            auto prices = TradingStrategy::extract_prices(candles);
            std::tie(total_profit, total_trades) = scalp.wrapper_execute(WINDOW_SIZE, prices, csv_logger);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Logger(LogLevel::INFO) << "File Processed: " << file
                    << " | Total Trades: " << total_trades
                    << " | Duration: " << duration << " ms";
        }
    }
}

TEST_F(ScalpingBacktesting, ScalpingBacktesting) {
    ScalpingStr scalp({SCALPING_SMA_SHORT,
                     SCALPING_SMA_LONG,
                     SCALPING_RSI_OVERSOLD,
                     SCALPING_RSI_OVERBOUGHT},
                     START_BALANCE, false, 0.0, 0.0);
    std::filesystem::create_directories("../data/scalping/");
    for (size_t year = START_YEAR; year <= END_YEAR; ++year) {
        std::string output_file = "../data/scalping/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);
        double total_profit = 0.0;
        size_t total_trades = 0;
        processYearData("../data/Data_extracted/5m/BTCUSDT/"+std::to_string(year), scalp, total_profit, total_trades, csv_logger);
        Logger(LogLevel::INFO) << "Year: " << year
                << " | Expected Profit: " << EXPECTED_PROFIT[year - START_YEAR]
                << " | Calculated Profit: " << total_profit
                << " | Total Trades: " << total_trades;
        EXPECT_NEAR(EXPECTED_PROFIT[year - START_YEAR], total_profit, EXPECTED_PROFIT_MARGIN);
    }
}
