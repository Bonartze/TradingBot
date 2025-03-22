#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;


void process_scalping(nlohmann::json j);

void process_mean_reverse(nlohmann::json j);

void process_inter_exchange_arbitrage(nlohmann::json j);

void process_intra_exchange_arbitrage(nlohmann::json j);

void process_arima_garch(nlohmann::json j);


beast::http::response<beast::http::string_body> handle_http_request(
    beast::http::request<beast::http::string_body> const &request
);


class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(net::ip::tcp::socket socket);


    void start();

private:
    void do_read();

    void do_write(beast::http::response<beast::http::string_body> response);

private:
    net::ip::tcp::socket socket_;
    beast::flat_buffer buffer_;
    beast::http::request<beast::http::string_body> request_;
};


class TradingServer {
public:
    TradingServer(net::io_context &io_context, short port);


    void run(unsigned numThreads = 1);

private:
    void do_accept();

    net::io_context &io_context_;
    net::ip::tcp::acceptor acceptor_;
};
