#include "../include/ExchangeAPI.h"
#include <thread>
#include <iostream>
#include <limits>

std::vector<std::unique_ptr<BinanceScalping> > ExchangeAPI::apis;
std::vector<std::thread> ExchangeAPI::api_threads;
std::vector<OrderBookEntry> ExchangeAPI::orders;

void ExchangeAPI::run_interexchange_arbitrage(std::string crypto_pair) {
    apis.push_back(std::make_unique<BinanceAPI>("BTCUSDT", 11, "api.binance.com", "443", "/api/v3/ticker/bookTicker"));
    apis.push_back(std::make_unique<KrakenAPI>("XXBTZUSD", 11, "api.kraken.com", "443", "/0/public/Ticker"));
    apis.push_back(std::make_unique<OKXAPI>("BTC-USDT", 11, "www.okx.com", "443", "/api/v5/market/ticker"));

    for (auto &api: apis) {
        api_threads.emplace_back([&]() {
            api->fetch_raw_data(orders);
        });
    }

    for (auto &api_thread: api_threads) {
        if (api_thread.joinable())
            api_thread.join();
    }

    // Выбираем биржу с самой низкой ask_price (где покупать)
    double min_ask = std::numeric_limits<double>::max();
    int buy_index = -1;

    // Выбираем биржу с самой высокой bid_price (где продавать)
    double max_bid = std::numeric_limits<double>::lowest();
    int sell_index = -1;

    for (size_t i = 0; i < orders.size(); ++i) {
        if (orders[i].ask_price < min_ask) {
            min_ask = orders[i].ask_price;
            buy_index = i;
        }
        if (orders[i].bid_price > max_bid) {
            max_bid = orders[i].bid_price;
            sell_index = i;
        }
    }

    printf("\n\nnew\n\n");
    for (const auto &order: orders) {
        std::cout << "Ask Price: " << order.ask_price << std::endl;
        std::cout << "Bid Price: " << order.bid_price << std::endl;
        std::cout << "Ask Qty: " << order.ask_qty << std::endl;
        std::cout << "Bid Qty: " << order.bid_qty << std::endl;
    }

    if (buy_index != -1 && sell_index != -1 && buy_index != sell_index) {
        double spread = max_bid - min_ask;

        double buy_fee = orders[buy_index].calculate_fees(min_ask, false);
        double sell_fee = orders[sell_index].calculate_fees(max_bid, true);

        double total_fee = buy_fee + sell_fee;
        double profit_threshold = total_fee;

        if (spread > profit_threshold) {
            double trade_volume = std::min(orders[buy_index].ask_qty, orders[sell_index].bid_qty);

            double potential_profit = (spread * trade_volume) - total_fee * trade_volume;

            if (potential_profit > 0) {
                // buying and selling logic (using appropriate managers)
                std::cout << "\n===== Arbitrage Opportunity Found! =====\n";
                std::cout << "Buy from Exchange " << buy_index << " at: " << min_ask << "\n";
                std::cout << "Sell to Exchange " << sell_index << " at: " << max_bid << "\n";
                std::cout << "Trade Volume: " << trade_volume << "\n";
                std::cout << "Buy Fee: " << buy_fee << "\n";
                std::cout << "Sell Fee: " << sell_fee << "\n";
                std::cout << "Expected Profit: " << potential_profit << "\n";
            } else {
                std::cout << "\nNo profitable arbitrage opportunity found (fees too high).\n";
            }
        } else {
            std::cout << "\nNo arbitrage opportunity: Spread too low.\n";
        }
    } else {
        std::cout << "\nNo valid arbitrage opportunity found.\n";
    }
}

int main() {
    ExchangeAPI::run_interexchange_arbitrage("BTCUSDT");
}
