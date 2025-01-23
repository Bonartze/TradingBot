#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip> // Для std::setprecision


// Callback-функция для обработки данных, полученных от libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Функция для выполнения GET-запроса
std::string fetch_price_from_binance(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to fetch data from Binance: " + std::string(curl_easy_strerror(res)));
        }
    }
    return readBuffer;
}

// Функция для извлечения цены из JSON-ответа
double extract_price_from_response(const std::string& json_response) {
    auto price_pos = json_response.find("\"price\":\"");
    if (price_pos == std::string::npos) {
        throw std::runtime_error("Price not found in the response");
    }

    price_pos += 9; // Переходим к позиции после "price":""
    auto end_pos = json_response.find("\"", price_pos);
    if (end_pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format");
    }

    std::string price_str = json_response.substr(price_pos, end_pos - price_pos);
    return std::stod(price_str);
}

int main() {
    const std::string url = "https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT";
    std::vector<double> prices;

    try {
        for (int i = 0; i < 100; ++i) {
            // Выполняем запрос
            std::string response = fetch_price_from_binance(url);

            // Извлекаем цену
            double price = extract_price_from_response(response);

            // Сохраняем цену в вектор
            prices.push_back(price);

            // Устанавливаем формат вывода для дробных чисел
            std::cout << std::fixed << std::setprecision(8);

            // Выводим информацию
            std::cout << "Fetched price #" << (i + 1) << ": " << price << std::endl;

            // Пауза между запросами (например, 500 мс)
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Вывод всех сохранённых цен
        std::cout << "\nCollected prices:\n";
        for (const auto& price : prices) {
            std::cout << price << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
