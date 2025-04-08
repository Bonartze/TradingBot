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

enum class Market:std::uint8_t { BINANCE, KRAKEN, OKX };

//! Omitting linters
// NOLINTBEGIN

struct OrderBookEntry {
    std::string symbol;
    double bid_price;
    double ask_price;
    double bid_qty;
    double ask_qty;
    Market market;

    OrderBookEntry(std::string symbol, Market mk) : symbol(symbol), market(mk), bid_price(0.0), ask_price(0.0) {
    }

    double calculate_fees(double trade_amount, bool is_maker_order) {
        if (market == Market::BINANCE) {
            return trade_amount * 0.001;
        } else if (market == Market::KRAKEN) {
            constexpr double MAKER_FEE = 0.0016;
            constexpr double TAKER_FEE = 0.0026;

            double fee_rate = is_maker_order ? MAKER_FEE : TAKER_FEE;
            return trade_amount * fee_rate;
        } else {
            constexpr double maker_fee = 0.0008;
            constexpr double taker_fee = 0.001;

            double fee_rate = is_maker_order ? maker_fee : taker_fee;
            return trade_amount * fee_rate;
        }
    }

    OrderBookEntry() = default;
};

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
    bool is_connected = false;
    // NOLINTEND

public:
    explicit BinanceScalping(int8_t version, std::string host_, std::string port_, std::string target_);

    BinanceScalping(BinanceScalping const &) = delete;

    BinanceScalping(BinanceScalping &&) = delete;

    auto operator=(BinanceScalping const &) -> BinanceScalping & = delete;

    auto operator=(BinanceScalping &&) -> BinanceScalping & = delete;

    BinanceScalping() = delete;

    virtual void fetch_raw_data(std::vector<OrderBookEntry> &) = 0;

    void fetch_raw_data() {
        std::vector<OrderBookEntry> ord;
        fetch_raw_data(ord);
    }


    virtual ~BinanceScalping() = default;

    void connect();
};
