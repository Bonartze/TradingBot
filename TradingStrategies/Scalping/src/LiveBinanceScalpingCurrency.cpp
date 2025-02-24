#include "../include/LiveBinanceScalpingCurrency.h"
#include "simdjson.h"
#include <boost/beast/version.hpp>
#include <locale>
#include <iomanip>

#include "../../../Logger/include/Logger.h"


LiveBinanceScalpingCurrency::LiveBinanceScalpingCurrency(int8_t version, const std::string &host_,
                                                         const std::string &port_,
                                                         const std::string &target_) : BinanceScalping(
    version, host_, port_, target_) {
}

void LiveBinanceScalpingCurrency::fetch_raw_data() {
    ctx.set_verify_mode(ssl::verify_peer);
    ctx.set_default_verify_paths();

    for (size_t i = 0; i < 1; ++i) {
        tcp::resolver resolver(ioc);
        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

        auto const results = resolver.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        stream.handshake(ssl::stream_base::client);

        http::request<http::string_body> req{http::verb::get, target, m_version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        try {
            std::string body = res.body();
            simdjson::ondemand::parser parser;
            auto doc = parser.iterate(body);

            if (doc["price"].type() == simdjson::ondemand::json_type::string) {
                const std::string_view price_view = doc["price"].get_string();
                const double price = std::stod(std::string(price_view));
                prices.push_back(price);
                Logger(LogLevel::INFO) << std::fixed << "Fetched price: " << std::string(price_view) << "\n";
            } else {
                Logger(LogLevel::ERROR) << "Error: 'price' field is not a string or is missing.\n";
            }
        } catch (const simdjson::simdjson_error &e) {
            Logger(LogLevel::ERROR) << "Simdjson error: " << e.what() << "\n";
        } catch (const std::exception &e) {
            Logger(LogLevel::ERROR) << "Error: " << e.what() << "\n";
        }
    }
}
