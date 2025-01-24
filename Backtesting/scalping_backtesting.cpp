#include "scalping_backtesting.h"
#include "../Logger/Logger.h"


constexpr double START_BALANCE = 1000.0;
constexpr size_t START_YEAR = 2020;
constexpr size_t END_YEAR = 2024;
constexpr size_t WINDOW_SIZE = 100;
constexpr size_t SCALPING_SMA_SHORT = 5.0;
constexpr size_t SCALPING_SMA_LONG = 10.0;
constexpr size_t SCALPING_RSI_OVERSOLD = 30.0;
constexpr size_t SCALPING_RSI_OVERBOUGHT = 29.0;
constexpr size_t EXPECTED_PROFIT_MARGIN = 100.0;

constexpr std::array<double, 5> EXPECTED_PROFIT = {700, 200, -600, 300, 200};

void Backtesting::SetUp() {
    data_dir = "/home/ivan/CLionProjects/TradingBot/Backtesting/Data_extracted/5m/BTCUSDT/";
}

void Backtesting::TearDown() {
}

namespace {
    void processYearData(const std::string &data_dir, ScalpingStr &scalp, double &total_profit, size_t &total_trades,
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
            auto candles = ScalpingStr::loadCandles(file);
            auto prices = ScalpingStr::extract_prices(candles);

            for (size_t i = WINDOW_SIZE; i <= prices.size(); i += WINDOW_SIZE) {
                const std::vector<double> price_segment(prices.begin(), prices.begin() + i);
                total_profit += scalp.execute(price_segment, {
                                                  SCALPING_SMA_SHORT, SCALPING_SMA_LONG, SCALPING_RSI_OVERSOLD,
                                                  SCALPING_RSI_OVERBOUGHT
                                              }, csv_logger);
                total_trades++;
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            Logger(LogLevel::INFO) << "File Processed: " << file
                    << " | Total Trades: " << total_trades
                    << " | Duration: " << duration << " ms";
        }
    }
}

TEST_F(Backtesting, ScalpingBacktesting) {
    ScalpingStr scalp(START_BALANCE, false, 0.0, 0.0);

    for (size_t year = START_YEAR; year <= END_YEAR; ++year) {
        const std::string year_dir = data_dir + std::to_string(year) + "/";
        std::string output_file = "/home/ivan/scalping_metrics_" + std::to_string(year) + ".csv";
        CSVLogger csv_logger(output_file);

        double total_profit = 0.0;
        size_t total_trades = 0;

        processYearData(year_dir, scalp, total_profit, total_trades, csv_logger);

        Logger(LogLevel::INFO) << "Year: " << year
                << " | Expected Profit: " << EXPECTED_PROFIT[year - START_YEAR]
                << " | Calculated Profit: " << total_profit
                << " | Total Trades: " << total_trades;
        EXPECT_NEAR(EXPECTED_PROFIT[year - START_YEAR], total_profit, EXPECTED_PROFIT_MARGIN);
    }
}
