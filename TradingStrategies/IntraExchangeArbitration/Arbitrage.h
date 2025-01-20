#ifndef TRADINGBOT_ARBITRAGE_H
#define TRADINGBOT_ARBITRAGE_H

#include "BinanceScalping.h"
#include "Bellman_Ford.h"
#include "../../TradingEngine/OrderManager.h"
#include <memory>

class Arbitrage {
private:
    std::unique_ptr<OrderManager> order_manager; // order manager
    std::unique_ptr<BinanceScalping> binance_scalping; // initial scalping
    std::unique_ptr<Graph> order_graph; // graph with possible orders for arbitrage opportunities
    void do_order_sequence(const std::vector<std::string> &, const double);

    double apply_commission(double, double);

    double calculate_potential_profit(double, double, size_t);

public:
    Arbitrage() = delete;

    Arbitrage(int) = delete;

    Arbitrage(const std::unordered_set<std::string> &, const int &version, const std::string &, const std::string &);

    void find_arbitrage_opportunities(const std::string &source, const double); // innner arbitrage opportunities


};


#endif
