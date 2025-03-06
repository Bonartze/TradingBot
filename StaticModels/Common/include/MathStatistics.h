#pragma once
#include <vector>
#include <numeric>
#include <stdexcept>
#include <random>

class MathStatistics {
public:
    template<typename T>
    static double mean(const std::vector<T> &values) {
        if (values.empty()) {
            throw std::invalid_argument("Mean requires at least one data point.");
        }
        return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    }

    template<typename T>
    static double variance(const std::vector<T> &values) {
        if (values.size() <= 1) {
            throw std::invalid_argument("Variance requires at least two data points.");
        }

        double mu = mean(values);
        double var = std::accumulate(values.begin(), values.end(), 0.0,
                                     [mu](double acc, T el) {
                                         return acc + (el - mu) * (el - mu);
                                     });
        return var / (values.size() - 1);
    }

    static double sample_standart_normal() {
        static std::mt19937 generator(std::random_device{}());
        std::normal_distribution<double> distribution(0.0, 1.0);
        return distribution(generator);
    }
};
