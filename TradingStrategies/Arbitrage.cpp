#include "Arbitrage.h"

Arbitrage::Arbitrage(const std::unordered_set<std::string> &user_symbols, const int &version) : binance_scalping(
        version) {
    binance_scalping.fetch_raw_data();
    order_graph = binance_scalping.generate_order_graph(user_symbols);
}

void Arbitrage::find_arbitrage_opportunities(const std::string &source) {
    Bellman_Ford bellmanFord(order_graph);
    std::vector<std::string> cycle;
    if (bellmanFord.find_negative_cycle(source, cycle)) {
        std::cout << "Here\n";
        for (auto &currency: cycle) {
            std::cout << currency << " -> ";
        }
    } else
        std::cout << "Not here\n";
}

int main() {
    Arbitrage arbitrage({"TON", "ADA", "LTC", "ATOM", "DOT", "BTC", "ETH", "BNB", "USDT", "SOL", "AVAX", "XRP", "UNI"},
                        11);
    arbitrage.find_arbitrage_opportunities("BTC");
    return 0;
}

/*
 * Попытаться нормально спроктировать, но так вроде работает
 *
 * */