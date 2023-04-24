#include "DiscordClient.hpp"

namespace discord 
{
    DiscordClient::DiscordClient(const std::string& bot_token, DiscordEvents::ResponseCallback response_callback)
        : bot_token_(bot_token), worker_threads_count_(4), response_callback_(response_callback)
    {
        try
        {
            client_handler_ptr = make_unique<websocket_handler::WebsocketClientHandler>();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void DiscordClient::connect(const std::string& uri)
    {
        client_handler_ptr->connect(uri,headers);

        event_handler_.register_event_handler("MESSAGE_CREATE", [this](const nlohmann::json& data) {
            std::unique_lock<std::mutex> lock(mutex_);
            event_queue_.push(data);
            lock.unlock();
            cv_.notify_one();
        });

        // Create worker threads
        for (unsigned int i = 0; i < worker_threads_count_; ++i) {
            worker_threads_.emplace_back([this]() {
                while (true) {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [this]() { return !event_queue_.empty(); });

                    nlohmann::json event_data = event_queue_.front();
                    event_queue_.pop();
                    lock.unlock();

                    nlohmann::json response = event_handler_.on_message_create(event_data, response_callback_);

                    if (!response.empty()) {
                         client_handler_ptr->send(response);
                    }
                }
            });
        }
    }

    void DiscordClient::send(const std::string& message)
    {
        client_handler_ptr->send(message);
    }
}