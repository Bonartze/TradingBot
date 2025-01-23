#include "LiveBinanceScalpingCurrency.h"
#include "simdjson.h"
#include <boost/beast/version.hpp>

LiveBinanceScalpingCurrency::LiveBinanceScalpingCurrency(int8_t version, const std::string &host_,
                                                         const std::string &port_,
                                                         const std::string &target_) : BinanceScalping(
    version, host_, port_, target_) {
}

void LiveBinanceScalpingCurrency::fetch_raw_data(size_t scalping_data_point) {
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
    for (size_t i = 0; i < scalping_data_point; i++) {
        http::write(stream, req);

        beast::flat_buffer buffer;
        http::read(stream, buffer, json_raw_data);

        std::string body = beast::buffers_to_string(json_raw_data.body().data());

        simdjson::dom::parser parser;
        simdjson::dom::element json_data = parser.parse(body);

        for (auto symbol_price: json_data)
            prices.push_back(std::stod(std::string(symbol_price["price"])));
    }
}
