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

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;

class LiveBinanceScalping : public BinanceScalping {
private:
    std::unordered_map<std::string, double> cryptomarket_pairs;
    Graph order_graph;

public:
    double get_price(const std::string &crypto_pair) {
        return cryptomarket_pairs[crypto_pair];
    }

    // constructor, necessary to clarify the version of the API
    explicit LiveBinanceScalping(const int8_t &version, const std::string &host_, const std::string &port_,
                                 const std::string &target_);

    // fetch raw data from Binance API
    void fetch_raw_data(size_t scalping_data_points = 1) override;

    // just getting the price map
    const std::unordered_map<std::string, double> &get_price_map();

    //parse obtained data and creating graph order, for clarifying check DOXYGEN docs
    Graph &generate_order_graph(const std::unordered_set<std::string> &);

    ~LiveBinanceScalping() = default;
};
