#ifndef WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENT_HPP
#define WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENT_HPP

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include "WebSocketHandler/WebsocketClientBase.hpp"

#include <iostream>
#include <mutex>
#include <memory>

namespace websocket_handler {

    typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

    class WebsocketClient : public WebsocketClientBase{

    public:

        WebsocketClient() : current_init_status(InitializationStatus::NOT_INITIALIZED) {};

        InitializationStatus init() override;

        websocketpp::lib::shared_ptr<boost::asio::ssl::context> get_ssl_context_ptr(websocketpp::connection_hdl hdl);

        client& get_client();

    private:
        client m_client;
        std::mutex m_client_mutex;
        websocketpp::lib::shared_ptr<boost::asio::ssl::context> ssl_context_ptr;
        InitializationStatus current_init_status;
    };

}

#endif //WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENT_HPP

