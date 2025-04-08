#include "../include/ExchangeAPI.h"
#include <thread>
#include <iostream>
#include <limits>
#include "../include/BinanceAPI.h"
#include "../include/KrakenAPI.h"
#include "../include/OKXAPI.h"
constexpr int VERSION = 11;

std::vector<std::unique_ptr<BinanceScalping> > ExchangeAPI::apis;
std::vector<std::thread> ExchangeAPI::api_threads;
std::vector<OrderBookEntry> ExchangeAPI::orders;

void ExchangeAPI::run_interexchange_arbitrage([[maybe_unused]] const std::string &crypto_pair) {
    apis.push_back(std::make_unique<BinanceAPI>("BTCUSDT", VERSION, "api.binance.com", "443",
                                                "/api/v3/ticker/bookTicker"));
    apis.push_back(std::make_unique<KrakenAPI>("XXBTZUSD", VERSION, "api.kraken.com", "443", "/0/public/Ticker"));
    apis.push_back(std::make_unique<OKXAPI>("BTC-USDT", VERSION, "www.okx.com", "443", "/api/v5/market/ticker"));

    for (auto &api: apis) {
        api_threads.emplace_back([&]() {
            api->fetch_raw_data(orders);
        });
    }

    for (auto &api_thread: api_threads) {
        if (api_thread.joinable()) {
            api_thread.join();
        }
    }

    double min_ask = std::numeric_limits<double>::max();
    int buy_index = -1;


    double max_bid = std::numeric_limits<double>::lowest();
    int sell_index = -1;

    for (size_t i = 0; i < orders.size(); ++i) {
        if (orders[i].ask_price < min_ask) {
            min_ask = orders[i].ask_price;
            buy_index = static_cast<int>(i);
        }
        if (orders[i].bid_price > max_bid) {
            max_bid = orders[i].bid_price;
            sell_index = static_cast<int>(i);
        }
    }

    //printf("\n\nnew\n\n");
    // for (const auto &order: orders) {
    //     std::cout << "Ask Price: " << order.ask_price << std::endl;
    //     std::cout << "Bid Price: " << order.bid_price << std::endl;
    //     std::cout << "Ask Qty: " << order.ask_qty << std::endl;
    //     std::cout << "Bid Qty: " << order.bid_qty << std::endl;
    // }

    if (buy_index != -1 && sell_index != -1 && buy_index != sell_index) {
        const double spread = max_bid - min_ask;

        const double buy_fee = orders[buy_index].calculate_fees(min_ask, false);
        const double sell_fee = orders[sell_index].calculate_fees(max_bid, true);

        const double total_fee = buy_fee + sell_fee;
        const double profit_threshold = total_fee;

        if (spread > profit_threshold) {
            const double trade_volume = std::min(orders[buy_index].ask_qty, orders[sell_index].bid_qty);

            const double potential_profit = (spread * trade_volume) - (total_fee * trade_volume);

            if (potential_profit > 0) {
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
