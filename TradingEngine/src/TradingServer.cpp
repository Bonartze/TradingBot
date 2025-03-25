#include "../include/TradingServer.h"

#include "../../DataCollector/include/DataCollector.h"
#include "../../TradingStrategies/Common/include/Common.h"
#include <filesystem>
#include <strategies_api.h>
#include <stdexcept>
#include <string>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using json = nlohmann::json;

std::string log_dir = "/home/ivan/TradingBot/TradingEngine/Logs";


void process_scalping(const json &j) {
    if (!std::filesystem::exists("../Logs"))
        std::filesystem::create_directories("../Logs");

    std::string email = j["email"].get<std::string>();
    std::string log_file_name = log_dir + "/scalping_" + email + ".csv";
    CSVLogger logger_scalping = CSVLogger(log_file_name);
    ScalpingStr *scalping = nullptr;
    std::string symbol = j["symbol"].get<std::string>();
    std::string data_frame = j["data_frame"].get<std::string>();
    double balance = j["balance"].get<double>();
    json params = j["parameters"];
    size_t window_size = params["window_size"].get<size_t>();
    double sma_short = params["sma_short"].get<double>();
    double sma_long = params["sma_long"].get<double>();
    double rsi_oversold = params["rsi_oversold"].get<double>();
    double rsi_overbought = params["rsi_overbought"].get<double>();
    bool is_testing = j["is_testing"].get<bool>();

    std::string api_key = j["exchange_api"]["api_key"].get<std::string>();
    std::string secret_key = j["exchange_api"]["secret_key"].get<std::string>();
    TradingParams tp = {sma_short, sma_long, rsi_overbought, rsi_oversold};

    scalping = CreateScalpingStrEx(&tp, balance, false, 0.0, 0.0,
                                   api_key.c_str(), secret_key.c_str(), symbol.c_str(), is_testing);
    DataCollector data_collector(symbol, data_frame);
    auto prices = data_collector.get_close_prices();
    std::cout << window_size << ' ' << prices.size() << std::endl;
    for (auto &a: prices)
        std::cout << a << ' ';
    std::cout << std::endl;

    double result = ScalpingStr_WrapperExecute(scalping, window_size, prices.data(), prices.size(), logger_scalping);
    std::cout << "Scalping strategy result: " << result << std::endl;
    DestroyScalpingStr(scalping);


    std::cout << "process_scalping called.\n";
}

void process_mean_reverse(const json &j) {
    if (!std::filesystem::exists("../Logs"))
        std::filesystem::create_directories("../Logs");

    std::string email = j["email"].get<std::string>();
    std::string log_file_name = log_dir + "/mean_reverse_" + email + ".csv";
    CSVLogger logger_mean_reverse = CSVLogger(log_file_name);
    MeanReverseStrategy *mean_reverse = nullptr;
    std::string symbol = j["symbol"].get<std::string>();
    std::string data_frame = j["data_frame"].get<std::string>();
    double balance = j["balance"].get<double>();
    json params = j["parameters"];
    size_t window_size = params["window_size"].get<size_t>();
    double sma_short = params["sma_short"].get<double>();
    double sma_long = params["sma_long"].get<double>();
    double rsi_oversold = params["rsi_oversold"].get<double>();
    double rsi_overbought = params["rsi_overbought"].get<double>();
    bool is_testing = j["is_testing"].get<bool>();

    std::string api_key = j["exchange_api"]["api_key"].get<std::string>();
    std::string secret_key = j["exchange_api"]["secret_key"].get<std::string>();
    TradingParams tp = {sma_short, sma_long, rsi_overbought, rsi_oversold};

    mean_reverse = CreateMeanReverseStrategyEx(&tp, balance, false, 0.0, 0.0,
                                               api_key.c_str(), secret_key.c_str(), symbol.c_str(), is_testing);
    DataCollector data_collector(symbol, data_frame);
    auto prices = data_collector.get_close_prices();
    // CSVLogger *logger = nullptr;
    std::cout << window_size << ' ' << prices.size() << std::endl;
    for (auto &a: prices)
        std::cout << a << ' ';
    std::cout << std::endl;
    double result = MeanReverseStrategy_WrapperExecute(mean_reverse, window_size, prices.data(), prices.size(),
                                                       logger_mean_reverse);
    std::cout << "Mean reverse strategy result: " << result << std::endl;
    DestroyMeanReverseStrategy(mean_reverse);


    std::cout << "mean_reverse called.\n";
}

