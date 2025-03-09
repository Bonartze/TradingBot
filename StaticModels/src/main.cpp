#include "../include/ArimaModel.h"
#include "../include/GarchModel.h"
#include <iostream>
#include <random>
#include <cmath>
#include <vector>


static double sample_standard_normal() {
    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::normal_distribution<> dist(0.0, 1.0);
    return dist(gen);
}

std::vector<double> generateDummyData(int n) {
    std::vector<double> data;
    data.reserve(n);
    for (int i = 0; i < n; ++i) {
        double noise = sample_standard_normal();
        data.push_back(
            100.0 + 10.0 * std::sin(2 * M_PI * i / 50.0) + noise
        );
    }
    return data;
}

int main() {
    try {
        ARIMAModel arimaModel("../../Backtesting/data/Data_extracted/5m/BTCUSDT/2020/BTCUSDT-5m-2020-01.csv");


        GarchModel garchModel(arimaModel);

        int forecastSteps = 10;
        auto combinedForecast = garchModel.combined_forecast(forecastSteps);

        // for (auto d: dummyData) {
        //     std::cout << d << ' ';
        // }
        // std::cout << std::endl;

        for (size_t i = 0; i < forecastSteps; i++)
            std::cout << arimaModel.close_prices[arimaModel.close_prices.size() - 1 - i] << ' ';
        std::cout << std::endl;

        std::cout << "Combined Forecast:\n";
        for (int i = 0; i < forecastSteps; ++i) {
            std::cout << "Step " << (i + 1)
                    << ": " << combinedForecast[i] << std::endl;
        }
    } catch (const std::exception &ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
