#pragma once
#include "AbstractOrderManager.h"
#include <chrono>

class OKXOrderManager : public AbstractOrderManager {
    std::string passphrase;

    std::string generate_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() %
                            1000;

        std::ostringstream oss;
        oss << std::put_time(gmtime(&seconds), "%Y-%m-%dT%H:%M:%S") << "."
                << std::setfill('0') << std::setw(3) << milliseconds << "Z";
        return oss.str();
    }

public:
    OKXOrderManager(const std::string &key, const std::string &secret, const std::string &phrase)
        : AbstractOrderManager(key, secret), passphrase(phrase) {
    }

    void place_order(const std::string &order_type, const std::string &side, double volume, const std::string &inst_id);
};
