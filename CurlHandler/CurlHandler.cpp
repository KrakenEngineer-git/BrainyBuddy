#include "CurlHandler.hpp"

CurlHandler::CurlHandler() : headers(nullptr), stop_processing_(false)
{
    request_processor_thread_ = std::thread(&CurlHandler::process_requests, this);
}

CurlHandler::~CurlHandler()
{
    if (headers)
    {
        curl_slist_free_all(headers);
    }
    stop_processing_ = true;
    request_queue_condition_variable_.notify_one();
    if (request_processor_thread_.joinable())
    {
        request_processor_thread_.join();
    }
}

void CurlHandler::enqueue_request(std::function<void()> request)
{
    {
        std::lock_guard<std::mutex> lock(request_queue_mutex_);
        request_queue_.push(request);
    }
    request_queue_condition_variable_.notify_one();
}

void CurlHandler::process_requests()
{
    while (!stop_processing_)
    {
        std::unique_lock<std::mutex> lock(request_queue_mutex_);
        request_queue_condition_variable_.wait(lock, [this]() { return !request_queue_.empty() || stop_processing_; });
        if (stop_processing_)
        {
            break;
        }
        std::function<void()> request = request_queue_.front();
        request_queue_.pop();
        lock.unlock();
        request();
    }
}

void CurlHandler::AddHeader(const std::string &header)
{
    headers = curl_slist_append(headers, header.c_str());
}

size_t CurlHandler::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    std::string *str = static_cast<std::string *>(userp);
    if (!str)
    {
        return 0;
    }
    str->append((char *)contents, totalSize);
    return totalSize;
}

std::string CurlHandler::Get(const std::string &url)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        throw std::runtime_error("Curl initialization failed in CurlHandler::Get");
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlHandler::WriteCallback);

    std::string response = perform_curl_request(curl);

    curl_easy_cleanup(curl);

    return response;
}

CurlHandler::Response CurlHandler::post(const std::string &url, const std::string &payload, bool get_response)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        throw std::runtime_error("Curl initialization failed in CurlHandler::post");
    }

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    std::string readBuffer;
    if (get_response)
    {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlHandler::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    }

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        throw std::runtime_error(curl_easy_strerror(res));
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl);

    return {http_code, readBuffer};
}

std::string CurlHandler::perform_curl_request(CURL *curl)
{
    std::string readBuffer;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        throw std::runtime_error(curl_easy_strerror(res));
    }

    return readBuffer;
}