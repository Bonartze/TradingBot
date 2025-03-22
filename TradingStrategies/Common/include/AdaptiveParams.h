#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <tuple>
#include "../../../StaticModels/Common/include/MathStatistics.h"

class AdaptiveParameters {
public:
    virtual ~AdaptiveParameters() = default;

    virtual void updateParameters(const std::vector<double> &recentPrices) = 0;

    bool dynamic = false;
};


class BayesianAdaptiveParameters : public AdaptiveParameters {
public:
    double base_buyThreshold = 0.3;
    double base_sellThreshold = 0.3;
    double base_rsiThreshold = 40.0;

    double delta = 0.05;
    double delta_rsi = 1.0;


    double buyThreshold = base_buyThreshold;
    double sellThreshold = base_sellThreshold;
    double rsiThreshold = base_rsiThreshold;

    void updateParameters(const std::vector<double> &recentPrices) override {
        if (!dynamic) return;
        double vol = computeVolatility(recentPrices);

        double factor = std::tanh(vol);
        buyThreshold = base_buyThreshold - delta * 5 * factor;
        sellThreshold = base_sellThreshold + delta * 5 * factor;

        rsiThreshold = base_rsiThreshold + delta_rsi * 5 * factor;
    }


    std::tuple<double, double, double> getParameters() const {
        return std::make_tuple(buyThreshold, sellThreshold, rsiThreshold);
    }

private:
    double computeVolatility(const std::vector<double> &prices) {
        if (prices.size() < 2) return 0.0;
        return std::sqrt(MathStatistics::variance(prices));
    }
};


class MeanReversalAdaptiveParameters : public AdaptiveParameters {
public:
    size_t base_window_size = 100;
    size_t base_sma_short = 5;
    size_t base_sma_long = 10;
    size_t base_rsi_oversold = 30;
    double base_rsi_overbought = 29.0;

    size_t window_size = base_window_size;
    size_t sma_short = base_sma_short;
    size_t sma_long = base_sma_long;
    size_t rsi_oversold = base_rsi_oversold;
    double rsi_overbought = base_rsi_overbought;

    void updateParameters(const std::vector<double> &recentPrices) override {
        if (!dynamic) return;
        double vol = computeVolatility(recentPrices);
        double factor = std::tanh(std::max(0.0, vol - 1.0));
        window_size = base_window_size + static_cast<size_t>(50 * factor);
        sma_short = base_sma_short + static_cast<size_t>(base_sma_short * 0.2 * factor);
        sma_long = base_sma_long + static_cast<size_t>(base_sma_long * 0.2 * factor);
        rsi_oversold = static_cast<size_t>(base_rsi_oversold - 5 * factor);
        rsi_overbought = base_rsi_overbought + 5 * factor;
    }


    std::tuple<size_t, size_t, size_t, size_t, double> getParameters() const {
        return std::make_tuple(window_size, sma_short, sma_long, rsi_oversold, rsi_overbought);
    }

private:
    double computeVolatility(const std::vector<double> &prices) {
        if (prices.size() < 2) return 0.0;
        return std::sqrt(MathStatistics::variance(prices));
    }
};


class ScalpingAdaptiveParameters : public AdaptiveParameters {
public:
    size_t base_window_size = 10;
    double base_sma_short = 5.0;
    double base_sma_long = 10.0;
    double base_rsi_oversold = 30.0;
    double base_rsi_overbought = 29.0;

    size_t window_size = base_window_size;
    double sma_short = base_sma_short;
    double sma_long = base_sma_long;
    double rsi_oversold = base_rsi_oversold;
    double rsi_overbought = base_rsi_overbought;

    void updateParameters(const std::vector<double> &recentPrices) override {
        if (!dynamic)
            return;
        double vol = computeVolatility(recentPrices);
        double factor = std::tanh(std::max(0.0, vol - 0.5));
        window_size = static_cast<size_t>(base_window_size - base_window_size * 0.2 * factor);
        sma_short = base_sma_short - base_sma_short * 0.1 * factor;
        sma_long = base_sma_long - base_sma_long * 0.1 * factor;
        rsi_oversold = base_rsi_oversold - 3 * factor;
        rsi_overbought = base_rsi_overbought + 3 * factor;
    }


    std::tuple<size_t, double, double, double, double> getParameters() const {
        return std::make_tuple(window_size, sma_short, sma_long, rsi_oversold, rsi_overbought);
    }

private:
    double computeVolatility(const std::vector<double> &prices) {
        if (prices.size() < 2)
            return 0.0;
        return std::sqrt(MathStatistics::variance(prices));
    }
};
