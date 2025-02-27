#include "../include/BinanceScalping.h"
#include <boost/beast/version.hpp>
#include <simdjson.h>

BinanceScalping::BinanceScalping(int8_t version, std::string host_, std::string port_,
                                 std::string target_): m_version(version),
                                                       host(std::move(host_)),
                                                       port(std::move(port_)),
                                                       target(std::move(target_)),
                                                       ctx(ssl::context::tlsv12_client),
                                                       stream(ioc, ctx) {
    if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        const beast::error_code er_code{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{er_code};
    }
}

void BinanceScalping::connect() {
    if (!is_connected) {
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        stream.handshake(ssl::stream_base::client);
        is_connected = true;
    }
}

void BinanceScalping::fetch_raw_data(std::vector<OrderBookEntry> &v) {}
