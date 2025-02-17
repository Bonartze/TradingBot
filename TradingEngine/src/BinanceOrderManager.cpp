#include "../include/BinanceOrderManager.h"

void BinanceOrderManager::place_order(const std::string &symbol, const std::string &side,
                                      const std::string &order_type, const std::string &timeInForce,
                                      double quantity, double price, const std::string &newClientOrderId,
                                      double stopPrice, double icebergQty, long recvWindow) {
    uint64_t timestamp = generate_timestamp();
    const std::string url = BINANCE_HOST + "/api/v3/order";

    nlohmann::json payload = {
        {"symbol", symbol},
        {"side", side},
        {"type", order_type},
        {"quantity", std::to_string(quantity)},
        {"timestamp", timestamp}
    };

    if (order_type == "LIMIT") {
        payload["timeInForce"] = timeInForce;
        payload["price"] = std::to_string(price);
    }

    if (!newClientOrderId.empty()) {
        payload["newClientOrderId"] = newClientOrderId;
    }

    if (stopPrice > 0.0) {
        payload["stopPrice"] = std::to_string(stopPrice);
    }

    if (icebergQty > 0.0) {
        payload["icebergQty"] = std::to_string(icebergQty);
    }

    if (recvWindow > 0) {
        payload["recvWindow"] = recvWindow;
    }

    std::string query_string;
    for (auto &[key, value]: payload.items()) {
        query_string += key + "=" + value.get<std::string>() + "&";
    }

    query_string.pop_back();
    const std::string signature = hmac(api_secret, query_string, EVP_sha256);
    query_string += "&signature=" + signature;

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-MBX-APIKEY: " + api_key).c_str());

    const std::string response = send_post_request(url + "?" + query_string, "", headers);
    std::cout << "Response: " << response << std::endl;
}
