#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #ifdef STRATEGIES_API_EXPORT
        #define STRATEGIES_API __declspec(dllexport)
    #else
        #define STRATEGIES_API __declspec(dllimport)
    #endif
#elif defined(__linux__) || defined(__unix__)
    #define STRATEGIES_API __attribute__((visibility("default")))
#else
    #define STRATEGIES_API
#endif

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include "../../TradingEngine/include/OrderManager.h"
#include "../IntraExchangeArbitration/include/LiveBinanceScalping.h"
#include "../Common/include/Graph.h"
#include "../Common/include/Common.h"
#include "../../Logger/include/CSVLogger.h"


class STRATEGIES_API Arbitrage {
private:
    std::unique_ptr<OrderManager> order_manager;
    std::unique_ptr<LiveBinanceScalping> binance_scalping;
    std::unique_ptr<Graph> order_graph;
    void do_order_sequence(const std::vector<std::string> &, const double);

    double apply_commission(double, double);

    double calculate_potential_profit(double, double, size_t);

public:
    Arbitrage() = delete;

    Arbitrage(int) = delete;

    Arbitrage(const std::unordered_set<std::string> &, const int &version, const std::string &, const std::string &);

    void find_arbitrage_opportunities(const std::string &source, const double);
};

class STRATEGIES_API ScalpingStr {
private:
    double balance;
    bool position_open;
    double entry_price;
    double asset_quantity;


    static auto should_buy(const std::vector<double> &, const TradingParams &, CSVLogger &) -> bool;

    static auto should_sell(const std::vector<double> &, const TradingParams &, double, CSVLogger &) -> bool;

public:
    ScalpingStr();

    ScalpingStr(double balance_, bool position_opened_, double entry_price_,
                double asset_quantity_);

    auto execute() -> void;

    static auto extract_prices(const std::vector<Candle> &) -> std::vector<double>;

    static auto loadCandles(const std::string &) -> std::vector<Candle>;

    auto execute(const std::vector<double> &, TradingParams, CSVLogger &) -> double;
};


