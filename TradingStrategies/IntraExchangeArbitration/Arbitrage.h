#ifndef TRADINGBOT_ARBITRAGE_H
#define TRADINGBOT_ARBITRAGE_H

#include "BinanceScalping.h"
#include "Bellman_Ford.h"
#include "../../TradingEngine/OrderManager.h"

class Arbitrage {
private:
    OrderManager order_manager; // order manager
    BinanceScalping binance_scalping; // initial scalping
    Graph order_graph; // graph with possible orders for arbitrage opportunities
    void do_order_sequence(const std::vector<std::string> &, const double);

    double apply_commission(double, double);

    double calculate_potential_profit(double, double, size_t);

public:
    Arbitrage() = delete; // чтобы ебланы не делали херню ??

    Arbitrage(int) = delete; // сэйм, что и выше ??

    Arbitrage(const std::unordered_set<std::string> &, const int &version, const std::string &, const std::string &);

    void find_arbitrage_opportunities(const std::string &source, const double); // innner arbitrage opportunities


};


#endif
