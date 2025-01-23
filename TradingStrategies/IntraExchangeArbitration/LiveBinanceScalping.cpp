#include "LiveBinanceScalping.h"
#include <iostream>
#include "../Common/Common.h"

LiveBinanceScalping::LiveBinanceScalping(const int8_t &version, const std::string &host_, const std::string &port_,
                                         const std::string &target_): BinanceScalping(version, host_, port_, target_) {
}

void LiveBinanceScalping::fetch_raw_data(size_t scalping_data_point) {
    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, port);
    if (!beast::get_lowest_layer(stream).connect(results).data()) {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{ec};
    }
    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req{http::verb::get, target, m_version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::connection, "keep-alive");

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::read(stream, buffer, json_raw_data);

    std::string body = beast::buffers_to_string(json_raw_data.body().data());

    simdjson::dom::parser parser;
    simdjson::dom::element json_data = parser.parse(body);

    for (auto symbol_price: json_data) {
        std::string_view symbol = std::string_view(symbol_price["symbol"].get_c_str());
        double price = std::stod(std::string(symbol_price["price"]));
        cryptomarket_pairs.emplace(std::string(symbol), price);
    }
}

const std::unordered_map<std::string, double> &LiveBinanceScalping::get_price_map() {
    return cryptomarket_pairs;
}

Graph &LiveBinanceScalping::generate_order_graph(const std::unordered_set<std::string> &user_symbols) {
    for (const auto &[currency_pair, price]: cryptomarket_pairs) {
        std::string_view base_currency;
        std::string_view quote_currency;
        bool valid_pair = false;

        for (size_t i = 1; i < currency_pair.size(); ++i) {
            base_currency = std::string_view(currency_pair).substr(0, i);
            quote_currency = std::string_view(currency_pair).substr(i);

            if (currencies.count(std::string(base_currency)) && currencies.count(std::string(quote_currency))) {
                valid_pair = true;
                break;
            }
        }

        if (valid_pair && user_symbols.count(std::string(base_currency)) &&
            user_symbols.count(std::string(quote_currency))) {
            order_graph[std::string(base_currency)].emplace(std::string(quote_currency), -log(price));
        }
    }

    for (const auto &[base, edges]: order_graph) {
        std::cout << base << " -> ";
        for (const auto &[quote, weight]: edges) {
            std::cout << quote << " (weight: " << weight << "), ";
        }
        std::cout << std::endl;
    }
    return order_graph;
}
