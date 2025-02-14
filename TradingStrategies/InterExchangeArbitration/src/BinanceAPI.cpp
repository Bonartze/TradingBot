#include "../include/BinanceAPI.h"

#include "simdjson.h"
#include <iostream>
#include <boost/beast/version.hpp>

void BinanceAPI::fetch_raw_data(size_t scalping_data_point) {
    connect();

    http::request<http::string_body> req{http::verb::get, target, m_version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::connection, "keep-alive");

    http::write(stream, req);

    beast::flat_buffer buffer{4096};
    http::response<http::string_body> json_raw_data;
    http::read(stream, buffer, json_raw_data);

    auto padded_json = simdjson::padded_string(json_raw_data.body());

    simdjson::ondemand::parser parser;
    simdjson::ondemand::document doc = parser.iterate(padded_json);

    local_price = std::stod(std::string(doc["price"].get_string().value()));


    std::cout << std::fixed << std::setprecision(10) << local_price << std::endl;
}


int main() {
    BinanceAPI b("BTCUSDT", 11, "api.binance.com", "443", "/api/v3/ticker/price");
    b.fetch_raw_data();
}
