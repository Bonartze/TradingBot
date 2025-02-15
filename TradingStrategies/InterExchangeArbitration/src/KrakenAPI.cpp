#include "../include/KrakenAPI.h"
#include <boost/beast/version.hpp>
#include <iostream>
#include <simdjson.h>

void KrakenAPI::fetch_raw_data(std::vector<OrderBookEntry> &r_order) {
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

    // Доступ к нужному символу
    auto market_data = doc["result"][order.symbol];

    // Исправляем bid/ask цены
    order.bid_price = std::stod(std::string(market_data["b"].at(0).get_string().value()));
    order.ask_price = std::stod(std::string(market_data["a"].at(0).get_string().value()));

    // Исправляем bid_qty и ask_qty
    order.bid_qty = std::stod(std::string(market_data["b"].at(1).get_string().value())); // Объем на лучшей цене bid
    order.ask_qty = std::stod(std::string(market_data["a"].at(1).get_string().value())); // Объем на лучшей цене ask
    r_order.emplace_back(order);
    // just for debug
    std::cout << std::fixed << std::setprecision(10) << "Ask Price: " << order.ask_price << std::endl;
    std::cout << std::fixed << std::setprecision(10) << "Bid Price: " << order.bid_price << std::endl;
    std::cout << std::fixed << std::setprecision(10) << "Bid Quantity: " << order.bid_qty << std::endl;
    std::cout << std::fixed << std::setprecision(10) << "Ask Quantity: " << order.ask_qty << std::endl;
}
