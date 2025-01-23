#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <string>


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;

class BinanceScalping {
protected:
    std::string host;
    std::string port;
    std::string target;
    http::response<http::dynamic_body> json_raw_data;
    int8_t m_version;
    net::io_context ioc;
    ssl::context ctx;
    beast::ssl_stream<beast::tcp_stream> stream;

public:
    explicit BinanceScalping(const int8_t &version, const std::string &host_, const std::string &port_,
                             const std::string &target_);

    BinanceScalping() = delete;

    virtual void fetch_raw_data(size_t scalping_data_point = 100) = 0;

    virtual ~BinanceScalping() = default;
};
