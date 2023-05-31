#include "DiscordClient.hpp"
namespace discord
{

const int MAX_REQUESTS_PER_MINUTE = 60;
const int OP_HEARTBEAT = 1;
const int OP_IDENTIFY = 2;
const int OP_HELLO = 10;
const int OP_HEARTBEAT_ACK = 11;
const int GUILD_MESSAGES = (1 << 9);
const int MESSAGE_CONTENT = (1 << 15);
const int LARGE_THRESHOLD = 250;

const int WOKRER_THREAD_COUNT = 4;
const int EVENT_TREAD_COUNT = 4;
const int TASK_THREAD_COUNT = 2;

DiscordClient::DiscordClient(const std::string &bot_token, DiscordEvents::CheckIfIsAQuestion check_if_question,
                             DiscordEvents::ResponseCallback response_callback)
    : bot_token_(bot_token), check_if_question_(check_if_question), response_callback_(response_callback),
      curlHandler(std::make_unique<CurlHandler>())
{
    try
    {
        client_handler_ptr = std::make_unique<websocket_handler::WebsocketClientHandler>();
        curlHandler->AddHeader("Authorization: Bot " + bot_token_);
        curlHandler->AddHeader("Content-Type: application/json");

        event_handling_pool_ = std::make_unique<ThreadPool>(EVENT_TREAD_COUNT);
        worker_threads_pool_ = std::make_unique<ThreadPool>(WOKRER_THREAD_COUNT);
        task_pool_ = std::make_unique<ThreadPool>(TASK_THREAD_COUNT);

        active_threads_ = 0;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime error: " << e.what() << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred" << '\n';
    }
}

void DiscordClient::run()
{
    try
    {
        std::unique_lock<std::mutex> lk(mtx_run_);
        lk.unlock();

        connect("wss://gateway.discord.gg/?v=10&encoding=json");
        running_ = true;

        lk.lock();
        while (running_)
        {
            if (cv_run_.wait_for(lk, std::chrono::seconds(1), [this]() { return !running_; }))
                break;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in run: " << e.what() << std::endl;
        running_ = false;
    }
}

void DiscordClient::stop()
{
    running_ = false;
    cv_run_.notify_one();
}

DiscordClient::~DiscordClient()
{
    stop();
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return this->active_threads_ == 0; });
    std::cout << "DiscordClient destructor called" << std::endl;
}

void DiscordClient::connect(const std::string &uri)
{
    client_handler_ptr->connect(uri, headers);
    setup_handlers(uri);
    setup_event_handler();
    start_worker_threads();
}

void DiscordClient::setup_handlers(const std::string &uri)
{
    client_handler_ptr->set_open_handler([this]() { start_identify_thread(); });

    client_handler_ptr->set_fail_handler([this, uri]() {
        std::cout << "Reconnecting..." << std::endl;
        reconnect(uri);
    });

    client_handler_ptr->set_error_handler([this, uri](const std::string &error_message) {
        std::cerr << "WebSocket Error: " << error_message << std::endl;
        reconnect(uri);
    });

    client_handler_ptr->set_message_handler([this](const std::string &payload) { handle_payload(payload); });

    client_handler_ptr->set_close_handler([this, uri](int status, const std::string &reason) {
        std::cout << "WebSocket connection closed with status " << status << " and reason: " << reason << std::endl;
        std::cout << "Reconnecting..." << std::endl;
        reconnect(uri);
    });
}

void DiscordClient::increment_request_count()
{
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    std::chrono::seconds time_diff = std::chrono::duration_cast<std::chrono::seconds>(now - last_request_time_);

    // Reset request count every minute
    if (time_diff.count() >= MAX_REQUESTS_PER_MINUTE - 1)
    {
        request_count_ = 0;
        last_request_time_ = now;
    }

    request_count_++;
}

