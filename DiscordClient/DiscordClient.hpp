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

namespace discord {

class DiscordClient {
public:
    DiscordClient(const std::string& bot_token, DiscordEvents::ResponseCallback response_callback);
    ~DiscordClient();

    void connect(const std::string& uri);
    void send(const std::string& message);

private:

    void send_message(const std::string& channel_id, const std::string& message);
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
    std::vector<std::thread> worker_threads_;
    std::queue<nlohmann::json> event_queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    DiscordEvents::ResponseCallback response_callback_;
    discord::DiscordEventHandler event_handler_;
    std::atomic<bool> stop_threads_;
    int last_sequence_ = -1;

    std::map<std::string, std::string> headers = {
        {"Authorization", "Bot " + bot_token_}
    };
};

} // namespace discord

#endif // DISCORD_CLIENT_HPP
