#pragma once
#include "AbstractOrderManager.h"
#include <chrono>

class KrakenOrderManager : public AbstractOrderManager {
    uint64_t generate_nonce() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

public:
    KrakenOrderManager(const std::string &key, const std::string &secret)
        : AbstractOrderManager(key, secret) {
    }

    void place_order(const std::string &order_type, const std::string &side, double volume, const std::string &pair);

};