void process_arima_garch(const json &j) {
    if (!std::filesystem::exists("../Logs"))
        std::filesystem::create_directories("../Logs");

    std::string email = j["email"].get<std::string>();
    std::string log_file_name = log_dir + "/arima_garch_" + email + ".csv";
    CSVLogger logger_arima_garch = CSVLogger(log_file_name);
    std::cout << "process_arima_garch called.\n";

    std::cout << j;
    std::string symbol = j["symbol"].get<std::string>();
    std::string data_frame = j["data_frame"].get<std::string>();
    double balance = j["balance"].get<double>();
    size_t window_size = j["parameters"]["window_size"].get<size_t>();


    std::string api_key = j["exchange_api"]["api_key"].get<std::string>();
    std::string secret_key = j["exchange_api"]["secret_key"].get<std::string>();
    bool is_testing = j["is_testing"].get<bool>();
    DataCollector data_collector(symbol, data_frame);
    auto prices = data_collector.get_close_prices();
    TradingParams params = {0, 0, 0, 0};
    ArimaGarchAdaptive *model = CreateArimaGarchAdaptiveEx(prices.data(), prices.size(), &params, balance, false, 0.0,
                                                           0.0,
                                                           api_key.c_str(), secret_key.c_str(), symbol.c_str(),
                                                           is_testing);

    if (!model) {
        std::cerr << "Error: Failed to create ARIMA-GARCH model.\n";
        return;
    }

    if (prices.empty()) {
        std::cerr << "Error: No price data available for " << symbol << ".\n";
        DestroyArimaGarchAdaptive(model);
        return;
    }

    //CSVLogger *logger = nullptr;


    double result = ArimaGarchStrategy_WrapperExecute(model, window_size, prices.data(), prices.size(),
                                                      logger_arima_garch);
    std::cout << "ARIMA-GARCH strategy result: " << result << std::endl;


    DestroyArimaGarchAdaptive(model);
}

void process_bayesian(const json &j) {
    if (!std::filesystem::exists("../Logs"))
        std::filesystem::create_directories("../Logs");

    std::string email = j["email"].get<std::string>();
    std::string log_file_name = log_dir + "/bayesian_" + email + ".csv";
    CSVLogger logger_bayesian = CSVLogger(log_file_name);

    BayesianSignalFiltering *bayesian = nullptr;

    std::string symbol = j["symbol"].get<std::string>();
    std::string data_frame = j["data_frame"].get<std::string>();
    double balance = j["balance"].get<double>();


    // bool position_open = j.value("position_open", false);
    //double entry_price = j.value("entry_price", 0.0);
    //double quantity = j.value("quantity", 0.0);


    std::string api_key = j["exchange_api"]["api_key"].get<std::string>();
    std::string secret_key = j["exchange_api"]["secret_key"].get<std::string>();
    bool is_testing = j["is_testing"].get<bool>();


    json params = j["parameters"];

    double window_size = params.value("window_size", 50.0);


    TradingParams tp = {};


    bayesian = CreateBayesianStrategyEx(
            &tp,
            balance,
            false,
            0.0,
            0.0,
            api_key.c_str(),
            secret_key.c_str(),
            symbol.c_str(),
            is_testing
    );


    DataCollector data_collector(symbol, data_frame);
    auto prices = data_collector.get_close_prices();


    // CSVLogger *logger = nullptr;


    double result = BayesianStrategy_WrapperExecute(
            bayesian,
            static_cast<size_t>(window_size),
            prices.data(),
            prices.size(),
            logger_bayesian
    );

    std::cout << "Bayesian strategy result: " << result << std::endl;


    DestroyBayesianStrategy(bayesian);
}

void process_inter_exchange_arbitrage(const json &j) {
    std::string symbol = j["symbol"].get<std::string>();
    std::string data_frame = j["data_frame"].get<std::string>();


    std::string api_key = j["exchange_api"]["api_key"].get<std::string>();
    std::string secret_key = j["exchange_api"]["secret_key"].get<std::string>();

    RunInter_exchangeArbitrage(symbol.c_str());

    std::cout << "process_inter_exchange_arbitrage called.\n";
}

void process_intra_exchange_arbitrage(const json &j) {

    std::cout << "process_intra_exchange_arbitrage called.\n";

    std::vector<std::string> symbols = j["symbols"].get<std::vector<std::string>>();
    if (symbols.empty()) {
        std::cerr << "No symbols provided.\n";
        return;
    }

    std::vector<const char *> crypto_pairs;
    for (const auto &s: symbols) {
        crypto_pairs.push_back(s.c_str());
    }
    crypto_pairs.push_back(nullptr);

    int version = j["version"].get<int>();

    std::string api_key = j["exchange_api"]["api_key"].get<std::string>();
    std::string secret_key = j["exchange_api"]["secret_key"].get<std::string>();


    RunIntra_exchangeArbitrage(crypto_pairs.data(), version, api_key.c_str(), secret_key.c_str());
}


