#include "../include/Arbitrage.h"
#include <chrono>
#include <iostream>
#include <json/json.h>

double Arbitrage::apply_commission(double amount, double commission_rate) {
    return amount * (1 - commission_rate);
}

double Arbitrage::calculate_potential_profit(double initial_amount, double current_amount, size_t i) {
    double profit = current_amount - initial_amount;
    return (i % 2 == 1) ? profit : -profit;
}

Arbitrage::Arbitrage(const std::unordered_set<std::string> &user_symbols, const int &version,
                     const std::string &api_key, const std::string &secret_key) {
    try {
        
        
        binance_scalping = std::make_unique<LiveBinanceScalping>(version, host, port, target);
        binance_scalping->fetch_raw_data();
        order_graph = std::make_unique<Graph>(binance_scalping->generate_order_graph(user_symbols));
        
        
    } catch (const std::exception &e) {
        Logger(LogLevel::ERROR) << "Error during Arbitrage initialization: " << e.what();
        throw;
    }
}

void Arbitrage::do_order_sequence(const std::vector<std::string> &cycle, double amount) {
    double current_amount = amount;
    double commission_rate = 0.001;
    double potential_profit = 0.0;

    
    auto send_order_wrapper = [&](const std::string &from_currency, const std::string &to_currency,
                                  double order_quantity, const std::string &order_type, const std::string &order_id,
                                  Json::Value &json_result) {
        
        std::string symbol = from_currency + to_currency;
        double price = binance_scalping->get_price(symbol);
        if (order_manager) {
            order_manager->place_order(symbol, order_type, "LIMIT", "GTC",
                                         order_quantity, price, order_id, 0.0, 0.0, 5000);
        } else {
            Logger(LogLevel::ERROR) << "Order manager not initialized.";
        }
    };

    
    for (size_t i = 1; i < cycle.size(); i++) {
        std::string from_currency = cycle[i - 1];
        std::string to_currency = cycle[i];
        try {
            double price = binance_scalping->get_price(from_currency + to_currency);
            if (price <= 0) {
                throw std::runtime_error("Invalid price encountered for " + from_currency + " to " + to_currency);
            }
            
            current_amount = apply_commission(current_amount, commission_rate);
            
            current_amount = (i % 2 == 1) ? current_amount * price : current_amount / price;
            
            
            potential_profit += calculate_potential_profit(current_amount, current_amount, i);
        } catch (const std::exception &e) {
            Logger(LogLevel::ERROR) << "Error during order execution: " << e.what();
            throw;
        }
    }

    
    if (potential_profit < 0)
        return;

    Json::Value json_result;
    
    for (size_t i = 1; i < cycle.size(); i++) {
        auto now = std::chrono::system_clock::now();
        auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        std::string order_id = std::to_string(milliseconds_since_epoch);
        std::string from_currency = cycle[i - 1];
        std::string to_currency = cycle[i];
        
        std::string order_type = (i % 2 == 1) ? "SELL" : "BUY";
        send_order_wrapper(from_currency, to_currency, current_amount, order_type, order_id, json_result);
    }

    Logger(LogLevel::INFO) << "Potential profit: " << potential_profit;
    Logger(LogLevel::INFO) << "Final amount after all orders: " << current_amount;
}

void Arbitrage::find_arbitrage_opportunities(const std::string &source, const double amount) {
    if (!order_graph) {
        throw std::runtime_error("Order graph is not initialized. Cannot find arbitrage opportunities.");
    }

    try {
        std::unique_ptr<Bellman_Ford> bellmanFord = std::make_unique<Bellman_Ford>(*order_graph);
        std::vector<std::string> cycle;

        if (bellmanFord->find_negative_cycle(source, cycle)) {
            if (cycle.empty()) {
                throw std::runtime_error("Negative cycle found, but it is empty.");
            }

            for (size_t i = 0; i < cycle.size(); ++i) {
                std::cout << cycle[i];
                if (i < cycle.size() - 1) {
                    std::cout << " -> ";
                }
            }
            Logger(LogLevel::INFO) << "IntraExchangeArbitration opportunity found.";
            do_order_sequence(cycle, amount);
        } else {
            Logger(LogLevel::INFO) << "IntraExchangeArbitration opportunity isn't found.";
        }
    } catch (const std::exception &e) {
        Logger(LogLevel::ERROR) << "Error during arbitrage search: " << e.what();
    }
}