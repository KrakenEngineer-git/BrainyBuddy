#ifndef WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENTHANDLER_HPP
#define WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENTHANDLER_HPP

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include "WebsocketClient.hpp"
#include "WebSocketHandler/WebsocketClientBase.hpp"

#include <iostream>
#include <mutex>
#include <memory>

namespace websocket_handler {

    class WebsocketClientHandler : public WebsocketClientBase {

    public:
        using IdentifyCallback = std::function<void()>;
        using PayloadCallback = std::function<void(const std::string&)>;
        using ReconnectCallback = std::function<void()>;


        WebsocketClientHandler();
        ~WebsocketClientHandler();

        void connect(const std::string& uri, const std::map<std::string, std::string>& headers);
        void send(const std::string& message);
        void receive(websocketpp::connection_hdl, client::message_ptr msg);
        void on_close(websocketpp::connection_hdl hdl);
        void on_fail(websocketpp::connection_hdl hdl);
        void set_identify_callback(IdentifyCallback callback);
        void set_payload_callback(PayloadCallback callback);
        void set_reconnect_callback(ReconnectCallback callback);
        void WebsocketClientHandler::reconnect();
        bool is_connected();

    private:
        void close_connection();
        std::atomic<int> reconnect_delay;
        ReconnectCallback reconnect_callback_;
        InitializationStatus init() override;
        void on_open(websocketpp::connection_hdl hdl);
        websocketpp::connection_hdl m_hdl;
        std::unique_ptr<WebsocketClient> client_ptr;
        std::mutex m_client_handler_mutex;
        InitializationStatus current_init_status;
        IdentifyCallback identify_callback_;
        PayloadCallback payload_callback_;
    };

}

#endif //WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENTHANDLER_HPP
