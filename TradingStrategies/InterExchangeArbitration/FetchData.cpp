#include "FetchData.h"


void BinanceFetchData::FetchCurrentPrice(std::string currency_name, size_t amount) {
    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, port);
    if (!beast::get_lowest_layer(stream).connect(results).data()) {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{ec};
    }
    stream.handshake(ssl::stream_base::client);

    http::request <http::string_body> req{http::verb::get, target, version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::connection, "keep-alive");

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::read(stream, buffer, json_raw_data);

    std::string body = beast::buffers_to_string(json_raw_data.body().data());

    simdjson::dom::parser parser;
    simdjson::dom::element json_data = parser.parse(body);
    std::cout << json_data << std::endl;

}

int main() {
    BinanceFetchData binanceFetchData;
    binanceFetchData.FetchCurrentPrice("BTCUSDT", 100);
    return 0;
}