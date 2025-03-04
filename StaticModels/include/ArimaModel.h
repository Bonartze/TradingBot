#pragma once
#include <iostream>
#include <vector>
#include <Eigen/Dense>

struct ArimaParams {
    int p;
    int d;
    int q;
};

struct ARIMACoefficients {
    std::vector<double> phi;
    std::vector<double> theta;
};


class ARIMAModel {
private:
    ArimaParams params;
    ARIMACoefficients coefficients;
    std::vector<double> close_prices;

    auto compute_first_diff(const std::vector<double> &) -> std::vector<double>;

    auto PACF(int) -> double;

    auto PACF() -> std::vector<double>;

    auto ACF() -> std::vector<double>;

    auto build_regression_matrix(const std::vector<double> &, int, int) -> Eigen::MatrixXd;

    auto build_target_vector(const std::vector<double> &diff_series, int p) -> Eigen::VectorXd;

    auto estimate_coefficients(const std::vector<double> &diff_series, int p, int q) -> ARIMACoefficients;

    auto update_errors(Eigen::MatrixXd &, const std::vector<double> &, int, int, const std::vector<double> &) -> void;

public:
    ARIMAModel();

   // auto fill_data() -> void;

    auto forecast(int steps) -> std::vector<double>;

    auto Dickey_Fuller_test(const std::vector<double> &) -> bool;

    auto arima_parameters_evaluation(const std::vector<double> &) -> ArimaParams;
};
