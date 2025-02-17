#include "../include/AbstractOrderManager.h"

std::string AbstractOrderManager::bhmac(const std::string &key, const std::string &data, const EVP_MD *(*hash_func)()) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int len = 0;

    HMAC(hash_func(), key.c_str(), key.length(),
         (unsigned char*)data.c_str(), data.length(), digest, &len);

    return std::string(reinterpret_cast<char*>(digest), len);
}

std::string AbstractOrderManager::base64_encode(const std::string &in) {
    size_t out_len = 4 * ((in.size() + 2) / 3);
    std::string out(out_len, '\0');
    EVP_EncodeBlock(reinterpret_cast<unsigned char*>(&out[0]),
                    reinterpret_cast<const unsigned char*>(in.c_str()), in.size());
    return out;
}

size_t AbstractOrderManager::write_callback(void *contents, size_t size, size_t nmemb, std::string *output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

std::string AbstractOrderManager::send_post_request(const std::string &url, const std::string &post_data, struct curl_slist *headers) {
    std::string response_string;
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    curl_slist_free_all(headers);
    return response_string;
}