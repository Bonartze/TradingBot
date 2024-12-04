#pragma once

#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <execution>
#include "../Logger/Logger.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

const std::string BINANCE_HOST = "https://testnet.binance.vision";

std::string hmac_sha256(const std::string &, const std::string &);

class OrderManager {
private:
    static std::string api_key;
    static std::string secret_key;
    static CURL *curl;
public:
    void init(const std::string &, const std::string &);

    void cleanup();

    ~OrderManager();

    static void
    curl_api_with_header(std::string &url, std::string &result_json, std::vector<std::string> &extra_http_header,
                         std::string &post_data, std::string &action);

    static size_t curl_cb(void *content, size_t size, size_t nmemb, std::string *buffer);

    void send_order(const char *symbol,
                    const char *side,
                    const char *type,
                    const char *timeInForce,
                    double quantity,
                    double price,
                    const char *newClientOrderId,
                    double stopPrice,
                    double icebergQty,
                    long recvWindow,
                    Json::Value &json_result);
};


