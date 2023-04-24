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
        WebsocketClientHandler();
        ~WebsocketClientHandler();

        InitializationStatus init() override;
        void connect(const std::string& uri, const std::map<std::string, std::string>& headers);
        void send(const std::string& message);
        void receive(websocketpp::connection_hdl, client::message_ptr msg);

        
        


    private:
        void on_open(websocketpp::connection_hdl hdl);
        websocketpp::connection_hdl m_hdl;
        std::unique_ptr<WebsocketClient> client_ptr;
        std::mutex m_client_handler_mutex;
        InitializationStatus current_init_status;
    };

}

#endif //WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENTHANDLER_HPP
