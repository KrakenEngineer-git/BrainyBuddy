#ifndef CURL_HANDLER_HPP
#define CURL_HANDLER_HPP

#include <string>
#include <curl/curl.h>
#include <vector>

#include <nlohmann/json.hpp>
#include <mutex>

class CurlHandler {
public:
    CurlHandler();
    ~CurlHandler();

    void AddHeader(const std::string& header);

    std::string Get(const std::string& url);
    std::string post(const std::string& url, const std::string& payload, bool get_response);

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    CURL* curl;
    struct curl_slist* headers;
    std::mutex curl_mutex;
};


#endif // CURL_HANDLER_HPP