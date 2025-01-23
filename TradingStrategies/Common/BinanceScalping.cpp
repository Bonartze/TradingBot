#include "BinanceScalping.h"
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/json.hpp>
#include <simdjson.h>

BinanceScalping::BinanceScalping(const int8_t &version, const std::string &host_, const std::string &port_,
                                         const std::string &target_) : m_version(version),
                                                                       ctx(ssl::context::tlsv12_client),
                                                                       stream(ioc, ctx), host(host_),
                                                                       port(port_), target(target_) {
    if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{ec};
    }
}

