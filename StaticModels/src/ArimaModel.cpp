#include "../include/ArimaModel.h"
#include "../../TradingStrategies/Common/include/Common.h"
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <Eigen/Dense>
#include <fstream>


ARIMAModel::ARIMAModel(const std::string &data) {
    fill_data(data);
    if (close_prices.empty())
        throw std::runtime_error("Error: close_prices is empty.");

    params = arima_parameters_evaluation(close_prices);
    if (params.q > 5)
        params.q = 5;
    auto diff_close_prices = close_prices;
    for (int i = 0; i < params.d; i++) {
        diff_close_prices = compute_first_diff(diff_close_prices);
        if (diff_close_prices.empty())
            throw std::runtime_error("Error: Not enough data to compute difference.");
    }


    coefficients = estimate_coefficients(diff_close_prices, params.p, params.q);
}


std::vector<double> ARIMAModel::compute_first_diff(const std::vector<double> &series) {
    if (series.size() < 2) {
        throw std::runtime_error("Error: Series too short for differencing.");
    }
    std::vector<double> first_diff(series.size() - 1);
    for (size_t i = 1; i < series.size(); ++i) {
        first_diff[i - 1] = series[i] - series[i - 1];
    }
    return first_diff;
}


bool ARIMAModel::Dickey_Fuller_test(const std::vector<double> &series) {
    if (series.size() < 4) {
        return true;
    }
    size_t n = series.size() - 1;
    std::vector<double> first_diff = compute_first_diff(series);

    Eigen::MatrixXd X(n, 3);
    Eigen::VectorXd Y(n);
    for (size_t i = 0; i < n; ++i) {
        X(i, 0) = 1.0;
        X(i, 1) = static_cast<double>(i + 1);
        X(i, 2) = series[i];
        Y(i) = first_diff[i];
    }
    Eigen::VectorXd beta = (X.transpose() * X).fullPivLu().solve(X.transpose() * Y);

    double gamma = beta(2);


    Eigen::VectorXd eps = Y - X * beta;

    double sigma2 = (double) (eps.transpose() * eps) / (n - 3.0);


    Eigen::MatrixXd covBeta = sigma2 * (X.transpose() * X).inverse();
    double se_gamma = std::sqrt(covBeta(2, 2));

    double tau = gamma / se_gamma;


    double critical_value_5 = -3.41;
    return (tau < critical_value_5);
}


std::vector<double> ARIMAModel::PACF() {
    size_t max_lags = std::min<size_t>(close_prices.size() - 1, 20);
    std::vector<double> pacf_vec(max_lags + 1);
    for (size_t k = 0; k <= max_lags; k++) {
        pacf_vec[k] = PACF(static_cast<int>(k));
    }
    return pacf_vec;
}

double ARIMAModel::PACF(int k) {
    if (k == 0) return 1.0;
    if (k >= static_cast<int>(close_prices.size())) {
        throw std::runtime_error("k too large in PACF");
    }


    size_t n = close_prices.size();

    std::vector<double> r(k + 1, 0.0);

    double mean = std::accumulate(close_prices.begin(), close_prices.end(), 0.0) / n;
    for (int lag = 0; lag <= k; ++lag) {
        double c = 0;
        for (size_t t = lag; t < n; ++t) {
            double a = close_prices[t] - mean;
            double b = close_prices[t - lag] - mean;
            c += a * b;
        }
        r[lag] = c / (n);
    }


    Eigen::MatrixXd R(k, k);
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            R(i, j) = r[std::abs(i - j)];
        }
    }

    Eigen::VectorXd rhs(k);
    for (int i = 0; i < k; i++) {
        rhs(i) = r[i + 1];
    }
    Eigen::VectorXd a = R.fullPivLu().solve(rhs);

    return a(k - 1);
}