bool DiscordClient::can_send_request()
{
    // Limit to 60 requests per minute
    if (request_count_ >= MAX_REQUESTS_PER_MINUTE - 1)
    {
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
        std::chrono::seconds time_diff = std::chrono::duration_cast<std::chrono::seconds>(now - last_request_time_);

        if (time_diff.count() < MAX_REQUESTS_PER_MINUTE - 1)
        {
            return false;
        }

        // Reset request count if a minute has passed
        request_count_ = 0;
        last_request_time_ = now;
    }

    return true;
}

void DiscordClient::process_event(const nlohmann::json &data, bool is_update)
{
    event_handling_pool_->enqueue_task([this, data, is_update]() {
        try
        {
            nlohmann::json response;

            if (is_update)
            {
                response = event_handler_.on_message_update(data);
            }
            else
            {
                response = event_handler_.on_message_create(data, check_if_question_, response_callback_);
            }

            if (!response.empty())
            {
                std::lock_guard<std::mutex> lock(mutex_);
                event_queue_.push(response);
                condition_variable_.notify_one();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception processing event: " << e.what() << std::endl;
        }
        std::lock_guard<std::mutex> lock(mtx_);
        --active_threads_;
        if (active_threads_ == 0)
            cv_.notify_all();
    });
    std::lock_guard<std::mutex> lock(mtx_);
    ++active_threads_;
}

void DiscordClient::setup_event_handler()
{
    event_handler_.register_event_handler("MESSAGE_CREATE",
                                          [this](const nlohmann::json &data) { process_event(data, false); });

    event_handler_.register_event_handler("MESSAGE_UPDATE",
                                          [this](const nlohmann::json &data) { process_event(data, true); });

    event_handler_.register_event_handler("MESSAGE_DELETE", [this](const nlohmann::json &data) {
        nlohmann::json response = event_handler_.on_message_delete(data);
        if (!response.empty())
        {
            std::lock_guard<std::mutex> lock(mutex_);
            event_queue_.push(response);
            condition_variable_.notify_one();
        }
    });
}

void DiscordClient::start_worker_threads()
{
    for (unsigned int i = 0; i < WOKRER_THREAD_COUNT; ++i)
    {
        worker_threads_pool_->enqueue_task([this]() {
            while (running_)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_variable_.wait(lock, [this]() { return !event_queue_.empty() || !running_; });

                if (!running_)
                {
                    return;
                }

                if (!event_queue_.empty())
                {
                    nlohmann::json event_data = event_queue_.front();
                    event_queue_.pop();
                    lock.unlock();
                    if (!event_data.is_null())
                    {
                        if (event_data.value("action", "NOT FOUND") == "send_message")
                        {
                            send_message(event_data.value("channel_id", "NOT FOUND"),
                                         event_data.value("content", "NOT FOUND"),
                                         event_data.value("message_id", "NOT FOUND"));
                        }
                    }
                }
            }
            std::lock_guard<std::mutex> lock(mtx_);
            --active_threads_;
            if (active_threads_ == 0)
                cv_.notify_all();
        });
        std::lock_guard<std::mutex> lock(mtx_);
        ++active_threads_;
    }

    std::cout << "Discord bot connected" << std::endl;
}

void DiscordClient::handle_payload(const std::string &raw_payload)
{
    try
    {
        auto payload = nlohmann::json::parse(raw_payload);

        if (payload["op"] == OP_HEARTBEAT_ACK)
        {
            std::cout << "Received Heartbeat Acknowledgement" << std::endl;
        }
        else if (payload["op"] == OP_HELLO)
        { // Opcode 10: Hello
            int heartbeat_interval = payload["d"]["heartbeat_interval"];
            start_heartbeat_thread(heartbeat_interval);
        }
        else if (payload["t"] == "READY")
        {
            std::cout << "Received READY event" << std::endl;
        }
        else
        {
            event_handler_.handle_event(raw_payload, [](const std::string &error_message) {
                std::cerr << "Error handling event: " << error_message << std::endl;
            });
        }
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "Error parsing JSON payload: " << e.what() << std::endl;
    }
}

