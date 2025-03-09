#include <vector>
#include <memory>
#include "ArimaModel.h"

struct GarchParams {
    double omega;
    double alpha;
    double beta;
};

class GarchModel {
private:
    std::unique_ptr<ARIMAModel> arima_model;
    GarchParams params{0.1, 0.05, 0.90};

    void naive_grid_search(const std::vector<double> &residuals);

public:
    explicit GarchModel(const ARIMAModel &arima);


    void fit_garch_parameters(const std::vector<double> &residuals);


    double log_likelihood(const std::vector<double> &residuals,
                          double omega, double alpha, double beta);


    double last_sigma2(const std::vector<double> &residuals);


    std::vector<double> forecast(const std::vector<double> &residuals, int steps);


    std::vector<double> combined_forecast(int steps);
};
