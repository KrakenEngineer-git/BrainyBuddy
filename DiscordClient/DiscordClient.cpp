#include "DiscordClient.hpp"
// #include <curl/curl.h>
// #include <sstream>
namespace discord 
{
    DiscordClient::DiscordClient(const std::string& bot_token, DiscordEvents::ResponseCallback response_callback)
        : bot_token_(bot_token), worker_threads_count_(4), response_callback_(response_callback), stop_threads_(false)
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

    DiscordClient::~DiscordClient()
    {
        stop_threads_ = true;
        cv_.notify_all();
        for (auto& thread : worker_threads_) {
            thread.join();
        }
    }

    void DiscordClient::connect(const std::string& uri) {
        client_handler_ptr->connect(uri, headers);

        client_handler_ptr->set_reconnect_callback([this, uri]() {
            std::cout << "Reconnecting..." << std::endl;
            connect(uri);
        });
        
        client_handler_ptr->set_identify_callback([this]() {
            start_identify_thread();
        });

        client_handler_ptr->set_payload_callback([this](const std::string& payload) {
            handle_payload(payload);
        });

        event_handler_.register_event_handler("MESSAGE_CREATE", [this](const nlohmann::json& data) {
            std::unique_lock<std::mutex> lock(mutex_);
            event_queue_.push(data);
            lock.unlock();
            cv_.notify_one();
        });
        
        for (unsigned int i = 0; i < worker_threads_count_; ++i) {
            worker_threads_.emplace_back([this]() {
                while (true) {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [this]() { return !event_queue_.empty() || stop_threads_; });

                    if (stop_threads_) {
                        break;
                    }

                    nlohmann::json event_data = event_queue_.front();
                    event_queue_.pop();
                    lock.unlock();
                    // Check if the "content" value is not null before accessing it
                    if (!event_data["content"].is_null()) {
                        // Extract message content and channel ID from the event_data
                        std::string message_content = event_data["content"].get<std::string>();
                        std::string channel_id = event_data["channel_id"].get<std::string>();
                        std::cout << event_data["author"]["username"].get<std::string>() << " send: " << message_content << std::endl;

                        if (!message_content.empty() && !channel_id.empty()) {
                            // Invoke your on_message_create function, passing the response as argument
                            nlohmann::json response = event_handler_.on_message_create(event_data,response_callback_);
                            // If the response is not empty, send the response message to the same channel
                            if (!response.empty()) {
                                std::cout << response << std::endl;
                                // Handle message sending
                            }
                        }
                    }
                }
            });
        }
        std::cout << "Discord bot connected" << std::endl;
    }

    void DiscordClient::handle_payload(const std::string& raw_payload) {
        auto payload = nlohmann::json::parse(raw_payload);

        if (payload["op"] == 11) {
            std::cout << "Received Heartbeat Acknowledgement" << std::endl;
        } else if (payload["op"] == 10) { // Opcode 10: Hello
            int heartbeat_interval = payload["d"]["heartbeat_interval"];
            start_heartbeat_thread(heartbeat_interval);
        } else if (payload["t"] == "READY") {
            std::cout << "Received READY event" << std::endl;
        } else {
            event_handler_.handle_event(raw_payload, [](const std::string& error_message) {
                std::cerr << "Error handling event: " << error_message << std::endl;
            });
        }
    }
    
    void DiscordClient::start_heartbeat_thread(int interval_ms) {
        std::cout << "Trying to heartbeat" << std::endl;
        std::thread([this, interval_ms]() {
            while (!stop_threads_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                nlohmann::json heartbeat_payload;
                heartbeat_payload["op"] = 1; // Opcode for Heartbeat
                heartbeat_payload["d"] = last_sequence_;

                if (client_handler_ptr->is_connected()) {
                    std::string heartbeat_str = heartbeat_payload.dump();
                    client_handler_ptr->send(heartbeat_str);
                }
            }
        }).detach();
    }

    void DiscordClient::start_identify_thread() {
        std::cout << "Trying to identify" << std::endl;
        std::thread([this]() {
            nlohmann::json identify_payload{
                {"op", 2},
                {"d", {
                    {"token", bot_token_},
                    {"intents", (1 << 9) | (1 << 15)}, // Add the GUILD_MESSAGES and MESSAGE_CONTENT intents
                    {"properties", {
                        {"$os", "linux"},
                        {"$browser", "disco"},
                        {"$device", "disco"}
                    }},
                    {"compress", false},
                    {"large_threshold", 250},
                    {"shard", {0, 1}}
                }}
            };
            if (client_handler_ptr->is_connected()) {
                std::string identify_str = identify_payload.dump();
                client_handler_ptr->send(identify_str);
            }
        }).detach();
    }


    void DiscordClient::send(const std::string& message)
    {
        client_handler_ptr->send(message);
    }
}
