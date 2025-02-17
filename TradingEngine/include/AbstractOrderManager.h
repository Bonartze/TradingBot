#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/buffer.h>
#include <nlohmann/json.hpp>

class AbstractOrderManager {
protected:
    std::string api_key;
    std::string api_secret;

    AbstractOrderManager(const std::string &key, const std::string &secret)
        : api_key(key), api_secret(secret) {}

    std::string hmac(const std::string &key, const std::string &data, const EVP_MD *(*hash_func)());

    std::string base64_encode(const std::string &in);

    static size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *output);

    std::string send_post_request(const std::string &url, const std::string &post_data, struct curl_slist *headers);
};
