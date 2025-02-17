#include "../include/KrakenOrderManager.h"

void KrakenOrderManager::place_order(const std::string &order_type, const std::string &side, double volume,
                                     const std::string &pair) {
    uint64_t nonce = generate_nonce();
    const std::string url = "https://api.kraken.com/0/private/AddOrder";

    const nlohmann::json payload = {
        {"nonce", nonce},
        {"ordertype", order_type},
        {"type", side},
        {"volume", volume},
        {"pair", pair}
    };
    const std::string post_data = payload.dump();

    const std::string hmac_digest = hmac(api_secret, std::to_string(nonce) + post_data, EVP_sha512);
    const std::string api_sign = base64_encode(hmac_digest);

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, ("API-Key: " + api_key).c_str());
    headers = curl_slist_append(headers, ("API-Sign: " + api_sign).c_str());

    const std::string response = send_post_request(url, post_data, headers);
    std::cout << "Response: " << response << std::endl;
}