http::response<http::string_body> handle_http_request(
        http::request<http::string_body> const &req) {
    http::response<http::string_body> response;


    // for local server launch
//    response.set(http::field::access_control_allow_origin, "http://localhost:3000");
//    response.set(http::field::access_control_allow_methods, "POST, GET, OPTIONS");
//    response.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
//    response.set(http::field::access_control_max_age, "86400");


    response.set(http::field::access_control_allow_origin, "*");
    response.set(http::field::access_control_allow_methods, "POST, GET, OPTIONS");
    response.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
    response.set(http::field::access_control_max_age, "86400");


    if (req.method() == http::verb::options && req.target() == "/application/json") {
        response.result(http::status::ok);
        response.body() = "";
        response.prepare_payload();
        return response;
    }


    if (req.method() == http::verb::post && req.target() == "/application/json") {
        auto j = json::parse(req.body(), nullptr, false);
        if (j.is_discarded()) {
            response.result(http::status::bad_request);
            response.body() = "Invalid JSON in request body";
            response.prepare_payload();
            return response;
        }

        std::string strategy = j.value("strategy", "");
        try {
            if (strategy == "scalping") {
                process_scalping(j);
            } else if (strategy == "mean reverse") {
                process_mean_reverse(j);
            } else if (strategy == "inter exchange arbitrage") {
                process_inter_exchange_arbitrage(j);
            } else if (strategy == "intra exchange arbitrage") {
                process_intra_exchange_arbitrage(j);
            } else if (strategy == "arima garch") {
                process_arima_garch(j);
            } else if (strategy == "bayesian signal pro") {
                process_bayesian(j);
            } else {
                throw std::runtime_error("Invalid or unknown strategy");
            }

            response.result(http::status::accepted);
            response.body() = "Request accepted for processing";
        } catch (const std::exception &e) {
            response.result(http::status::internal_server_error);
            response.body() = std::string("Strategy error: ") + e.what();
        }
        response.prepare_payload();
    } else if (req.method() == http::verb::get) {
        response.result(http::status::ok);
        response.body() = "GET request received";
        response.prepare_payload();
    } else {
        response.result(http::status::bad_request);
        response.body() = "Unsupported HTTP method";
        response.prepare_payload();
    }

    return response;
}


Session::Session(net::ip::tcp::socket socket)
        : socket_(std::move(socket)) {
}

void Session::start() {
    do_read();
}

void Session::do_read() {
    auto self = shared_from_this();

    buffer_.consume(buffer_.size());
    request_ = {};


    http::async_read(socket_, buffer_, request_,
                     [this, self](beast::error_code ec, std::size_t bytes_transferred) {
                         if (!ec) {
                             auto response = handle_http_request(request_);
                             do_write(std::move(response));
                         } else {
                             std::cerr << "Read error: " << ec.message() << std::endl;
                         }
                     }
    );
}

void Session::do_write(http::response<http::string_body> response) {
    auto self = shared_from_this();

    auto sp = std::make_shared<http::response<http::string_body> >(std::move(response));


    http::async_write(socket_, *sp,
                      [this, self, sp](beast::error_code ec, std::size_t) {
                          if (!ec) {
                              if (sp->need_eof()) {
                                  beast::error_code ignore_ec;
                                  socket_.shutdown(net::ip::tcp::socket::shutdown_send, ignore_ec);
                                  return;
                              }

                              do_read();
                          } else {
                              std::cerr << "Write error: " << ec.message() << std::endl;
                          }
                      }
    );
}

TradingServer::TradingServer(net::io_context &io_context, short port)
        : io_context_(io_context),
          acceptor_(io_context_, net::ip::tcp::endpoint(net::ip::tcp::v4(), port)) {
    do_accept();
}

void TradingServer::do_accept() {
    acceptor_.async_accept(
            [this](boost::system::error_code ec, net::ip::tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                } else {
                    std::cerr << "Accept error: " << ec.message() << std::endl;
                }

                do_accept();
            }
    );
}

void TradingServer::run(unsigned numThreads) {
    std::vector<std::thread> threads;
    threads.reserve(numThreads > 0 ? numThreads : 1);

    for (unsigned i = 0; i < numThreads; ++i) {
        threads.emplace_back([this] {
            io_context_.run();
        });
    }
    for (auto &t: threads) {
        t.join();
    }
}

int main() {
    try {
        boost::asio::io_context io_context;

        TradingServer server(io_context, 8080);

        server.run(2);
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
