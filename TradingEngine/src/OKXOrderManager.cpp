#include "../include/OKXOrderManager.h"


void OKXOrderManager::place_order(const std::string &order_type, const std::string &side, double volume,
                                  const std::string &inst_id) {
    const std::string timestamp = generate_timestamp();
    const std::string url = "https://www.okx.com/api/v5/trade/order";

    const nlohmann::json payload = {
        {"instId", inst_id},
        {"tdMode", "cash"},
        {"side", side},
        {"ordType", order_type},
        {"sz", std::to_string(volume)}
    };
    const std::string post_data = payload.dump();

    const std::string message = timestamp + "POST" + "/api/v5/trade/order" + post_data;
    const std::string hmac_digest = hmac(api_secret, message, EVP_sha256);
    const std::string api_sign = base64_encode(hmac_digest);

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("OK-ACCESS-KEY: " + api_key).c_str());
    headers = curl_slist_append(headers, ("OK-ACCESS-SIGN: " + api_sign).c_str());
    headers = curl_slist_append(headers, ("OK-ACCESS-TIMESTAMP: " + timestamp).c_str());
    headers = curl_slist_append(headers, ("OK-ACCESS-PASSPHRASE: " + passphrase).c_str());

    const std::string response = send_post_request(url, post_data, headers);
    std::cout << "Response: " << response << std::endl;
}
