#pragma once

#include <string>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/container/flat_map.hpp>
#include "simdjson.h"
#include <boost/json.hpp>


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;

struct ChosenCurrecny {
    std::string name;
    double price;
    size_t amount;
};

class FetchData {
private:
    ChosenCurrecny currency;
public:
    FetchData() = delete;

    virtual void FetchCurrentPrice(std::string currency_name, size_t amount) = 0;
};


// Strategy pattern

class BinanceFetchData : public FetchData {
private:
    const char *const host = "api.binance.com";
    const char *const port = "443";
    const char *const target = "/api/v3/ticker/price";
    http::response<http::dynamic_body> json_raw_data;
    int version;
    net::io_context ioc;
    ssl::context ctx;
    beast::ssl_stream<beast::tcp_stream> stream;
    std::unordered_map<std::string, double> cryptomarket_pairs;
public:
    void FetchCurrentPrice(std::string currency_name, size_t amount) override;
};


