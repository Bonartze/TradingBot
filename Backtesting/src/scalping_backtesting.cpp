#include "../include/scalping_backtesting.h"
#include "../../Logger/include/Logger.h"
#include "../../TradingStrategies/Common/include/Common.h"
#include "../../TradingStrategies/Common/include/TradingStrategy.h"
#include "../../TradingStrategies/Scalping/include/ScalpingStr.h"
#include "../../TradingStrategies/Common/include/AdaptiveParams.h"
#include <filesystem>
#include <tuple>

constexpr double START_BALANCE = 1000.0;
constexpr size_t START_YEAR = 2020;
constexpr size_t END_YEAR = 2024;

constexpr size_t STATIC_WINDOW_SIZE = 10;
constexpr double STATIC_SMA_SHORT = 5.0;
constexpr double STATIC_SMA_LONG = 10.0;
constexpr double STATIC_RSI_OVERSOLD = 30.0;
constexpr double STATIC_RSI_OVERBUGHT = 29.0;

constexpr size_t EXPECTED_PROFIT_MARGIN = 30;
constexpr std::array<double, 5> EXPECTED_PROFIT = {40, 2, -20, 60, -80};
constexpr std::array<double, 5> EXPECTED_PROFIT_a = {-20, -20, 5, 10, -5};

namespace {
    
    void processYearDataStatic(const std::string &data_dir, ScalpingStr &scalp,
                                 double &total_profit, size_t &total_trades, CSVLogger &csv_logger) {
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
        Logger(LogLevel::INFO) << "[STATIC] Starting backtesting for directory: " << data_dir
                               << ". Total files: " << files.size();
        for (const auto &file: files) {
            Logger(LogLevel::INFO) << "[STATIC] Processing file: " << file;
            auto start_time = std::chrono::high_resolution_clock::now();
            auto candles = loadCandles(file);
            auto prices = TradingStrategy::extract_prices(candles);
            Logger(LogLevel::DEBUG) << "[STATIC] Extracted prices: " << prices.size();

            
            std::vector<double> staticParams = {
                STATIC_SMA_SHORT,
                STATIC_SMA_LONG,
                STATIC_RSI_OVERSOLD,
                STATIC_RSI_OVERBUGHT
            };
            scalp.set_parameters(staticParams);

            std::tie(total_profit, total_trades) = scalp.wrapper_execute(STATIC_WINDOW_SIZE, prices, csv_logger);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Logger(LogLevel::INFO) << "[STATIC] File Processed: " << file
                                   << " | Total Trades: " << total_trades
                                   << " | Duration: " << duration << " ms";
        }
    }

    
    void processYearDataAdaptive(const std::string &data_dir, ScalpingStr &scalp,
                                 double &total_profit, size_t &total_trades, CSVLogger &csv_logger) {
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
        Logger(LogLevel::INFO) << "[ADAPTIVE] Starting backtesting for directory: " << data_dir
                               << ". Total files: " << files.size();

        ScalpingAdaptiveParameters adaptiveParams;
        adaptiveParams.dynamic = true;

        for (const auto &file: files) {
            Logger(LogLevel::INFO) << "[ADAPTIVE] Processing file: " << file;
            auto start_time = std::chrono::high_resolution_clock::now();
            auto candles = loadCandles(file);
            auto prices = TradingStrategy::extract_prices(candles);
            Logger(LogLevel::DEBUG) << "[ADAPTIVE] Extracted prices: " << prices.size();

            adaptiveParams.updateParameters(prices);
            auto [adapt_window, adapt_sma_short, adapt_sma_long, adapt_rsi_oversold, adapt_rsi_overbought] =
                    adaptiveParams.getParameters();
            Logger(LogLevel::DEBUG) << "[ADAPTIVE] Adaptive parameters updated: "
                                     << "window_size=" << adapt_window << ", "
                                     << "sma_short=" << adapt_sma_short << ", "
                                     << "sma_long=" << adapt_sma_long << ", "
                                     << "rsi_oversold=" << adapt_rsi_oversold << ", "
                                     << "rsi_overbought=" << adapt_rsi_overbought;

            std::vector<double> newParams = {
                static_cast<double>(adapt_sma_short),
                static_cast<double>(adapt_sma_long),
                static_cast<double>(adapt_rsi_oversold),
                static_cast<double>(adapt_rsi_overbought)
            };
            scalp.set_parameters(newParams);

            std::tie(total_profit, total_trades) = scalp.wrapper_execute(adapt_window, prices, csv_logger);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Logger(LogLevel::INFO) << "[ADAPTIVE] File Processed: " << file
                                   << " | Total Trades: " << total_trades
                                   << " | Duration: " << duration << " ms";
        }
    }
}

TEST_F(ScalpingBacktesting, StaticParametersBacktesting) {
    ScalpingStr scalp({
                          STATIC_SMA_SHORT,
                          STATIC_SMA_LONG,
                          STATIC_RSI_OVERSOLD,
                          STATIC_RSI_OVERBUGHT
                      },
                      START_BALANCE, false, 0.0, 0.0, "", "", "");
    std::filesystem::create_directories("../data/scalping/static/");
    for (size_t year = START_YEAR; year <= END_YEAR; ++year) {
        std::string output_file = "../data/scalping/static/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);
        double total_profit = 0.0;
        size_t total_trades = 0;
        processYearDataStatic("../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year), scalp,
                              total_profit, total_trades, csv_logger);
        Logger(LogLevel::INFO) << "[STATIC] Year: " << year
                               << " | Expected Profit: " << EXPECTED_PROFIT[year - START_YEAR]
                               << " | Calculated Profit: " << total_profit
                               << " | Total Trades: " << total_trades;
        EXPECT_NEAR(EXPECTED_PROFIT[year - START_YEAR], total_profit, EXPECTED_PROFIT_MARGIN);
    }
}

TEST_F(ScalpingBacktesting, AdaptiveParametersBacktesting) {
    ScalpingStr scalp({
                          STATIC_SMA_SHORT,
                          STATIC_SMA_LONG,
                          STATIC_RSI_OVERSOLD,
                          STATIC_RSI_OVERBUGHT
                      },
                      START_BALANCE, false, 0.0, 0.0, "", "", "");
    std::filesystem::create_directories("../data/scalping/adaptive/");
    for (size_t year = START_YEAR; year <= END_YEAR; ++year) {
        std::string output_file = "../data/scalping/adaptive/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);
        double total_profit = 0.0;
        size_t total_trades = 0;
        processYearDataAdaptive("../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year), scalp,
                                total_profit, total_trades, csv_logger);
        Logger(LogLevel::INFO) << "[ADAPTIVE] Year: " << year
                               << " | Expected Profit: " << EXPECTED_PROFIT[year - START_YEAR]
                               << " | Calculated Profit: " << total_profit
                               << " | Total Trades: " << total_trades;
        EXPECT_NEAR(EXPECTED_PROFIT_a[year - START_YEAR], total_profit, EXPECTED_PROFIT_MARGIN);
    }
}
