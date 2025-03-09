#include "../include/arima_garch_adaptive_backtesting.h"
constexpr size_t WINDOW_SIZE = 100;
constexpr size_t EXPECTED_PROFIT_MARGIN = 200;

void ArimaGarchAdaptiveBacktesting::SetUp() {
    test_file = "../data/Data_extracted/5m/BTCUSDT/2021/BTCUSDT-5m-2021-02.csv";
    arima_garch_adaptive_backtesting = std::make_unique<ArimaGarchAdaptive>(test_file);
}

void ArimaGarchAdaptiveBacktesting::TearDown() {
    arima_garch_adaptive_backtesting.reset();
}

constexpr std::array<double, 5> EXPECTED_PROFIT = {1200, -150, 200, 400, -700};

TEST_F(ArimaGarchAdaptiveBacktesting, arimaGarchMultiYearBacktesting) {
    std::filesystem::create_directories("../data/arima_garch/");

    for (size_t year = 2020; year <= 2024; ++year) {
        const std::string data_dir = "../data/Data_extracted/5m/BTCUSDT/" + std::to_string(year);
        double total_profit = 0.0;
        size_t total_trades = 0;

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

        for (const auto &file: files) {
            Logger(LogLevel::INFO) << "Processing file: " << file.string();
            auto start_time = std::chrono::high_resolution_clock::now();

            auto candles = loadCandles(file.string());
            auto prices = TradingStrategy::extract_prices(candles);

            if (prices.empty()) {
                Logger(LogLevel::WARNING) << "Warning: No prices extracted from file " << file;
                continue;
            }

            // Лог для конкретного файла
            const std::string output_file = "../data/arima_garch/" + std::to_string(year) + "_" + file.filename().
                                            string();
            CSVLogger csv_logger(output_file);

            auto [profit, trades] = arima_garch_adaptive_backtesting->wrapper_execute(WINDOW_SIZE, prices, csv_logger);
            total_profit += profit;
            total_trades += trades;

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Logger(LogLevel::INFO) << "File Processed: " << file.string()
                    << " | Trades: " << trades
                    << " | Duration: " << duration << " ms";
        }

        Logger(LogLevel::INFO) << "Year: " << year
                << " | Total Profit: " << total_profit
                << " | Total Trades: " << total_trades;

        EXPECT_NEAR(EXPECTED_PROFIT[year - 2020], total_profit, EXPECTED_PROFIT_MARGIN);
    }
}
