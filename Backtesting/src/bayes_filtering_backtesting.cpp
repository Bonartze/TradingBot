#include "../include/bayes_filtering_backtesting.h"
#include "../../Logger/include/Logger.h"
#include "../../TradingStrategies/Common/include/Common.h"
#include "../../TradingStrategies/Common/include/TradingStrategy.h"
#include "../../TradingStrategies/Common/include/AdaptiveParams.h"
#include <filesystem>
#include <tuple>

constexpr size_t WINDOW_SIZE = 100;
constexpr size_t START_YEAR = 2020;
constexpr size_t END_YEAR = 2024;
constexpr size_t EXPECTED_PROFIT_MARGIN = 1000;
constexpr std::array<double, 5> EXPECTED_PROFIT = {1100, -300, -50, 300, -400};
constexpr std::array<double, 5> EXPECTED_PROFIT_a = {10, 5, 0, 0, 0};


namespace {
    void processYearDataStatic(const std::string &data_dir, BayesianSignalFiltering &bayesian,
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
        Logger(LogLevel::INFO) << "Starting backtesting for directory: " << data_dir
                << ". Total files: " << files.size();
        for (const auto &file: files) {
            Logger(LogLevel::INFO) << "Processing file: " << file;
            auto start_time = std::chrono::high_resolution_clock::now();
            auto candles = loadCandles(file);
            auto prices = TradingStrategy::extract_prices(candles);
            std::tie(total_profit, total_trades) = bayesian.wrapper_execute(WINDOW_SIZE, prices, csv_logger);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Logger(LogLevel::INFO) << "File Processed: " << file
                    << " | Total Trades: " << total_trades
                    << " | Duration: " << duration << " ms";
        }
    }

    void processYearDataAdaptive(const std::string &data_dir, BayesianSignalFiltering &bayesian,
                                 double &total_profit, size_t &total_trades, CSVLogger &csv_logger) {
        std::vector<std::filesystem::path> files;
        for (const auto &entry: std::filesystem::directory_iterator(data_dir)) {
            if (entry.path().extension() == ".csv")
                files.push_back(entry.path());
        }
        std::sort(files.begin(), files.end(),
                  [](const std::filesystem::path &a, const std::filesystem::path &b) {
                      return a.filename().string() < b.filename().string();
                  });
        Logger(LogLevel::INFO) << "[ADAPTIVE] Starting backtesting for directory: " << data_dir
                << ". Total files: " << files.size();

        BayesianAdaptiveParameters adaptiveParams;
        adaptiveParams.dynamic = true;

        for (const auto &file: files) {
            Logger(LogLevel::INFO) << "[ADAPTIVE] Processing file: " << file;
            auto start_time = std::chrono::high_resolution_clock::now();
            auto candles = loadCandles(file);
            auto prices = TradingStrategy::extract_prices(candles);
            Logger(LogLevel::DEBUG) << "[ADAPTIVE] Extracted prices: " << prices.size();

            adaptiveParams.updateParameters(prices);
            auto [adapt_buyThreshold, adapt_sellThreshold, adapt_rsiThreshold] = adaptiveParams.getParameters();
            Logger(LogLevel::DEBUG) << "[ADAPTIVE] Adaptive parameters updated: "
                    << "buyThreshold=" << adapt_buyThreshold << ", "
                    << "sellThreshold=" << adapt_sellThreshold << ", "
                    << "rsiThreshold=" << adapt_rsiThreshold;

            std::vector<double> newParams = {adapt_buyThreshold, adapt_sellThreshold, adapt_rsiThreshold};
            bayesian.set_parameters(newParams);

            std::tie(total_profit, total_trades) = bayesian.wrapper_execute(WINDOW_SIZE, prices, csv_logger);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Logger(LogLevel::INFO) << "[ADAPTIVE] File Processed: " << file
                    << " | Total Trades: " << total_trades
                    << " | Duration: " << duration << " ms";
        }
    }
}

TEST_F(BayesianBacktesting, StaticParametersBacktesting) {
    BayesianSignalFiltering bayesian;
    std::filesystem::create_directories("../data/bayesian/static/");
    for (size_t year = START_YEAR; year <= END_YEAR; ++year) {
        const std::string output_file = "../data/bayesian/static/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);
        double total_profit = 0.0;
        size_t total_trades = 0;
        processYearDataStatic("../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year), bayesian, total_profit,
                              total_trades, csv_logger);
        Logger(LogLevel::INFO) << "[STATIC] Year: " << year
                << " | Expected Profit: " << EXPECTED_PROFIT[year - START_YEAR]
                << " | Calculated Profit: " << total_profit
                << " | Total Trades: " << total_trades;
        EXPECT_NEAR(EXPECTED_PROFIT[year - START_YEAR], total_profit, EXPECTED_PROFIT_MARGIN);
    }
}

TEST_F(BayesianBacktesting, AdaptiveParametersBacktesting) {
    BayesianSignalFiltering bayesian;
    std::filesystem::create_directories("../data/bayesian/adaptive/");
    for (size_t year = START_YEAR; year <= END_YEAR; ++year) {
        const std::string output_file = "../data/bayesian/adaptive/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);
        double total_profit = 0.0;
        size_t total_trades = 0;
        processYearDataAdaptive("../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year), bayesian, total_profit,
                                total_trades, csv_logger);
        Logger(LogLevel::INFO) << "[ADAPTIVE] Year: " << year
                << " | Expected Profit: " << EXPECTED_PROFIT[year - START_YEAR]
                << " | Calculated Profit: " << total_profit
                << " | Total Trades: " << total_trades;
        EXPECT_NEAR(EXPECTED_PROFIT_a[year - START_YEAR], total_profit, EXPECTED_PROFIT_MARGIN);

        
    }
}
