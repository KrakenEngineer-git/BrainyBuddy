#include "CurlHandler.hpp"
#include <stdexcept>

CurlHandler::CurlHandler() : headers(nullptr) {
    curl = curl_easy_init();
    if(!curl) {
        throw std::runtime_error("Curl initialization failed");
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
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }

    curl_easy_reset(curl);
}
