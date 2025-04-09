#include "../include/BinanceAPI.h"

#include "simdjson.h"
#include <boost/beast/version.hpp>


auto BinanceAPI::fetch_raw_data(std::vector<OrderBookEntry> &r_order) -> void {
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

    order.bid_price = std::stod(std::string(doc["bidPrice"].get_string().value()));
    order.ask_price = std::stod(std::string(doc["askPrice"].get_string().value()));
    order.bid_qty = std::stod(std::string(doc["bidQty"].get_string().value()));
    order.ask_qty = std::stod(std::string(doc["askQty"].get_string().value()));
    r_order.emplace_back(order);
    /*std::cout << std::fixed << std::setprecision(10)
            << "Bid Price: " << order.bid_price << " (Qty: " << order.bid_qty << ")\n"
            << "Ask Price: " << order.ask_price << " (Qty: " << order.ask_qty << ")\n";*/
}
