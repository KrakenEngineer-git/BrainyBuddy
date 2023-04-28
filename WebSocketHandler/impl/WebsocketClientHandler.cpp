#include "WebsocketClientHandler.hpp"

#include "utilities/utilities.hpp"

#include <nlohmann/json.hpp>

namespace websocket_handler {


    WebsocketClientHandler::WebsocketClientHandler(): client_ptr(make_unique<WebsocketClient>()),  reconnect_delay(5000)
    {
        init();
    }
    
    WebsocketClientHandler::InitializationStatus WebsocketClientHandler::init()
    {
        current_init_status = InitializationStatus::INITIALIZATION_FAILED;
        try
        {
            std::lock_guard<std::mutex> lock(m_client_handler_mutex);
            client_ptr->init();
            client_ptr->get_client().set_tls_init_handler([this](websocketpp::connection_hdl hdl) {
                return this->client_ptr->get_ssl_context_ptr(hdl);
            });
            client_ptr->get_client().set_access_channels(websocketpp::log::alevel::all);
            client_ptr->get_client().start_perpetual();
            client_ptr->get_client().set_close_handler(websocketpp::lib::bind(&WebsocketClientHandler::on_close, this, websocketpp::lib::placeholders::_1));
            client_ptr->get_client().set_fail_handler(websocketpp::lib::bind(&WebsocketClientHandler::on_fail, this, websocketpp::lib::placeholders::_1));
            client_ptr->get_client().set_open_handler(websocketpp::lib::bind(&WebsocketClientHandler::on_open, this, websocketpp::lib::placeholders::_1));
            client_ptr->get_client().set_message_handler(websocketpp::lib::bind(&WebsocketClientHandler::receive, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
            current_init_status = InitializationStatus::INITIALIZATION_SUCCESS;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return current_init_status;
    }

    WebsocketClientHandler::~WebsocketClientHandler()
    {
        std::lock_guard<std::mutex> lock(m_client_handler_mutex);
        client_ptr->get_client().clear_access_channels(websocketpp::log::alevel::frame_payload);
        client_ptr->get_client().stop_perpetual();
        client_ptr->get_client().close(m_hdl, websocketpp::close::status::normal, "Closing connection");
    }
    void WebsocketClientHandler::close_connection() {
        // Stop any ongoing reconnect attempts
        reconnect_callback_ = nullptr;
        std::lock_guard<std::mutex> lock(m_client_handler_mutex);
        client_ptr->get_client().close(m_hdl, websocketpp::close::status::normal, "Closing connection");
    }
    
    void WebsocketClientHandler::connect(const std::string& uri, const std::map<std::string, std::string>& headers)
    {
        try {
            std::lock_guard<std::mutex> lock(m_client_handler_mutex);
            websocketpp::lib::error_code ec;

            // Add the bot token to the connection request headers
            client::connection_ptr con = client_ptr->get_client().get_connection(uri, ec);
            
            for (const auto& header : headers) {
                con->append_header(header.first, header.second);
            }

            if (ec) {
                std::cout << "Error creating connection: " << ec.message() << std::endl;
                return;
            }
            std::cout << "Connecting to WebSocket..." << std::endl;
            client_ptr->get_client().connect(con);
            std::cout<<"Connected"<<std::endl;

            std::thread([this]() {
                client_ptr->get_client().run();
            }).detach();
            std::cout<<"Run"<<std::endl;
        } catch (websocketpp::exception const& e) {
            std::cout << "Error: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Other exception" << std::endl;
        }
    }

    void WebsocketClientHandler::reconnect() 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(reconnect_delay));
        if (reconnect_callback_) 
        {
            reconnect_callback_();
        }
    }

    void WebsocketClientHandler::on_open(websocketpp::connection_hdl hdl) {
        std::cout << "WebSocket connection opened" << std::endl;
        m_hdl = hdl;
        if (identify_callback_) {
            identify_callback_();
        }
    }
    void WebsocketClientHandler::on_close(websocketpp::connection_hdl hdl) {
        std::cout << "WebSocket connection closed" << std::endl;
        reconnect();
    }
    void WebsocketClientHandler::send(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_client_handler_mutex);
        if (m_hdl.expired()) {
            std::cerr << "Error: connection handle is not valid" << std::endl;
            return;
        }

        try {
            client_ptr->get_client().send(m_hdl.lock(), message, websocketpp::frame::opcode::text);
            std::cout << "Message send success" << std::endl;
        } catch (const websocketpp::exception& e) {
            std::cerr << "Error sending message: " << e.what() << std::endl;
        }
    }

    void WebsocketClientHandler::receive(websocketpp::connection_hdl, client::message_ptr msg) 
    {
        std::string raw_payload = msg->get_payload();
        if (payload_callback_) {
            payload_callback_(raw_payload);
        }
    }

    void WebsocketClientHandler::on_fail(websocketpp::connection_hdl hdl) {
        std::cout << "WebSocket connection failed" << std::endl;
        reconnect();
    }

    void WebsocketClientHandler::set_identify_callback(IdentifyCallback callback) {
        identify_callback_ = std::move(callback);
    }

    void WebsocketClientHandler::set_payload_callback(PayloadCallback callback) {
        payload_callback_ = std::move(callback);
    }

    void WebsocketClientHandler::set_reconnect_callback(ReconnectCallback callback) {
        reconnect_callback_ = std::move(callback);
    }

    bool WebsocketClientHandler::is_connected() 
    {
        std::lock_guard<std::mutex> lock(m_client_handler_mutex);
        return !m_hdl.expired();
    }
}
