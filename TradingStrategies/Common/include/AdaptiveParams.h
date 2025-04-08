#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <tuple>
#include "../../../StaticModels/Common/include/MathStatistics.h"

static constexpr double kStandardThreshold = 0.3;
static constexpr double kStandardRsiValue = 30.0;
static constexpr double kCloseRsiValue = 29.0;
static constexpr double kHighRsiValue = 40.0;
static constexpr double kSmallDelta = 0.05;
static constexpr double kMediumDelta = 1.0;
static constexpr double kStandardMultiplier = 5.0;
static constexpr double kReducedMultiplier = 3.0;
static constexpr double kSmallAdjustment = 0.1;
static constexpr double kMediumAdjustment = 0.2;
static constexpr size_t kWindowSizeAdjustment = 50;

static constexpr size_t kShortWindow = 5;
static constexpr size_t kMediumWindow = 10;
static constexpr size_t kLongWindow = 100;

static constexpr double kHighVolatilityThreshold = 1.0;
static constexpr double kMediumVolatilityThreshold = 0.5;

class AdaptiveParameters {
protected:
    bool dynamic = false; // NOLINT

public:
    virtual ~AdaptiveParameters() = default;

    AdaptiveParameters() = default;

    AdaptiveParameters(const AdaptiveParameters &) = default;

    auto operator=(const AdaptiveParameters &) -> AdaptiveParameters & = default;

    AdaptiveParameters(AdaptiveParameters &&) = default;

    auto operator=(AdaptiveParameters &&) -> AdaptiveParameters & = default;

    virtual void updateParameters(const std::vector<double> &recentPrices) = 0;
};

class BayesianAdaptiveParameters : public AdaptiveParameters {
public:
    BayesianAdaptiveParameters() = default;

    void updateParameters(const std::vector<double> &recentPrices) override {
        if (!dynamic) {
            return;
        }

        const double vol = computeVolatility(recentPrices);
        const double factor = std::tanh(vol);
        buyThreshold = base_buyThreshold - delta * kStandardMultiplier * factor;
        sellThreshold = base_sellThreshold + delta * kStandardMultiplier * factor;
        rsiThreshold = base_rsiThreshold + delta_rsi * kStandardMultiplier * factor;
    }

    [[nodiscard]] auto getParameters() const -> std::tuple<double, double, double> {
        return {buyThreshold, sellThreshold, rsiThreshold};
    }

private:
    double base_buyThreshold = kStandardThreshold;
    double base_sellThreshold = kStandardThreshold;
    double base_rsiThreshold = kHighRsiValue;
    double delta = kSmallDelta;
    double delta_rsi = kMediumDelta;

    double buyThreshold = base_buyThreshold;
    double sellThreshold = base_sellThreshold;
    double rsiThreshold = base_rsiThreshold;

    static auto computeVolatility(const std::vector<double> &prices) -> double {
        return prices.size() < 2 ? 0.0 : std::sqrt(MathStatistics::variance(prices));
    }
};

class MeanReversalAdaptiveParameters : public AdaptiveParameters {
public:
    MeanReversalAdaptiveParameters() = default;

    void updateParameters(const std::vector<double> &recentPrices) override {
        if (!dynamic) {
            return;
        }

        const double vol = computeVolatility(recentPrices);
        const double factor = std::tanh(std::max(0.0, vol - kHighVolatilityThreshold));

        window_size = base_window_size + static_cast<size_t>(kWindowSizeAdjustment * factor);
        sma_short = base_sma_short + static_cast<size_t>(
                        static_cast<double>(base_sma_short) * kMediumAdjustment * factor);
        sma_long = base_sma_long + static_cast<size_t>(static_cast<double>(base_sma_long) * kMediumAdjustment * factor);
        rsi_oversold = static_cast<size_t>(static_cast<double>(base_rsi_oversold) - (kStandardMultiplier * factor));
        rsi_overbought = base_rsi_overbought + (kStandardMultiplier * factor);
    }

    [[nodiscard]] auto getParameters() const -> std::tuple<size_t, size_t, size_t, size_t, double> {
        return {window_size, sma_short, sma_long, rsi_oversold, rsi_overbought};
    }

private:
    size_t base_window_size = kLongWindow;
    size_t base_sma_short = kShortWindow;
    size_t base_sma_long = kMediumWindow;
    size_t base_rsi_oversold = static_cast<size_t>(kStandardRsiValue);
    double base_rsi_overbought = kCloseRsiValue;

    size_t window_size = base_window_size;
    size_t sma_short = base_sma_short;
    size_t sma_long = base_sma_long;
    size_t rsi_oversold = base_rsi_oversold;
    double rsi_overbought = base_rsi_overbought;

    static auto computeVolatility(const std::vector<double> &prices) -> double {
        return prices.size() < 2 ? 0.0 : std::sqrt(MathStatistics::variance(prices));
    }
};

class ScalpingAdaptiveParameters : public AdaptiveParameters {
public:
    ScalpingAdaptiveParameters() = default;

    void updateParameters(const std::vector<double> &recentPrices) override {
        if (!dynamic) {
            return;
        }

        const double vol = computeVolatility(recentPrices);
        const double factor = std::tanh(std::max(0.0, vol - kMediumVolatilityThreshold));

        window_size = static_cast<size_t>(static_cast<double>(base_window_size) - (
                                              static_cast<double>(base_window_size) * kMediumAdjustment * factor));
        sma_short = base_sma_short - (base_sma_short * kSmallAdjustment * factor);
        sma_long = base_sma_long - (base_sma_long * kSmallAdjustment * factor);
        rsi_oversold = base_rsi_oversold - (kReducedMultiplier * factor);
        rsi_overbought = base_rsi_overbought + (kReducedMultiplier * factor);
    }

    [[nodiscard]] auto getParameters() const -> std::tuple<size_t, double, double, double, double> {
        return {window_size, sma_short, sma_long, rsi_oversold, rsi_overbought};
    }

private:
    size_t base_window_size = kMediumWindow;
    double base_sma_short = static_cast<double>(kShortWindow);
    double base_sma_long = static_cast<double>(kMediumWindow);
    double base_rsi_oversold = kStandardRsiValue;
    double base_rsi_overbought = kCloseRsiValue;

    size_t window_size = base_window_size;
    double sma_short = base_sma_short;
    double sma_long = base_sma_long;
    double rsi_oversold = base_rsi_oversold;
    double rsi_overbought = base_rsi_overbought;

    static auto computeVolatility(const std::vector<double> &prices) -> double {
        return prices.size() < 2 ? 0.0 : std::sqrt(MathStatistics::variance(prices));
    }
};
