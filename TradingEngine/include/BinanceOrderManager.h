#pragma once
#include "AbstractOrderManager.h"
#include <chrono>

const std::string BINANCE_HOST = "https://api.binance.com";

class BinanceOrderManager : public AbstractOrderManager {
    uint64_t generate_timestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count();
    }

public:
    BinanceOrderManager(const std::string &key, const std::string &secret)
        : AbstractOrderManager(key, secret) {}

    void place_order(const std::string &symbol, const std::string &side,
                     const std::string &order_type, const std::string &timeInForce,
                     double quantity, double price, const std::string &newClientOrderId,
                     double stopPrice, double icebergQty, long recvWindow);
};
