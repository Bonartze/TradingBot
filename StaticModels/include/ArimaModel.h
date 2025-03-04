#pragma once
#include <iostream>
#include <vector>
#include <Eigen/Dense>

struct ArimaParams {
    int p;
    int d;
    int q;
};

class ARIMAModel {
private:
    ArimaParams params;

    std::vector<double> close_prices;

    auto compute_first_diff(const std::vector<double> &) -> std::vector<double>;

    auto PACF() -> void;

public:
    auto fill_data() -> void;

    auto Dikey_Fuller_test(const std::vector<double> &) -> bool;

    auto arima_parameters_evaluation(const std::vector<double> &) -> std::tuple<double, double, double>
};
