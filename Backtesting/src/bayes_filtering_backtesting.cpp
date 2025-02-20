#include "../include/bayes_filtering_backtesting.h"
#include "../../Logger/include/Logger.h"
#include "../../TradingStrategies/Common/include/Common.h"
#include "../../TradingStrategies/Common/include/TradingStrategy.h"
#include <filesystem>

constexpr size_t WINDOW_SIZE = 100;
constexpr size_t START_YEAR = 2020;
constexpr size_t END_YEAR = 2024;
constexpr size_t EXPECTED_PROFIT_MARGIN = 900.0;
constexpr std::array<double, 5> EXPECTED_PROFIT = {700, 200, -600, 300, 200};

void Backtesting::SetUp() {
    //data_dir = "/home/ivan/CLionProjects/TradingBot/Backtesting/data/Data_extracted/5m/BTCUSDT/";
}


void Backtesting::TearDown() {
}

namespace {
    void processYearData(const std::string &data_dir, BayesianSignalFiltering &bayesian, double &total_profit,
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
            std::tie(total_profit, total_trades) = bayesian.wrapper_execute(WINDOW_SIZE, prices, csv_logger);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            Logger(LogLevel::INFO) << "File Processed: " << file
                    << " | Total Trades: " << total_trades
                    << " | Duration: " << duration << " ms";
        }
    }
}

TEST_F(Backtesting, bayesianingBacktesting) {
    BayesianSignalFiltering bayesian = BayesianSignalFiltering();
    std::filesystem::create_directories("../data/bayesian/");
    for (size_t year = START_YEAR; year <= END_YEAR; ++year) {
        const std::string output_file = "../data/bayesian/" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);

        double total_profit = 0.0;
        size_t total_trades = 0;

        processYearData("../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year), bayesian, total_profit,
                        total_trades,
                        csv_logger);

        Logger(LogLevel::INFO) << "Year: " << year
                << " | Expected Profit: " << EXPECTED_PROFIT[year - START_YEAR]
                << " | Calculated Profit: " << total_profit
                << " | Total Trades: " << total_trades;

        EXPECT_NEAR(EXPECTED_PROFIT[year - START_YEAR], total_profit, EXPECTED_PROFIT_MARGIN);
    }
}