std::vector<double> ARIMAModel::ACF() {
    size_t n = close_prices.size();
    std::vector<double> acf(n, 0.0);
    double mean = std::accumulate(close_prices.begin(), close_prices.end(), 0.0) / n;
    double var = 0.0;
    for (auto &val: close_prices) {
        var += (val - mean) * (val - mean);
    }
    if (var < 1e-14) {
        return acf;
    }

    for (size_t lag = 0; lag < n; lag++) {
        double c = 0;
        for (size_t t = lag; t < n; t++) {
            c += (close_prices[t] - mean) * (close_prices[t - lag] - mean);
        }
        acf[lag] = c / var;
    }
    return acf;
}


ArimaParams ARIMAModel::arima_parameters_evaluation(const std::vector<double> &series) {
    int d = 0;
    std::vector<double> diff_series = series;
    while (!Dickey_Fuller_test(diff_series)) {
        diff_series = compute_first_diff(diff_series);
        d++;
        if (diff_series.size() < 2) break;
    }


    std::vector<double> pacf_vals = PACF();
    double crit = 1.96 / std::sqrt((double) series.size());
    int p = 0;
    for (size_t i = 1; i < pacf_vals.size(); i++) {
        if (std::fabs(pacf_vals[i]) > crit) {
            p = (int) i;
        } else {
            break;
        }
    }

    std::vector<double> acf_vals = ACF();
    int q = 0;
    for (size_t i = 1; i < acf_vals.size(); i++) {
        if (std::fabs(acf_vals[i]) > crit) {
            q = (int) i;
        } else {
            break;
        }
    }

   // std::cout << "[ARIMA] Automatic guess => p=" << p << " d=" << d << " q=" << q << std::endl;
    return {p, d, q};
}


Eigen::MatrixXd ARIMAModel::build_regression_matrix(const std::vector<double> &diff_series, int p, int q) {
    int maxpq = std::max(p, q);
    int N = (int) diff_series.size();
    if (N <= maxpq) {
        throw std::runtime_error("Not enough data for regression matrix");
    }
    int nrows = N - maxpq;
    Eigen::MatrixXd X(nrows, p + q + 1);
    X.setZero();
    for (int i = 0; i < nrows; i++) {
        X(i, 0) = 1.0;

        for (int j = 0; j < p; j++) {
            X(i, 1 + j) = diff_series[i + maxpq - (j + 1)];
        }
    }
    return X;
}

Eigen::VectorXd ARIMAModel::build_target_vector(const std::vector<double> &diff_series, int p, int q) {
    int maxpq = std::max(p, q);
    int N = (int) diff_series.size();
    if (N <= maxpq) {
        throw std::runtime_error("Not enough data for target vector");
    }
    int nrows = N - maxpq;
    Eigen::VectorXd Y(nrows);
    for (int i = 0; i < nrows; i++) {
        Y(i) = diff_series[i + maxpq];
    }
    return Y;
}


void ARIMAModel::update_errors(Eigen::MatrixXd &X,
                               const std::vector<double> &diff_series,
                               int p, int q,
                               const std::vector<double> &phi,
                               double intercept) {
    if (q == 0) return;
    int maxpq = std::max(p, q);
    int nrows = X.rows();
    for (int i = 0; i < nrows; i++) {
        double predicted = intercept;

        for (int j = 0; j < p; j++) {
            double val = diff_series[i + maxpq - (j + 1)];
            predicted += phi[j] * val;
        }
        double actual = diff_series[i + maxpq];
        double resid = actual - predicted;

        for (int j = 0; j < q; j++) {
            if (j == 0) {
                X(i, p + 1 + j) = (i > 0 ? resid : 0.0);
            } else {
                int idxErr = i - j;
                X(i, p + 1 + j) = (idxErr >= 0 ? 0.0 /* we could store from previous iteration's residuals */ : 0.0);
            }
        }
    }
}


