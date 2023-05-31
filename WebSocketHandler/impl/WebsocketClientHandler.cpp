#include "WebsocketClientHandler.hpp"
namespace websocket_handler
{

WebsocketClientHandler::WebsocketClientHandler()
{
    set_client_handlers();
}

WebsocketClientHandler::~WebsocketClientHandler() = default;

void WebsocketClientHandler::set_client_handlers()
{
    std::lock_guard<std::mutex> lock(m_client_handler_mutex);
    try
    {
        run_thread_pool_ = std::make_unique<ThreadPool>(1);
        ws_client_.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect);
        ws_client_.set_error_channels(websocketpp::log::elevel::info | websocketpp::log::elevel::warn |
                                      websocketpp::log::elevel::rerror | websocketpp::log::elevel::fatal);
        ws_client_.set_open_handler(std::bind(&WebsocketClientHandler::on_open, this, std::placeholders::_1));
        ws_client_.set_close_handler(std::bind(&WebsocketClientHandler::on_close, this, std::placeholders::_1));
        ws_client_.set_fail_handler(std::bind(&WebsocketClientHandler::on_fail, this, std::placeholders::_1));
        ws_client_.set_message_handler(
            std::bind(&WebsocketClientHandler::on_message, this, std::placeholders::_1, std::placeholders::_2));
        ws_client_.set_tls_init_handler([this](websocketpp::connection_hdl hdl) { return mSslContext; });
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        throw;
    }
}

void WebsocketClientHandler::connect(const std::string &uri, const std::map<std::string, std::string> &headers)
{
    try
    {
        std::lock_guard<std::mutex> lock(m_client_handler_mutex);
        websocketpp::lib::error_code ec;
        client::connection_ptr con = ws_client_.get_connection(uri, ec);

        for (const auto &header : headers)
        {
            con->append_header(header.first, header.second);
        }

        if (ec)
        {
            std::cerr << "Error creating connection: " << ec.message() << std::endl;
            return;
        }

        std::cout << "Connecting to WebSocket..." << std::endl;
        ws_client_.connect(con);
        std::cout << "Connected!" << std::endl;

        std::cout << "Trying to run..." << std::endl;
        run_thread_pool_->enqueue_task([this]() { ws_client_.run(); });
        std::cout << "Runned!" << std::endl;
    }
    catch (websocketpp::exception const &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Other exception" << std::endl;
    }
}

void WebsocketClientHandler::send(const std::string &message)
{
    std::lock_guard<std::mutex> lock(m_client_handler_mutex);

    if (connection_hdl_.expired())
    {
        std::cerr << "Error: connection handle is not valid" << std::endl;
        return;
    }

    try
    {
        ws_client_.send(connection_hdl_.lock(), message, websocketpp::frame::opcode::text);
    }
    catch (const websocketpp::exception &e)
    {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }
}

bool WebsocketClientHandler::is_connected()
{
    std::lock_guard<std::mutex> lock(m_client_handler_mutex);
    return !connection_hdl_.expired();
}

void WebsocketClientHandler::set_message_handler(MessageHandler handler)
{
    message_handler_ = std::move(handler);
}

void WebsocketClientHandler::set_open_handler(ConnectionHandler handler)
{
    open_handler_ = std::move(handler);
}

void WebsocketClientHandler::set_close_handler(CloseHandler handler)
{
    close_handler_ = std::move(handler);
}

void WebsocketClientHandler::set_fail_handler(ConnectionHandler handler)
{
    fail_handler_ = std::move(handler);
}

void WebsocketClientHandler::set_error_handler(ErrorHandler handler)
{
    error_handler_ = std::move(handler);
}

} // namespace websocket_handler
