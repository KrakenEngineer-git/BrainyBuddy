#include "WebsocketClient.hpp"

WebsocketClient::WebsocketClient()
    : mSslContext(websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12_client))
{
    std::lock_guard<std::mutex> lock(connection_mutex_);
    try
    {
        ws_client_.init_asio();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

WebsocketClient::~WebsocketClient() = default;

void WebsocketClient::on_open(websocketpp::connection_hdl hdl)
{
    {
        std::lock_guard<std::mutex> lock(connection_mutex_);
        connection_hdl_ = hdl;
    }

    if (open_handler_)
    {
        open_handler_();
    }
}

void WebsocketClient::on_message(websocketpp::connection_hdl, client::message_ptr msg)
{
    if (message_handler_)
    {
        message_handler_(msg->get_payload());
    }
}

void WebsocketClient::on_close(websocketpp::connection_hdl hdl)
{
    {
        std::lock_guard<std::mutex> lock(connection_mutex_);
        connection_hdl_.reset();
    }

    websocketpp::close::status::value code = ws_client_.get_con_from_hdl(hdl)->get_local_close_code();
    std::string reason = ws_client_.get_con_from_hdl(hdl)->get_local_close_reason();

    if (close_handler_)
    {
        close_handler_(code, reason);
    }
}

void WebsocketClient::on_fail(websocketpp::connection_hdl hdl)
{
    {
        std::lock_guard<std::mutex> lock(connection_mutex_);
        connection_hdl_.reset();
    }

    if (fail_handler_)
    {
        fail_handler_();
    }

    websocketpp::lib::error_code ec;
    auto con = ws_client_.get_con_from_hdl(hdl, ec);
    if (ec)
    {
        std::cerr << "Error: unable to get connection information: " << ec.message() << std::endl;
    }
    else
    {
        std::string reason = con->get_ec().message();
        if (error_handler_)
        {
            error_handler_(reason);
        }
    }
}
