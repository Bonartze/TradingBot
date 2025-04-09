#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/json.hpp>
#include <simdjson.h>
#include <boost/container/flat_map.hpp>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include "../../Common/include/Graph.h"
#include "../../Common/include/BinanceScalping.h"

namespace net = boost::asio;
using tcp = net::ip::tcp;

//namespace beast = boost::beast;
//namespace http = beast::http;
//namespace ssl = net::ssl;


class LiveBinanceScalping : public BinanceScalping {
    std::unordered_map<std::string, double> cryptomarket_pairs;
    Graph order_graph;

public:
    explicit LiveBinanceScalping(const int8_t &version, const std::string &host_,
                                 const std::string &port_, const std::string &target_);

    LiveBinanceScalping(const LiveBinanceScalping &) = delete;

    auto operator=(const LiveBinanceScalping &) -> LiveBinanceScalping & = delete;

    LiveBinanceScalping(LiveBinanceScalping &&) noexcept = delete;

    auto operator=(LiveBinanceScalping &&) noexcept -> LiveBinanceScalping & = delete;

    ~LiveBinanceScalping() override = default;

    auto get_price(const std::string &crypto_pair) -> double {
        return cryptomarket_pairs[crypto_pair];
    }

    auto fetch_raw_data(size_t scalping_data_points = 1) -> void;

    auto get_price_map() -> const std::unordered_map<std::string, double> &;

    auto generate_order_graph(const std::unordered_set<std::string> &) -> Graph &;

    void fetch_raw_data(std::vector<OrderBookEntry> &data) override {
    }
};
