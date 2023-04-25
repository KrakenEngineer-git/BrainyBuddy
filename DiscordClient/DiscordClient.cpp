#include "DiscordClient.hpp"

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

        client_handler_ptr->set_heartbeat_callback([this](int interval_ms) {
            start_heartbeat_thread(interval_ms);
        });

        client_handler_ptr->set_identify_callback([this]() {
            start_identify_thread();
        });

        event_handler_.register_event_handler("MESSAGE_CREATE", [this](const nlohmann::json& data) {
            std::unique_lock<std::mutex> lock(mutex_);
            event_queue_.push(data);
            lock.unlock();
            cv_.notify_one();
        });

        //To be fixed with rest api
        // // Create worker threads
        // for (unsigned int i = 0; i < worker_threads_count_; ++i) {
        //     worker_threads_.emplace_back([this]() {
        //         while (true) {
        //             std::unique_lock<std::mutex> lock(mutex_);
        //             cv_.wait(lock, [this]() { return !event_queue_.empty() || stop_threads_; });

        //             if (stop_threads_) {
        //                 break;
        //             }

        //             nlohmann::json event_data = event_queue_.front();
        //             event_queue_.pop();
        //             lock.unlock();

        //             nlohmann::json response = event_handler_.on_message_create(event_data, response_callback_);

        //             if (!response.empty()) {
        //                 client_handler_ptr->send(response);
        //             }
        //         }
        //     });
        // }
        std::cout << "Discord bot connected" << std::endl;
    }

    void DiscordClient::start_heartbeat_thread(int interval_ms) {
        std::cout<<"Trying to heartbeat"<<std::endl;
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
        std::cout<<"Trying to identify"<<std::endl;
        std::thread([this]() {
            nlohmann::json identify_payload{
                {"op", 2},
                {"d", {
                    {"token", bot_token_},
                    {"intents", 1 << 9}, // Add the GUILD_MESSAGES intent
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

    //To be fixed with RestApi

    // void DiscordClient::send_message(const std::string& channel_id, const std::string& content) {
    //     nlohmann::json payload{
    //         {"op", 8},
    //         {"t", "MESSAGE_CREATE"},
    //         {"d", {
    //             {"content", content},
    //             {"channel_id", channel_id}
    //         }}
    //     };

    //     std::string payload_str = payload.dump();
    //     client_handler_ptr->send(payload_str);
    // }
}