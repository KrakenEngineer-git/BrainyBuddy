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
        enum class MessageStatus
        {
            NONE,
            NOT_SENT,
            SENT_SUCCESS,
            SENT_FAILURE,
            NOT_RECEIVED,
            RECEIVED_SUCCESS,
            RECEIVED_FAILURE
        };

        WebsocketClientHandler(const std::string& bot_token);
        ~WebsocketClientHandler();

        InitializationStatus init() override;
        void connect(const std::string& uri);
        void send(const std::string& message);
        void receive(websocketpp::connection_hdl, client::message_ptr msg);
        


    private:
        void on_open(websocketpp::connection_hdl hdl);
        websocketpp::connection_hdl m_hdl;
        WebsocketClient *client_ptr; 
        std::mutex m_client_handler_mutex;
        InitializationStatus current_init_status;
        const std::string bot_token_;
    };

}

#endif //WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENTHANDLER_HPP
