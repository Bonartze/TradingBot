#include "../include/DataCollector.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <ctime>

using json = nlohmann::json;

namespace {
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
}


std::vector<double> DataCollector::get_close_prices() {
    auto candles = collectData(200);
    std::vector<double> close_prices;
    close_prices.reserve(candles.size());
    for (const auto& c : candles) {
        close_prices.push_back(c.close);
    }
    return close_prices;
}


DataCollector::DataCollector(const std::string& symbol, const std::string& interval)
    : symbol_(symbol), interval_(interval) {}

std::string DataCollector::httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("curl initialization failed");
    }
    std::string readBuffer;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
    }
    curl_easy_cleanup(curl);
    return readBuffer;
}

std::vector<Candle> DataCollector::collectData(size_t limit) {
    std::stringstream ss;
    ss << "https://api.binance.com/api/v3/klines?symbol=" << symbol_
       << "&interval=" << interval_ << "&limit=" << limit;
    std::string url = ss.str();

    std::string response = httpGet(url);
    json j = json::parse(response);

    std::vector<Candle> candles;
    for (const auto& item : j) {
        Candle c;
        c.timestamp = item[0].get<long long>();
        c.open  = std::stod(item[1].get<std::string>());
        c.high  = std::stod(item[2].get<std::string>());
        c.low   = std::stod(item[3].get<std::string>());
        c.close = std::stod(item[4].get<std::string>());
        c.volume = std::stod(item[5].get<std::string>());
        candles.push_back(c);
    }
    return candles;
}
