#include "../include/arima_garch_adaptive_backtesting.h"
#include <filesystem>
#include <vector>
#include <chrono>
#include <tuple>
#include <map>
#include <regex>
#include <array>
#include <sstream>

constexpr size_t WINDOW_SIZE = 100;
constexpr size_t EXPECTED_PROFIT_MARGIN = 50;
constexpr std::array<double, 12> EXPECTED_PROFIT = {-12, -35, 0, -54, -53, -34, 122, 48, -6, 85, 277, 552};

void ArimaGarchAdaptiveBacktesting::SetUp() {
    test_file = "../data/Data_extracted/5m/BTCUSDT/2020/BTCUSDT-5m-2020-01.csv";
    arima_garch_adaptive_backtesting = std::make_unique<ArimaGarchAdaptive>(test_file);
}

void ArimaGarchAdaptiveBacktesting::TearDown() {
    arima_garch_adaptive_backtesting.reset();
}

int extractMonth(const std::string &filename) {
    std::regex re(".*-(\\d{4})-(\\d{2})\\.csv");
    std::smatch match;
    if (std::regex_match(filename, match, re)) {
        return std::stoi(match[2]);
    }
    return 0;
}

TEST_F(ArimaGarchAdaptiveBacktesting, arimaGarch2020Backtesting) {
    std::filesystem::create_directories("../data/arima_garch/");

    const size_t year = 2020;
    const std::string data_dir = "../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year);
    double total_profit = 0.0;
    size_t total_trades = 0;

    std::map<int, double> monthly_profit;
    std::map<int, size_t> monthly_trades;

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

    Logger(LogLevel::INFO) << "Year: " << year << " | Total files: " << files.size();
    int i = 0;
    for (const auto &file: files) {
        Logger(LogLevel::INFO) << "Processing file: " << file.string();
        auto start_time = std::chrono::high_resolution_clock::now();

        auto candles = loadCandles(file.string());
        auto prices = TradingStrategy::extract_prices(candles);
        if (prices.empty()) {
            Logger(LogLevel::WARNING) << "Warning: No prices extracted from file " << file;
            continue;
        }

        const std::string output_file = "../data/arima_garch/" + std::to_string(year) + "_" + file.filename().string();
        CSVLogger csv_logger(output_file);

        auto [profit, trades] = arima_garch_adaptive_backtesting->wrapper_execute(WINDOW_SIZE, prices, csv_logger);
        total_profit += profit;
        total_trades += trades;

        int month = i++;
        if (month > 0) {
            monthly_profit[month] += profit;
            monthly_trades[month] += trades;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        Logger(LogLevel::INFO) << "File Processed: " << file.string()
                << " | Trades: " << trades
                << " | Duration: " << duration << " ms";
        Logger(LogLevel::INFO) << monthly_profit[month];
    }

    Logger(LogLevel::INFO) << "Year: " << year
            << " | Total Profit: " << total_profit
            << " | Total Trades: " << total_trades;

    std::stringstream ss;
    ss << "Monthly Profit:\n";
    for (const auto &pair: monthly_profit) {
        int month = pair.first;
        double profit = pair.second;
        size_t trades = monthly_trades[month];
        ss << "Month " << month << ": Profit = " << profit << ", Trades = " << trades << "\n";
        EXPECT_NEAR(EXPECTED_PROFIT[month], profit, EXPECTED_PROFIT_MARGIN);
    }
    Logger(LogLevel::INFO) << ss.str();
}
