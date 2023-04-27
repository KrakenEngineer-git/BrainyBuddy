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

        client_handler_ptr->set_heartbeat_callback([this](int interval_ms) {
            start_heartbeat_thread(interval_ms);
        });

        client_handler_ptr->set_identify_callback([this]() {
            start_identify_thread();
        });


        client_handler_ptr->set_message_callback([this](const std::string& message_payload) {
            event_handler_.handle_event(message_payload, [](const std::string& error_message) {
                std::cerr << "Error handling event: " << error_message << std::endl;
            });
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
                    if (!event_data["d"]["content"].is_null()) {
                        // Extract message content and channel ID from the event_data
                        std::string message_content = event_data["d"]["content"].get<std::string>();
                        std::string channel_id = event_data["d"]["channel_id"].get<std::string>();
                        std::cout << "Message Content " << message_content << std::endl;

                        if (!message_content.empty()) {
                        // Invoke your response_callback_, passing the message content as argument
                            std::string response = response_callback_(message_content);

                            // If the response is not empty, send the response message to the same channel
                            if (!response.empty()) {
                                // send_message(channel_id, response);
                            }
                        }
                    }
                }
            });
        }
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

    static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::stringstream*)userp)->write((char*)contents, size * nmemb);
        return size * nmemb;
    }

    // void DiscordClient::send_message(const std::string& channel_id, const std::string& content) {
    //     std::string url = "https://discord.com/api/v10/channels/" + channel_id + "/messages";
    //     std::stringstream response;

    //     CURL* curl = curl_easy_init();
    //     if (curl) {
    //         struct curl_slist* headers = NULL;
    //         headers = curl_slist_append(headers, "Content-Type: application/json");
    //         std::string auth_header = "Authorization: Bot " + bot_token_;
    //         headers = curl_slist_append(headers, auth_header.c_str());

    //         nlohmann::json payload{
    //             {"content", content}
    //         };
    //         std::string payload_str = payload.dump();

    //         curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    //         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    //         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    //         curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
    //         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    //         CURLcode res = curl_easy_perform(curl);
    //         if (res != CURLE_OK) {
    //             std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    //         }

    //         curl_easy_cleanup(curl);
    //         curl_slist_free_all(headers);
    //     }
    // }
}