void DiscordClient::start_heartbeat_thread(int interval_ms)
{
    std::cout << "Trying to heartbeat" << std::endl;

    {
        std::unique_lock<std::mutex> lock(heartbeat_mutex_);
        heartbeat_cv_.wait(lock, [this]() { return identify_started_; });
    }

    task_pool_->enqueue_task([this, interval_ms]() {
        while (running_)
        {
            std::cout << "Heartbeat thread sleeping for " << interval_ms << " milliseconds" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
            nlohmann::json heartbeat_payload;
            heartbeat_payload["op"] = OP_HEARTBEAT; // Opcode for Heartbeat
            heartbeat_payload["d"] = last_sequence_;

            if (client_handler_ptr->is_connected())
            {
                std::string heartbeat_str = heartbeat_payload.dump();
                std::cout << "Sending heartbeat: " << heartbeat_str << std::endl;
                client_handler_ptr->send(heartbeat_str);
                std::cout << "Heartbeat sent." << std::endl;
            }
            else
            {
                std::cout << "Websocket not connected, unable to send heartbeat." << std::endl;
            }
        }
        std::lock_guard<std::mutex> lock(mtx_);
        --active_threads_;
        if (active_threads_ == 0)
            cv_.notify_all();
    });
    std::lock_guard<std::mutex> lock(mtx_);
    ++active_threads_;
}

void DiscordClient::start_identify_thread()
{
    std::cout << "Trying to identify" << std::endl;
    task_pool_->enqueue_task([this]() {
        std::cout << "Identifying..." << std::endl;
        nlohmann::json identify_payload{
            {"op", OP_IDENTIFY},
            {"d",
             {{"token", bot_token_},
              {"intents", GUILD_MESSAGES | MESSAGE_CONTENT}, // Add the GUILD_MESSAGES and MESSAGE_CONTENT intents
              {"properties", {{"$os", "linux"}, {"$browser", "disco"}, {"$device", "disco"}}},
              {"compress", false},
              {"large_threshold", LARGE_THRESHOLD},
              {"shard", {0, 1}}}}};
        if (client_handler_ptr->is_connected())
        {
            std::string identify_str = identify_payload.dump();
            std::cout << "Sending identify payload: " << identify_str << std::endl;
            client_handler_ptr->send(identify_str);
            std::cout << "Identify payload sent." << std::endl;
        }
        else
        {
            std::cout << "Identify payload sent error." << std::endl;
        }
        std::lock_guard<std::mutex> lock(mtx_);
        --active_threads_;
        if (active_threads_ == 0)
            cv_.notify_all();
    });
    std::lock_guard<std::mutex> lock(mtx_);
    ++active_threads_;
    {
        std::unique_lock<std::mutex> lock(heartbeat_mutex_);
        identify_started_ = true;
        heartbeat_cv_.notify_one();
    }
}

void DiscordClient::reconnect(const std::string &uri)
{
    std::cout << "Reconnecting..." << std::endl;
    client_handler_ptr.reset();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    connect(uri);
}

void DiscordClient::send_message(const std::string &channel_id, const std::string &message,
                                 const std::string &message_id)
{
    if (!can_send_request())
    {
        std::cerr << "Rate limit exceeded, cannot send message at this time" << std::endl;
        return;
    }

    increment_request_count();

    std::string url = "https://discord.com/api/v10/channels/" + channel_id + "/messages";

    nlohmann::json payload = {{"content", message}, {"message_reference", {{"message_id", message_id}}}};

    std::string data = payload.dump();

    curlHandler->enqueue_request([=]() {
        try
        {
            CurlHandler::Response response = curlHandler->post(url, data, false);
            if (response.http_code != 200)
            {
                std::cerr << "HTTP Error: " << response.http_code << " for message: " << message << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception sending message: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Unknown exception sending message" << std::endl;
        }
    });
}

} // namespace discord
