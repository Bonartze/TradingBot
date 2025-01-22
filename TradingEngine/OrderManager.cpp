
#include "OrderManager.h"

std::string OrderManager::api_key = "";
std::string OrderManager::secret_key = "";
CURL *OrderManager::curl = NULL;

std::string hmac_sha256(const std::string &key, const std::string &data) {
    unsigned char *digest;
    digest = HMAC(EVP_sha256(), key.c_str(), key.length(),
                  reinterpret_cast<const unsigned char *>(data.c_str()), data.length(), nullptr, nullptr);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }

    return oss.str();
}

void OrderManager::init(const std::string &api_key_,
                        const std::string &secret_key_) {
    api_key = api_key_, secret_key = secret_key_;
}

void OrderManager::cleanup() {
    curl_easy_cleanup(OrderManager::curl);
    curl_global_cleanup();
}


OrderManager::~OrderManager() {
    cleanup();
}


/*
   symbol: Символ валютной пары (например, "BTCUSDT").
    side: Сторона ордера ("BUY" или "SELL").
    type: Тип ордера ("LIMIT", "MARKET" и др.).
    timeInForce: Параметр для ордеров типа LIMIT (например, "GTC" - Good Till Canceled).
    quantity: Количество валюты для ордера.
    price: Цена для лимитного ордера.
    newClientOrderId: Идентификатор ордера, который можно задать пользователю.
    stopPrice: Цена стопа для ордеров типа STOP_LOSS или STOP_LOSS_LIMIT.
    icebergQty: Количество для iceberg-ордера (разделение большого ордера на части).
    recvWindow: Интервал времени для ожидания ответа от сервера.
    json_result: Ссылка на объект, в который будет записан результат ответа.
  */
void OrderManager::send_order(const char *symbol,
                              const char *side,
                              const char *type,
                              const char *timeInForce,
                              double quantity,
                              double price,
                              const char *newClientOrderId,
                              double stopPrice,
                              double icebergQty,
                              long recvWindow,
                              Json::Value &json_result) {
    Logger(LogLevel::DEBUG) << "Sending order: symbol=" << symbol << ", side=" << side << ", type=" << type
                            << ", timeInForce=" << timeInForce << ", quantity=" << quantity << ", price=" << price
                            << ", newClientOrderId=" << newClientOrderId << ", stopPrice=" << stopPrice
                            << ", icebergQty=" << icebergQty << ", recvWindow=" << recvWindow;
    if (api_key.size() == 0 || secret_key.size() == 0) {
        Logger(LogLevel::DEBUG) << "<BinaCPP::send_order> API Key and Secret Key has not been set";
        return;
    }
    std::string url(BINANCE_HOST);
    url += "/api/v3/order?";

    std::string action = "POST";

    std::string post_data("symbol=");
    post_data.append(symbol);

    post_data.append("&side=");
    post_data.append(side);

    post_data.append("&type=");
    post_data.append(type);
    if (strcmp(type, "MARKET")) {
        post_data.append("&timeInForce=");
        post_data.append(timeInForce);
        post_data.append("&price=");
        post_data.append(std::to_string(price));
    }
    post_data.append("&quantity=");
    post_data.append(std::to_string(quantity));


    if (strlen(newClientOrderId) > 0) {
        post_data.append("&newClientOrderId=");
        post_data.append(newClientOrderId);
    }

    if (stopPrice > 0.0) {
        post_data.append("&stopPrice=");
        post_data.append(std::to_string(stopPrice));
    }

    if (icebergQty > 0.0) {
        post_data.append("&icebergQty=");
        post_data.append(std::to_string(icebergQty));
    }

    if (recvWindow > 0) {
        post_data.append("&recvWindow=");
        post_data.append(std::to_string(recvWindow));
    }


    post_data.append("&timestamp=");
    auto get_current_ms_epoch = []() -> long {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
    };

    post_data.append(std::to_string(get_current_ms_epoch()));

    std::string signature = hmac_sha256(secret_key.c_str(), post_data.c_str());
    post_data.append("&signature=");
    post_data.append(signature);


    std::vector<std::string> extra_http_header;
    std::string header_chunk("X-MBX-APIKEY: ");
    header_chunk.append(api_key);
    extra_http_header.push_back(header_chunk);

    Logger(LogLevel::DEBUG) << "<BinaCPP::send_order> url = " << url << ", post_data = " << post_data;

    std::string str_result;
    curl_api_with_header(url, str_result, extra_http_header, post_data, action);

    if (str_result.size() > 0) {

        try {
            Json::Reader reader;
            json_result.clear();
            reader.parse(str_result, json_result);

        } catch (std::exception &e) {
            Logger(LogLevel::DEBUG) << "<BinaCPP::send_order> Error ! %s" << std::string(e.what());
        }
        Logger(LogLevel::DEBUG) << "<BinaCPP::send_order> Done.";

    } else {
        Logger(LogLevel::DEBUG) << "<BinaCPP::send_order> Failed to get anything.";
    }

    Logger(LogLevel::DEBUG) << "<BinaCPP::send_order> Done.";
}

size_t
OrderManager::curl_cb(void *content, size_t size, size_t nmemb, std::string *buffer) {
    Logger(LogLevel::DEBUG) << "<BinaCPP::curl_cb> ";

    buffer->append((char *) content, size * nmemb);

    Logger(LogLevel::DEBUG) << "<BinaCPP::curl_cb> done";
    return size * nmemb;
}


void OrderManager::curl_api_with_header(std::string &url, std::string &str_result,
                                   std::vector<std::string> &extra_http_header, std::string &post_data,
                                   std::string &action) {

    Logger(LogLevel::DEBUG) << "<BinaCPP::curl_api>";

    CURLcode res;

    if (OrderManager::curl) {

        curl_easy_setopt(OrderManager::curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(OrderManager::curl, CURLOPT_WRITEFUNCTION, OrderManager::curl_cb);
        curl_easy_setopt(OrderManager::curl, CURLOPT_WRITEDATA, &str_result);
        curl_easy_setopt(OrderManager::curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(OrderManager::curl, CURLOPT_ENCODING, "gzip");

        if (extra_http_header.size() > 0) {

            struct curl_slist *chunk = NULL;
            for (int i = 0; i < extra_http_header.size(); i++) {
                chunk = curl_slist_append(chunk, extra_http_header[i].c_str());
            }
            curl_easy_setopt(OrderManager::curl, CURLOPT_HTTPHEADER, chunk);
        }

        if (post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE") {

            if (action == "PUT" || action == "DELETE") {
                curl_easy_setopt(OrderManager::curl, CURLOPT_CUSTOMREQUEST, action.c_str());
            }
            curl_easy_setopt(OrderManager::curl, CURLOPT_POSTFIELDS, post_data.c_str());
        }

        res = curl_easy_perform(OrderManager::curl);

        if (res != CURLE_OK) {
            Logger(LogLevel::DEBUG) << "<BinaCPP::curl_api> curl_easy_perform() failed: " << curl_easy_strerror(res);
        }

    }

    Logger(LogLevel::DEBUG) << "<BinaCPP::curl_api> done";

}