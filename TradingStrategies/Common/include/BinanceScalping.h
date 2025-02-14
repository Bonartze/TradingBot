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
    // NOLINTBEGIN
    std::string host;
    std::string port;
    std::string target;
    http::response<http::dynamic_body> json_raw_data;
    int8_t m_version;
    net::io_context ioc;
    ssl::context ctx;
    beast::ssl_stream<beast::tcp_stream> stream;
    bool is_connected = false;
    // NOLINTEND

public:
    explicit BinanceScalping(int8_t version, std::string host_, std::string port_, std::string target_);

    BinanceScalping(BinanceScalping const &) = delete;

    BinanceScalping(BinanceScalping &&) = delete;

    auto operator=(BinanceScalping const &) -> BinanceScalping & = delete;

    auto operator=(BinanceScalping &&) -> BinanceScalping & = delete;

    BinanceScalping() = delete;

    virtual void fetch_raw_data(size_t scalping_data_point) = 0;

    virtual ~BinanceScalping() = default;

    void connect();
};
