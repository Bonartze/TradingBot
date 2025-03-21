#include <vector>
#include <fstream>
#include "../include/ArimaModel.h"
#include "../include/GarchModel.h"
#include "../../TradingStrategies/Common/include/Common.h"
std::vector<double> split_data(const std::vector<double>& data, double train_ratio) {
    size_t train_size = static_cast<size_t>(train_ratio * data.size());
    return std::vector<double>(data.begin(), data.begin() + train_size);
}

void save_forecast_to_csv(const std::vector<double>& forecast, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    for (const auto& value : forecast) {
        file << value << "\n";
    }

    file.close();
    std::cout << "Saved forecast to " << filename << std::endl;
}


int main() {
    auto d = loadCandles("../../Backtesting/data/Data_extracted/5m/BTCUSDT/2020/BTCUSDT-5m-2020-01.csv");
    std::vector<double> full_data;

    for (auto& a : d) {
      full_data.push_back(a.close);
    }

    double train_ratio = 0.9;
    std::vector<double> train_data = split_data(full_data, train_ratio);

    ARIMAModel arimaModel(train_data);
    auto arima_forecast = arimaModel.forecast(full_data.size() - train_data.size());

    GarchModel garchModel(arimaModel);
    auto combined_forecast = garchModel.combined_forecast(full_data.size() - train_data.size());

    save_forecast_to_csv(arima_forecast, "arima_forecast.csv");
    save_forecast_to_csv(combined_forecast, "combined_forecast.csv");

    return 0;
}
