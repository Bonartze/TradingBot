#include "../include/mean_reverse_backtesting.h"
#include "../../Logger/include/Logger.h"
#include "../../TradingStrategies/Common/include/Common.h"
#include "../../TradingStrategies/Common/include/TradingStrategy.h"
#include "../../TradingStrategies/Common/include/AdaptiveParams.h" 
#include <filesystem>
#include <tuple>

constexpr double MEAN_REV_START_BALANCE = 1000.0;
constexpr size_t MEAN_REV_START_YEAR = 2020;
constexpr size_t MEAN_REV_END_YEAR = 2024;
constexpr size_t MEAN_REV_WINDOW_SIZE = 100;
constexpr size_t MEAN_REV_SMA_SHORT = 5;
constexpr size_t MEAN_REV_SMA_LONG = 10;
constexpr size_t MEAN_REV_RSI_OVERSOLD = 30;
constexpr double MEAN_REV_RSI_OVERBOUGHT = 29;
constexpr size_t MEAN_REV_EXPECTED_PROFIT_MARGIN = 2000;
constexpr std::array<double, 5> MEAN_REV_EXPECTED_PROFIT = {400, -100, 20, 100, 400};
constexpr std::array<double, 5> MEAN_REV_EXPECTED_PROFIT_a = {750, -10, 200, -600, 600};

namespace {
    
    void processYearDataStatic(const std::string &data_dir, MeanReverseStrategy &mvs,
                                 double &total_profit, size_t &total_trades, CSVLogger &csv_logger) {
        std::vector<std::filesystem::path> files;
        for (const auto &entry : std::filesystem::directory_iterator(data_dir)) {
            if (entry.path().extension() == ".csv")
                files.push_back(entry.path());
        }
        std::sort(files.begin(), files.end(),
                  [](const std::filesystem::path &a, const std::filesystem::path &b) {
                      return a.filename().string() < b.filename().string();
                  });
        Logger(LogLevel::INFO) << "[STATIC] Starting backtesting for directory: " << data_dir
                               << ". Total files: " << files.size();
        for (const auto &file : files) {
            Logger(LogLevel::INFO) << "[STATIC] Processing file: " << file;
            auto start_time = std::chrono::high_resolution_clock::now();
            auto candles = loadCandles(file);
            auto prices = TradingStrategy::extract_prices(candles);
            Logger(LogLevel::DEBUG) << "[STATIC] Extracted prices: " << prices.size();

            
            std::vector<double> staticParams = {
                static_cast<double>(MEAN_REV_WINDOW_SIZE),
                static_cast<double>(MEAN_REV_SMA_SHORT),
                static_cast<double>(MEAN_REV_SMA_LONG),
                static_cast<double>(MEAN_REV_RSI_OVERSOLD),
                MEAN_REV_RSI_OVERBOUGHT
            };
            mvs.set_parameters(staticParams);

            std::tie(total_profit, total_trades) = mvs.wrapper_execute(MEAN_REV_WINDOW_SIZE, prices, csv_logger);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Logger(LogLevel::INFO) << "[STATIC] File Processed: " << file
                                   << " | Total Trades: " << total_trades
                                   << " | Duration: " << duration << " ms";
        }
    }

    
    void processYearDataAdaptive(const std::string &data_dir, MeanReverseStrategy &mvs,
                                 double &total_profit, size_t &total_trades, CSVLogger &csv_logger) {
        std::vector<std::filesystem::path> files;
        for (const auto &entry : std::filesystem::directory_iterator(data_dir)) {
            if (entry.path().extension() == ".csv")
                files.push_back(entry.path());
        }
        std::sort(files.begin(), files.end(),
                  [](const std::filesystem::path &a, const std::filesystem::path &b) {
                      return a.filename().string() < b.filename().string();
                  });
        Logger(LogLevel::INFO) << "[ADAPTIVE] Starting backtesting for directory: " << data_dir
                               << ". Total files: " << files.size();

        
        MeanReversalAdaptiveParameters adaptiveParams;
        adaptiveParams.dynamic = true;

        for (const auto &file : files) {
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
                static_cast<double>(adapt_window),
                static_cast<double>(adapt_sma_short),
                static_cast<double>(adapt_sma_long),
                static_cast<double>(adapt_rsi_oversold),
                adapt_rsi_overbought
            };
            mvs.set_parameters(newParams);

            std::tie(total_profit, total_trades) = mvs.wrapper_execute(adapt_window, prices, csv_logger);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Logger(LogLevel::INFO) << "[ADAPTIVE] File Processed: " << file
                                   << " | Total Trades: " << total_trades
                                   << " | Duration: " << duration << " ms";
        }
    }
}

TEST_F(MeanReverseBacktesting, StaticParametersBacktesting) {
    MeanReverseStrategy mvs;
    std::filesystem::create_directories("../data/mean_reversion/static/");
    for (size_t year = MEAN_REV_START_YEAR; year <= MEAN_REV_END_YEAR; ++year) {
        std::string output_file = "../data/mean_reversion/static/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);

        double total_profit = 0.0;
        size_t total_trades = 0;

        processYearDataStatic("../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year), mvs,
                              total_profit, total_trades, csv_logger);

        Logger(LogLevel::INFO) << "[STATIC] Year: " << year
                               << " | Expected Profit: " << MEAN_REV_EXPECTED_PROFIT[year - MEAN_REV_START_YEAR]
                               << " | Calculated Profit: " << total_profit
                               << " | Total Trades: " << total_trades;
        EXPECT_NEAR(MEAN_REV_EXPECTED_PROFIT[year - MEAN_REV_START_YEAR], total_profit, MEAN_REV_EXPECTED_PROFIT_MARGIN);
    }
}

TEST_F(MeanReverseBacktesting, AdaptiveParametersBacktesting) {
    MeanReverseStrategy mvs;
    std::filesystem::create_directories("../data/mean_reversion/adaptive/");
    for (size_t year = MEAN_REV_START_YEAR; year <= MEAN_REV_END_YEAR; ++year) {
        std::string output_file = "../data/mean_reversion/adaptive/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);

        double total_profit = 0.0;
        size_t total_trades = 0;

        processYearDataAdaptive("../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year), mvs,
                                total_profit, total_trades, csv_logger);

        Logger(LogLevel::INFO) << "[ADAPTIVE] Year: " << year
                               << " | Expected Profit: " << MEAN_REV_EXPECTED_PROFIT[year - MEAN_REV_START_YEAR]
                               << " | Calculated Profit: " << total_profit
                               << " | Total Trades: " << total_trades;
        EXPECT_NEAR(MEAN_REV_EXPECTED_PROFIT_a[year - MEAN_REV_START_YEAR], total_profit, MEAN_REV_EXPECTED_PROFIT_MARGIN);

    }
}
