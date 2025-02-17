#include "../include/TradingServer.h"

boost::beast::http<beast::http::string_body> handle_http_request(
    beast::http::request<beast::http::string_body> const &request) {
    if (request.method() == beast::http::verb::post && request.target() == "/application/json") {
        auto json_request = nlohmann::json::parse(request.body());
        /*
         * call method for handling json_request
         *
         */
    } else if (request.method() == beast::http::verb::get) {
        /*
         * call method to get result from executed query
         */
    } else {
        // handlers for PUT and DELETE aren't done yet
    }
}

void SSLSession::start() {
    auto self = shared_from_this();
    socket_.async_handshake(asio::ssl::stream_base::server, [this, self](const boost::system::error_code &error) {
        if (!error) {
            do_read();
        }
    });
}

void SSLSession::do_read() {
    auto self = shared_from_this();
    socket_.async_read_some(asio::buffer(data_),
                            [this, self](const boost::system::error_code &error, std::size_t length) {
                                if (!error) {
                                    do_write(handle_http_request());
                                }
                            });
}

void SSLSession::do_write(std::size_t length) {
    auto self = shared_from_this();
    async_write(socket_, asio::buffer(data_, length),
                [this, self](const boost::system::error_code &error, std::size_t length) {
                    if (!error) {
                        do_read();
                    }
                });
}

TradingServer::TradingServer(asio::io_context &io_context, int16_t port, asio::ssl::context &ssl_context)
    : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), ssl_context_(ssl_context) {
    do_accept();
}

void TradingServer::do_accept() {
    acceptor_.async_accept([this](const boost::system::error_code &error, asio::ip::tcp::socket socket) {
        if (!error) {
            std::make_shared<SSLSession>(asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket), ssl_context_))->
                    start();
        }
        do_accept();
    });
}
