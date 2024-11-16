#ifndef TRADINGBOT_ARBITRAGE_H
#define TRADINGBOT_ARBITRAGE_H

#include "BinanceScalping.h"
#include "Bellman_Ford.h"

class Arbitrage {
private:
    BinanceScalping binance_scalping;
    Graph order_graph;
public:
    Arbitrage() = delete;

    Arbitrage(int) = delete;

    Arbitrage(const std::unordered_set<std::string> &, const int &version);

    void find_arbitrage_opportunities(const std::string &source);

};


#endif
