#pragma once
#include "../include/LiveBinanceScalping.h"
#include "../include/Bellman_Ford.h"
#include "../../../TradingEngine/include/BinanceOrderManager.h"
#include "../../Common/include/Common.h"
#include <memory>

struct TradeDirection {
    double initial_amount;
    double current_amount;
};

class Arbitrage {
    std::unique_ptr<BinanceOrderManager> order_manager; // order manager
    std::unique_ptr<LiveBinanceScalping> binance_scalping; // initial scalping
    std::unique_ptr<Graph> order_graph; // graph with possible orders for arbitrage opportunities
    void do_order_sequence(const std::vector<std::string> &, const double &) const;

    static auto apply_commission(double, double) -> double;

    static auto calculate_potential_profit(const TradeDirection &, size_t) -> double;

public:
    Arbitrage() = delete;

    Arbitrage(int) = delete;

    Arbitrage(const std::unordered_set<std::string> &, const int &version, const std::string &, const std::string &);

    void find_arbitrage_opportunities(const std::string &source, double) const; // innner arbitrage opportunities
};
