#include "WebsocketClient.hpp"

namespace websocket_handler {

    WebsocketClient::InitializationStatus WebsocketClient::init()
    {
        current_init_status = InitializationStatus::INITIALIZATION_FAILED;

        std::lock_guard<std::mutex> lock(m_client_mutex);
        try
        {
            ssl_context_ptr = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12_client);
            m_client.init_asio();
            current_init_status = InitializationStatus::INITIALIZATION_SUCCESS;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return current_init_status;
    }

    websocketpp::lib::shared_ptr<boost::asio::ssl::context> WebsocketClient::get_ssl_context_ptr(websocketpp::connection_hdl hdl) 
    {
        if(ssl_context_ptr != nullptr && current_init_status == InitializationStatus::INITIALIZATION_SUCCESS)
        {
            return ssl_context_ptr;
        }
        throw std::runtime_error("Failed to get SSL context pointer: WebsocketClient not initialized or SSL context is null.");
    }

    client& WebsocketClient::get_client()
    {
        if(ssl_context_ptr != nullptr && current_init_status == InitializationStatus::INITIALIZATION_SUCCESS)
        {
            return m_client;
        }
        throw std::runtime_error("WebsocketClient not properly initialized");
    }
}