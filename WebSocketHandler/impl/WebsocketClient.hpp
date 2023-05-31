#ifndef WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENT_HPP
#define WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENT_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
class WebsocketClient
{
  public:
    WebsocketClient();
    ~WebsocketClient();

  protected:
    typedef std::function<void(const std::string &)> MessageHandler;
    typedef std::function<void()> ConnectionHandler;
    typedef std::function<void(int, const std::string &)> CloseHandler;
    typedef std::function<void(const std::string &error_message)> ErrorHandler;

    virtual void set_message_handler(MessageHandler handler) = 0;
    virtual void set_open_handler(ConnectionHandler handler) = 0;
    virtual void set_close_handler(CloseHandler handler) = 0;
    virtual void set_fail_handler(ConnectionHandler handler) = 0;
    virtual void set_error_handler(ErrorHandler handler) = 0;

    client ws_client_;
    websocketpp::connection_hdl connection_hdl_;
    websocketpp::lib::shared_ptr<boost::asio::ssl::context> mSslContext;
    void on_message(websocketpp::connection_hdl, client::message_ptr msg);
    void on_open(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);
    void on_fail(websocketpp::connection_hdl hdl);

    MessageHandler message_handler_;
    ConnectionHandler open_handler_;
    CloseHandler close_handler_;
    ConnectionHandler fail_handler_;
    ErrorHandler error_handler_;

  private:
    std::mutex connection_mutex_;
};

#endif // WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENT_HPP
