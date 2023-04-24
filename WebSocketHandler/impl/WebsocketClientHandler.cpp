#include "WebsocketClientHandler.hpp"

#include "utilities/utilities.hpp"

namespace websocket_handler {


    WebsocketClientHandler::WebsocketClientHandler(): client_ptr(make_unique<WebsocketClient>())
    {
        init();
    }
    WebsocketClientHandler::~WebsocketClientHandler()
    {
        delete client_ptr.get();
    }

    WebsocketClientHandler::InitializationStatus WebsocketClientHandler::init()
    {
        current_init_status = InitializationStatus::INITIALIZATION_FAILED;

        std::lock_guard<std::mutex> lock(m_client_handler_mutex);
        try
        {
            client_ptr->init();
            client_ptr->get_client().set_tls_init_handler([this](websocketpp::connection_hdl hdl) {
                return this->client_ptr->get_ssl_context_ptr(hdl);
            });
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

    void WebsocketClientHandler::connect(const std::string& uri, const std::map<std::string, std::string>& headers)
    {
        std::lock_guard<std::mutex> lock(m_client_handler_mutex);
        try {
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

            client_ptr->get_client().connect(con);
            client_ptr->get_client().run();
        } catch (websocketpp::exception const& e) {
            std::cout << "Error: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Other exception" << std::endl;
        }
    }

    void WebsocketClientHandler::on_open(websocketpp::connection_hdl hdl) {
        m_hdl = hdl;
    }

    void WebsocketClientHandler::send(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_client_handler_mutex);
        if (m_hdl.expired()) {
            std::cerr << "Error: connection handle is not valid" << std::endl;
            return;
        }
        websocketpp::connection_hdl hdl = m_hdl.lock();
        client::connection_ptr conn = client_ptr->get_client().get_con_from_hdl(hdl);

        try {
            client_ptr->get_client().send(hdl, message, websocketpp::frame::opcode::text);
        } catch (const websocketpp::exception& e) {
            std::cerr << "Error sending message: " << e.what() << std::endl;
        }
    }

    void WebsocketClientHandler::receive(websocketpp::connection_hdl, client::message_ptr msg) {
        std::cout << "Received: " << msg->get_payload() << std::endl;
    }
}
