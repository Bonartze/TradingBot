#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
namespace asio = boost::asio;
namespace beast = boost::beast;

boost::beast::http<beast::http::string_body> handle_http_request(
    beast::http::request<beast::http::string_body> const &request);

// is used ssl session to create a secure connection
class SSLSession : public std::enable_shared_from_this<SSLSession> {
public:
    explicit SSLSession(asio::ssl::stream<asio::ip::tcp::socket> socket);

    void start();

private:
    asio::ssl::stream<asio::ip::tcp::socket> socket_;
    char data_[1024];

    void do_read();

    void do_write(std::size_t length);
};

class TradingServer {
public:
    TradingServer(asio::io_context &io_context, short port, asio::ssl::context &ssl_context);
private:
    void do_accept();

    asio::ip::tcp::acceptor acceptor_;

    asio::ssl::context &ssl_context_;
};
