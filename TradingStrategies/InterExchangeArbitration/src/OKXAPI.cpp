#include "../include/OKXAPI.h"
#include <boost/beast/version.hpp>
#include <iostream>
#include <simdjson.h>



void OKXAPI::fetch_raw_data(std::vector<OrderBookEntry> &r_order) {
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

    auto ticker_data = doc["data"].at(0);

    order.bid_price = std::stod(std::string(ticker_data["bidPx"].get_string().value()));
    order.ask_price = std::stod(std::string(ticker_data["askPx"].get_string().value()));
    order.bid_qty = std::stod(std::string(ticker_data["bidSz"].get_string().value()));
    order.ask_qty = std::stod(std::string(ticker_data["askSz"].get_string().value()));
    r_order.emplace_back(order);
    // Debug
    std::cout << std::fixed << std::setprecision(10) << "Ask Price: " << order.ask_price << std::endl;
    std::cout << std::fixed << std::setprecision(10) << "Bid Price: " << order.bid_price << std::endl;
    std::cout << std::fixed << std::setprecision(10) << "Bid Qty: " << order.bid_qty << std::endl;
    std::cout << std::fixed << std::setprecision(10) << "Ask Qty: " << order.ask_qty << std::endl;
}
