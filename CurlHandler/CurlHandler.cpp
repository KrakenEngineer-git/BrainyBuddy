#include "CurlHandler.hpp"
#include <stdexcept>
#include <thread>

CurlHandler::CurlHandler() : headers(nullptr) {
    curl = curl_easy_init();
    if(!curl) {
        throw std::runtime_error("Curl initialization failed in CurlHandler constructor");
    }
}

CurlHandler::~CurlHandler() {
    if (headers) {
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);
}

void CurlHandler::AddHeader(const std::string& header) {
    headers = curl_slist_append(headers, header.c_str());
}

size_t CurlHandler::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

std::string CurlHandler::Get(const std::string& url) {
    std::string readBuffer;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }

    curl_easy_reset(curl);

    return readBuffer;
}

void CurlHandler::post(const std::string& url, const std::string& payload) {
    while (true) { 
        std::lock_guard<std::mutex> lock(curl_mutex);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_reset(curl);

        if (http_code == 429) { // rate limit hit
            nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);
            int retryAfter = jsonResponse["retry_after"];
            std::this_thread::sleep_for(std::chrono::milliseconds(retryAfter));
            // Retry the request
        } else if (res != CURLE_OK) {
            std::string error_message = "Curl post request failed with error: ";
            error_message += curl_easy_strerror(res);
            throw std::runtime_error(error_message);
        } else {
            break; // Success, exit the loop
        }
    }
}