ARIMACoefficients ARIMAModel::estimate_coefficients(const std::vector<double> &diff_series, int p, int q) {
    if (p < 0 || q < 0) {
        throw std::runtime_error("Invalid p or q in estimate_coefficients");
    }

    ARIMACoefficients coefs;
    coefs.phi.resize(p, 0.0);
    coefs.theta.resize(q, 0.0);
    coefs.intercept = 0.0;

    if (p == 0 && q == 0) return coefs;


    Eigen::MatrixXd X = build_regression_matrix(diff_series, p, q);
    Eigen::VectorXd Y = build_target_vector(diff_series, p, q);


    const int ITER = 5;
    for (int it = 0; it < ITER; it++) {
        update_errors(X, diff_series, p, q, coefs.phi, coefs.intercept);


        Eigen::MatrixXd A = X.transpose() * X;
        Eigen::VectorXd b = X.transpose() * Y;

        Eigen::VectorXd beta = A.fullPivHouseholderQr().solve(b);


        coefs.intercept = beta(0);
        for (int i = 0; i < p; i++) {
            coefs.phi[i] = beta(1 + i);
        }
        for (int j = 0; j < q; j++) {
            coefs.theta[j] = beta(1 + p + j);
        }
    }

    std::cout << "[ARIMA] Fitted intercept=" << coefs.intercept << "\n";
    std::cout << "[ARIMA] AR coefficients (phi):";
    for (auto &pp: coefs.phi) std::cout << " " << pp;
    std::cout << "\n[ARIMA] MA coefficients (theta):";
    for (auto &tt: coefs.theta) std::cout << " " << tt;
    std::cout << std::endl;

    return coefs;
}


std::vector<double> ARIMAModel::get_residuals() {
    int n = (int) close_prices.size();
    if (n < 1) return {};

    int p = params.p;
    int q = params.q;
    int d = params.d;


    std::vector<double> resid(n, 0.0);
    double intercept = coefficients.intercept;


    int start = std::max(p, q);
    for (int t = start; t < n; t++) {
        double predicted = intercept;


        for (int i = 0; i < p; i++) {
            predicted += coefficients.phi[i] * close_prices[t - i - 1];
        }

        for (int j = 0; j < q; j++) {
            predicted += coefficients.theta[j] * resid[t - j - 1];
        }
        resid[t] = close_prices[t] - predicted;
    }


    return std::vector<double>(resid.begin() + start, resid.end());
}


std::vector<double> ARIMAModel::forecast(int steps) {
    int p = params.p;
    int q = params.q;
    int d = params.d;
    if (close_prices.empty()) {
        throw std::runtime_error("No data in ARIMA");
    }


    double last_original = close_prices.back();


    std::vector<double> ar_buffer;
    ar_buffer.reserve(p);
    for (int i = 0; i < p; i++) {
        if ((int) close_prices.size() - 1 - i >= 0) {
            ar_buffer.push_back(close_prices[close_prices.size() - 1 - i]);
        } else {
            ar_buffer.push_back(last_original);
        }
    }


    std::vector<double> in_sample_resid = get_residuals();
    std::vector<double> ma_buffer;
    ma_buffer.reserve(q);
    for (int i = 0; i < q; i++) {
        if ((int) in_sample_resid.size() - 1 - i >= 0) {
            ma_buffer.push_back(in_sample_resid[in_sample_resid.size() - 1 - i]);
        } else {
            ma_buffer.push_back(0.0);
        }
    }


    std::vector<double> forecasts;
    forecasts.reserve(steps);

    double intercept = coefficients.intercept;
    for (int step = 0; step < steps; step++) {
        double f = intercept;

        for (int i = 0; i < p; i++) {
            f += coefficients.phi[i] * ar_buffer[i];
        }


        for (int i = p - 1; i > 0; i--) {
            ar_buffer[i] = ar_buffer[i - 1];
        }


        if (d > 0) {
            double new_value = last_original + f;

            forecasts.push_back(new_value);

            ar_buffer[0] = new_value;
            last_original = new_value;
        } else {
            forecasts.push_back(f);
            ar_buffer[0] = f;
            last_original = f;
        }
    }
    return forecasts;
}

auto ARIMAModel::fill_data(const std::string &file_path) -> void {
    auto candles = loadCandles(file_path);
    for (auto &candle: candles)
        close_prices.push_back(candle.close);
}
