#include "../include/mean_reverse_backtesting.h"
#include "../../Logger/include/Logger.h"
#include "../../TradingStrategies/Common/include/Common.h"
#include "../../TradingStrategies/Common/include/TradingStrategy.h"
#include <filesystem>

constexpr double MEAN_REV_START_BALANCE = 1000.0;
constexpr size_t MEAN_REV_START_YEAR = 2020;
constexpr size_t MEAN_REV_END_YEAR = 2024;
constexpr size_t MEAN_REV_WINDOW_SIZE = 100;
constexpr size_t MEAN_REV_SMA_SHORT = 5;
constexpr size_t MEAN_REV_SMA_LONG = 10;
constexpr size_t MEAN_REV_RSI_OVERSOLD = 30;
constexpr double MEAN_REV_RSI_OVERBOUGHT = 29;
constexpr size_t MEAN_REV_EXPECTED_PROFIT_MARGIN = 200.0;

constexpr std::array<double, 5> MEAN_REV_EXPECTED_PROFIT = {400, -100, 20, 100, 400};

void MeanReverseBacktesting::SetUp() {
}

void MeanReverseBacktesting::TearDown() {
}

namespace {
    void processYearData(const std::string &data_dir, MeanReverseStrategy &mvs, double &total_profit,
                         size_t &total_trades,
                         CSVLogger &csv_logger) {
        std::vector<std::filesystem::path> files;

        for (const auto &entry: std::filesystem::directory_iterator(data_dir)) {
            if (entry.path().extension() == ".csv") {
                files.push_back(entry.path());
            }
        }

        std::sort(files.begin(), files.end(),
                  [](const std::filesystem::path &file_a, const std::filesystem::path &file_b) {
                      return file_a.filename().string() < file_b.filename().string();
                  });

        Logger(LogLevel::INFO) << "Starting backtesting for directory: " << data_dir
                << ". Total files: " << files.size();

        for (const auto &file: files) {
            Logger(LogLevel::INFO) << "Processing file: " << file;

            auto start_time = std::chrono::high_resolution_clock::now();
            auto candles = loadCandles(file);
            auto prices = TradingStrategy::extract_prices(candles);
            std::tie(total_profit, total_trades) = mvs.wrapper_execute(MEAN_REV_WINDOW_SIZE, prices, csv_logger);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            Logger(LogLevel::INFO) << "File Processed: " << file
                    << " | Total Trades: " << total_trades
                    << " | Duration: " << duration << " ms";
        }
    }
}

TEST_F(MeanReverseBacktesting, MeanReversionBacktesting) {
    MeanReverseStrategy mvs;
    std::filesystem::create_directories("../data/mean_reversion/");
    for (size_t year = MEAN_REV_START_YEAR; year <= MEAN_REV_END_YEAR; ++year) {
        std::string output_file = "../data/mean_reversion/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);

        double total_profit = 0.0;
        size_t total_trades = 0;

        processYearData("../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year), mvs, total_profit, total_trades,
                        csv_logger);

        Logger(LogLevel::INFO) << "Year: " << year
                << " | Expected Profit: " << MEAN_REV_EXPECTED_PROFIT[year - MEAN_REV_START_YEAR]
                << " | Calculated Profit: " << total_profit
                << " | Total Trades: " << total_trades;

        EXPECT_NEAR(MEAN_REV_EXPECTED_PROFIT[year - MEAN_REV_START_YEAR], total_profit,
                    MEAN_REV_EXPECTED_PROFIT_MARGIN);
    }
}
