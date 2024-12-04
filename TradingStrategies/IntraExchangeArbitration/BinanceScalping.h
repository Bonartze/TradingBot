#ifndef BINANCESCALPING_H
#define BINANCESCALPING_H

#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/json.hpp>
#include "simdjson.h"
#include <boost/container/flat_map.hpp>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include "../Common/Graph.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;

class BinanceScalping {
    http::response <http::dynamic_body> json_raw_data;
    int version;
    net::io_context ioc;
    ssl::context ctx;
    beast::ssl_stream <beast::tcp_stream> stream;
    std::unordered_map<std::string, double> cryptomarket_pairs;
    Graph order_graph;
public:
    double operator[](const std::string &crypto_pair) {
        return cryptomarket_pairs[crypto_pair];
    }

    // constructor, necessary to clarify the version of the API
    explicit BinanceScalping(const int &version);

    // fetch raw data from Binance API
    void fetch_raw_data();

    // just getting the price map
    const std::unordered_map<std::string, double> &get_price_map(); // нужен ли он вообще?

    //parse obtained data and creating graph order, for clarifying check DOXYGEN docs
    Graph &generate_order_graph(const std::unordered_set<std::string> &);
};

#endif
