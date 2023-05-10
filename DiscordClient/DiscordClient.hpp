#ifndef DISCORD_CLIENT_HPP
#define DISCORD_CLIENT_HPP

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

#include "WebSocketHandler/impl/WebsocketClientHandler.hpp"
#include "DiscordEventHandler/DiscordEventHandler.hpp"
#include "utilities/utilities.hpp"
#include "CurlHandler/CurlHandler.hpp"
#include "ThreadPool/ThreadPool.hpp"

namespace discord {

class DiscordClient {
public:
    DiscordClient(const std::string& bot_token, DiscordEvents::CheckIfIsAQuestion check_if_question,DiscordEvents::ResponseCallback response_callback);
    ~DiscordClient();

    void run();
    void stop();

private:
    void connect(const std::string& uri);
    std::string fetch_message(const std::string& channel_id, const std::string& message_id);
    void process_event(const nlohmann::json& data, bool is_update);
    void send_message(const std::string& channel_id, const std::string& message, const std::string& message_id);
    void setup_handlers(const std::string& uri);
    void setup_event_handler();
    void start_worker_threads();

    void reconnect(const std::string& uri);
    void start_heartbeat_thread(int interval_ms);
    void start_identify_thread();
    void handle_payload(const std::string& raw_payload);

    std::unique_ptr<websocket_handler::WebsocketClientHandler> client_handler_ptr;
    const std::string bot_token_;
    unsigned int worker_threads_count_;
    std::queue<nlohmann::json> event_queue_;
    std::condition_variable condition_variable_;
    DiscordEvents::CheckIfIsAQuestion check_if_question_;
    DiscordEvents::ResponseCallback response_callback_;
    discord::DiscordEventHandler event_handler_;
    std::atomic<bool> stop_threads_;
    int last_sequence_ = -1;
    bool running_;
    
    std::map<std::string, std::string> headers = {
        {"Authorization", "Bot " + bot_token_}
    };

    std::unique_ptr<CurlHandler> curlHandler;
    std::unique_ptr<ThreadPool> worker_threads_pool_;
    std::unique_ptr<ThreadPool> event_handling_pool_;
    std::unique_ptr<ThreadPool> task_pool_;
    std::mutex mutex_;

    std::condition_variable heartbeat_cv_;
    std::mutex heartbeat_mutex_;
    bool identify_started_ = false;

};

} // namespace discord

#endif // DISCORD_CLIENT_HPP
