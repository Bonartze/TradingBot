#include "LiveBinanceScalpingCurrency.h"
#include "simdjson.h"
#include <boost/beast/version.hpp>
#include <locale>


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

        json_raw_data.clear();

        beast::flat_buffer buffer;
        http::read(stream, buffer, json_raw_data);

        std::string body = beast::buffers_to_string(json_raw_data.body().data());

        size_t start = 0;
        while (start < body.size()) {
            size_t end = body.find('}', start);
            if (end == std::string::npos) break;

            std::string json_object = body.substr(start, end - start + 1);

            try {
                simdjson::dom::parser parser;
                simdjson::dom::element elem = parser.parse(json_object);

                std::cout << "Parsed JSON: " << elem << std::endl;

                auto num_str = std::string(elem["price"]);
                char *endptr;
                double price = std::strtod(num_str.c_str(), &endptr);
                std::cout << price << std::endl;
                prices.push_back(price);
            } catch (const simdjson::simdjson_error &e) {
                std::cerr << "JSON parsing error: " << e.what() << std::endl;
            }

            start = end + 1;
        }
    }


    beast::get_lowest_layer(stream).close();
}
