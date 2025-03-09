#pragma once
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
    double intercept;
};

class ARIMAModel {
public:
    explicit ARIMAModel(const std::string &);

    std::vector<double> forecast(int steps);


    std::vector<double> get_residuals();


    ArimaParams get_params() const { return params; }
    ARIMACoefficients get_coeffs() const { return coefficients; }
    std::vector<double> close_prices;

private:
    ArimaParams params{};
    ARIMACoefficients coefficients{};

    auto fill_data(const std::string &) -> void;

    static std::vector<double> compute_first_diff(const std::vector<double> &series);


    static bool Dickey_Fuller_test(const std::vector<double> &series);


    std::vector<double> PACF();

    double PACF(int k);

    std::vector<double> ACF();


    ArimaParams arima_parameters_evaluation(const std::vector<double> &series);


    static Eigen::MatrixXd build_regression_matrix(const std::vector<double> &diff_series, int p, int q);

    static Eigen::VectorXd build_target_vector(const std::vector<double> &diff_series, int p, int q);

    static void update_errors(Eigen::MatrixXd &X,
                              const std::vector<double> &diff_series,
                              int p, int q,
                              const std::vector<double> &phi,
                              double intercept);


    ARIMACoefficients estimate_coefficients(const std::vector<double> &diff_series, int p, int q);
};
