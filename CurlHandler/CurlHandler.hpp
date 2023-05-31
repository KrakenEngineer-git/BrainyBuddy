#ifndef CURL_HANDLER_HPP
#define CURL_HANDLER_HPP

#include "ThreadPool/ThreadPool.hpp"
#include <condition_variable>
#include <curl/curl.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
class CurlHandler
{
  public:
    struct Response
    {
        long http_code;
        std::string body;
    };

    CurlHandler();
    ~CurlHandler();
    void enqueue_request(std::function<void()> request);
    std::string Get(const std::string &url);
    Response post(const std::string &url, const std::string &payload, bool get_response);
    void AddHeader(const std::string &header);

  private:
    void process_requests();
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static std::string perform_curl_request(CURL *curl);

    curl_slist *headers = nullptr;
    bool stop_processing_ = false;
    std::queue<std::function<void()>> request_queue_;
    std::thread request_processor_thread_;
    std::mutex request_queue_mutex_;
    std::condition_variable request_queue_condition_variable_;
};

#endif // CURL_HANDLER_HPP