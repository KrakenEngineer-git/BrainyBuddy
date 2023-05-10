#include "DiscordClient.hpp"
namespace discord 
{
    DiscordClient::DiscordClient(const std::string& bot_token, DiscordEvents::CheckIfIsAQuestion check_if_question, DiscordEvents::ResponseCallback response_callback)
        : bot_token_(bot_token), worker_threads_count_(4), check_if_question_(check_if_question), response_callback_(response_callback), stop_threads_(false),
        curlHandler(make_unique<CurlHandler>()) {
        try {
            client_handler_ptr = make_unique<websocket_handler::WebsocketClientHandler>();
            curlHandler->AddHeader("Authorization: Bot " + bot_token_);
            curlHandler->AddHeader("Content-Type: application/json");

            worker_threads_pool_ = make_unique<ThreadPool>(4);
            event_handling_pool_ = make_unique<ThreadPool>(worker_threads_count_);
            task_pool_ = make_unique<ThreadPool>(2);

        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

    void DiscordClient::run() 
    {
        connect("wss://gateway.discord.gg/?v=10&encoding=json");
        running_ = true;

        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void DiscordClient::stop() 
    {
        running_ = false;   
    }

    DiscordClient::~DiscordClient()
    {
        stop();
        stop_threads_ = true;
        std::cout << "DiscordClient destructor called" << std::endl;
    }

    void DiscordClient::connect(const std::string& uri) {
        client_handler_ptr->connect(uri, headers);
        setup_handlers(uri);
        setup_event_handler();
        start_worker_threads();
    }

    void DiscordClient::setup_handlers(const std::string& uri) 
    {
        client_handler_ptr->set_open_handler([this]() {
            start_identify_thread();
        });

        client_handler_ptr->set_fail_handler([this, uri]() {
            std::cout << "Reconnecting..." << std::endl;
            reconnect(uri);
        });

        client_handler_ptr->set_error_handler([this, uri](const std::string& error_message) {
            std::cerr << "WebSocket Error: " << error_message << std::endl;
            reconnect(uri);
        });

        client_handler_ptr->set_message_handler([this](const std::string& payload) {
            handle_payload(payload);
        });

        client_handler_ptr->set_close_handler([this, uri](int status, const std::string& reason) {
            std::cout << "WebSocket connection closed with status " << status << " and reason: " << reason << std::endl;
            std::cout << "Reconnecting..." << std::endl;
            reconnect(uri);
        });
    }

    void DiscordClient::process_event(const nlohmann::json& data, bool is_update)
    {
        event_handling_pool_->enqueue_task([this, data, is_update]() {
            nlohmann::json response;

            if(is_update)
            {
                response = event_handler_.on_message_update(data);

                if(response.is_null())
                {
                    std::cerr << "Error: on_message_update returned a null JSON object" << std::endl;
                    return;
                }
            }

            response = event_handler_.on_message_create(is_update ? response : data, check_if_question_, response_callback_);

            if(!response.empty())
            {
                std::lock_guard<std::mutex> lock(mutex_);
                event_queue_.push(response);
                condition_variable_.notify_one();
            }
        });   
    }

    void DiscordClient::setup_event_handler() 
    {
            event_handler_.register_event_handler("MESSAGE_CREATE", [this](const nlohmann::json& data) {

            process_event(data, false);

        });

        event_handler_.register_event_handler("MESSAGE_UPDATE", [this](const nlohmann::json& data) {

            process_event(data, true);
            
        });


        event_handler_.register_event_handler("MESSAGE_DELETE", [this](const nlohmann::json& data) {
            event_handler_.on_message_delete(data);
        });
    }

    void DiscordClient::start_worker_threads()
    {
        for (unsigned int i = 0; i < worker_threads_count_; ++i) {
            worker_threads_pool_->enqueue_task([this]() {
                while (true)
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    condition_variable_.wait(lock, [this]() {
                        return !event_queue_.empty() || stop_threads_;
                    });

                    if (stop_threads_) {
                        return;
                    }

                    nlohmann::json event_data = event_queue_.front();
                    event_queue_.pop();
                    lock.unlock();

                    std::cout << "Event data: " << event_data << std::endl;
                    // Check if the "content" value is not null before accessing it
                    if (!event_data.is_null()) {
                        if (event_data.value("action", "NOT FOUND") == "send_message") {
                            send_message(event_data.value("channel_id", "NOT FOUND"), event_data.value("content", "NOT FOUND"), event_data.value("message_id", "NOT FOUND"));
                        }
                    }
                }
            });
        }

        std::cout << "Discord bot connected" << std::endl;
    }

    void DiscordClient::handle_payload(const std::string& raw_payload) {
        try
        {
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
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "Error parsing JSON payload: " << e.what() << std::endl;
        }
    }
    
    void DiscordClient::start_heartbeat_thread(int interval_ms) {
        std::cout << "Trying to heartbeat" << std::endl;

        {
            std::unique_lock<std::mutex> lock(heartbeat_mutex_);
            heartbeat_cv_.wait(lock, [this]() { return identify_started_; });
        }

        task_pool_->enqueue_task([this, interval_ms]() {
            while (!stop_threads_) {
                std::cout << "Heartbeat thread sleeping for " << interval_ms << " milliseconds" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                nlohmann::json heartbeat_payload;
                heartbeat_payload["op"] = 1; // Opcode for Heartbeat
                heartbeat_payload["d"] = last_sequence_;

                if (client_handler_ptr->is_connected()) {
                    std::string heartbeat_str = heartbeat_payload.dump();
                    std::cout << "Sending heartbeat: " << heartbeat_str << std::endl;
                    client_handler_ptr->send(heartbeat_str);
                    std::cout << "Heartbeat sent." << std::endl;
                } else {
                    std::cout << "Websocket not connected, unable to send heartbeat." << std::endl;
                }
            }
        });
    }


    void DiscordClient::start_identify_thread() {
        std::cout << "Trying to identify" << std::endl;
        task_pool_->enqueue_task([this]() {
            std::cout << "Identifying..." << std::endl;
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
                std::cout << "Sending identify payload: " << identify_str << std::endl;
                client_handler_ptr->send(identify_str);
                std::cout << "Identify payload sent." << std::endl;
            }
            else
            {
                std::cout << "Identify payload sent." << std::endl;
            }
        });

        {
            std::unique_lock<std::mutex> lock(heartbeat_mutex_);
            identify_started_ = true;
            heartbeat_cv_.notify_one();
        }
    }

    void DiscordClient::reconnect(const std::string& uri) {
        std::cout << "Reconnecting..." << std::endl;
        client_handler_ptr.reset();
        client_handler_ptr = make_unique<websocket_handler::WebsocketClientHandler>(); 
        std::this_thread::sleep_for(std::chrono::seconds(5));
        connect(uri);
    }
    
    void DiscordClient::send_message(const std::string& channel_id, const std::string& message, const std::string& message_id)
    {
        std::string url = "https://discord.com/api/v10/channels/" + channel_id + "/messages";

        // Include the message_reference field in the JSON payload
        nlohmann::json payload = {
            {"content", message},
            {"message_reference", {
                {"message_id", message_id}
            }}
        };

        // Serialize the payload to a string
        std::string data = payload.dump();
        curlHandler->post(url, data, false);
    }

}
