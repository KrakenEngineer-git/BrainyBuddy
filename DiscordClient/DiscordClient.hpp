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
    std::string bot_token_;
    std::queue<nlohmann::json> event_queue_;
    std::condition_variable condition_variable_;
    DiscordEvents::CheckIfIsAQuestion check_if_question_;
    DiscordEvents::ResponseCallback response_callback_;
    discord::DiscordEventHandler event_handler_;
    int last_sequence_ = -1;
    std::atomic<bool> running_;
    
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
    std::condition_variable cv_run_;
    std::mutex mtx_run_;

    void increment_request_count();
    bool can_send_request();

    std::mutex mtx_;
    std::condition_variable cv_;

    unsigned int request_count_ = 0;
    std::atomic<unsigned int> active_threads_; 
    std::chrono::time_point<std::chrono::steady_clock> last_request_time_ = std::chrono::steady_clock::now();
};

} // namespace discord

#endif // DISCORD_CLIENT_HPP
